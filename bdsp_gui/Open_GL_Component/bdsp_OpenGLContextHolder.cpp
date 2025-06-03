#include "bdsp_OpenGLContextHolder.h"
namespace bdsp
{

	GlContextHolder::GlContextHolder(juce::Component& topLevelComponent)
		: parent(topLevelComponent)
	{
		/*jassert(globalContextuniversals == nullptr);
		globalContextuniversals = this;*/


		juce::OpenGLPixelFormat pixelFormat;
		pixelFormat.multisamplingLevel = 4; // Change this value to your needs.
		pixelFormat.stencilBufferBits = 8;
		context.setPixelFormat(pixelFormat);

		context.setSwapInterval(0);

#if DEBUG
		context.setOpenGLVersionRequired(juce::OpenGLContext::openGL4_3);
#else
		context.setOpenGLVersionRequired(juce::OpenGLContext::openGL4_1);
#endif


		context.setRenderer(this);
		context.setContinuousRepainting(false);
		context.setComponentPaintingEnabled(true);
		context.attachTo(parent);
	}

	GlContextHolder::~GlContextHolder()
	{
		detach();
	}

	//==============================================================================
	// The context universals MUST explicitely call detach in their destructor

	void GlContextHolder::detach()
	{
		jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

		const int n = clients.size();
		for (int i = 0; i < n; ++i)
		{
			juce::Component* comp = clients.getUnchecked(i)->c;
			if (comp != nullptr)
			{
				comp->removeComponentListener(this);
			}

		}

		context.detach();
		context.setRenderer(nullptr);
	}

	//==============================================================================
	// Clients MUST call unregisterOpenGlRenderer manually in their destructors!!

	void GlContextHolder::registerOpenGlRenderer(OpenGLComponent* child)
	{
		jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());

		if (dynamic_cast<juce::OpenGLRenderer*> (child) != nullptr)
		{
			if (findClientIndexForComponent(child) < 0)
			{
				clients.add(new Client(child, (parent.isParentOf(child) ? Client::State::running : Client::State::suspended)));
				child->addComponentListener(this);
			}
		}
		else
			jassertfalse;
	}

	void GlContextHolder::unregisterOpenGlRenderer(OpenGLComponent* child)
	{
		jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());


		const int index = findClientIndexForComponent(child);

		if (index >= 0)
		{
			Client* client = clients.getUnchecked(index);
			{
				juce::ScopedLock stateChangeLock(stateChangeCriticalSection);
				client->nextState = Client::State::suspended;
			}

			child->removeComponentListener(this);
			if (context.isAttached())
			{
				context.executeOnGLThread([this](juce::OpenGLContext&)
				{
					checkComponents(false, false);
				}, true);
				client->c = nullptr;

			}
			clients.remove(index);
		}
	}

	void GlContextHolder::setBackgroundColour(const juce::Colour c)
	{
		backgroundColour = c;
	}

	bool GlContextHolder::getComponentRegistrationStatus(OpenGLComponent* c)
	{
		return findClientIndexForComponent(c) > -1;
	}

	//==============================================================================

	void GlContextHolder::checkComponents(bool isClosing, bool isDrawing)
	{
		juce::Array<Client*> initClients, runningClients;

		{
			juce::ScopedLock arrayLock(clients.getLock());
			juce::ScopedLock stateLock(stateChangeCriticalSection);

			const int n = clients.size();

			for (int i = 0; i < n; ++i)
			{
				Client* client = clients.getUnchecked(i);
				client->nextState = (client->c->isShowing() ? Client::State::running : Client::State::suspended);

				if (client->c != nullptr)
				{
					Client::State nextState = (isClosing ? Client::State::suspended : client->nextState);

					if (client->currentState == Client::State::running && nextState == Client::State::running)   runningClients.add(client);
					else if (client->currentState == Client::State::suspended && nextState == Client::State::running)   initClients.add(client);
					else if (client->currentState == Client::State::running && nextState == Client::State::suspended)
					{
						dynamic_cast<juce::OpenGLRenderer*> (client->c)->openGLContextClosing();
					}

					client->currentState = nextState;
				}
			}
		}

		for (int i = 0; i < initClients.size(); ++i)
			initClients.getReference(i)->c->newOpenGLContextCreated();

		if (runningClients.size() > 0 && isDrawing)
		{
			runningClients.sort(sorter, true);
			const float displayScale = juce::jmax(static_cast<float>(context.getRenderingScale()), juce::Desktop::getInstance().getGlobalScaleFactor());
			const juce::Rectangle<int> parentBounds = expandRectangleToInt(parent.getLocalBounds().toFloat() * displayScale);

			for (int i = 0; i < runningClients.size(); ++i)
			{
				OpenGLComponent* comp = runningClients.getReference(i)->c;

				juce::Rectangle<int> r = expandRectangleToInt(parent.getLocalArea(comp, comp->getLocalBounds()).toFloat() * displayScale);

				if (!r.isEmpty())
				{
					juce::gl::glViewport((GLint)r.getX(),
						(GLint)parentBounds.getHeight() - (GLint)r.getBottom(),
						(GLsizei)r.getWidth(), (GLsizei)r.getHeight());

					juce::OpenGLHelpers::enableScissorTest(juce::Rectangle<int>((GLint)r.getX(),
						(GLint)parentBounds.getHeight() - (GLint)r.getBottom(),
						(GLsizei)r.getWidth(), (GLsizei)r.getHeight()));
					//juce::OpenGLHelpers::clear(backgroundColour);

					comp->renderOpenGL();
					juce::gl::glDisable(juce::gl::GL_SCISSOR_TEST);
				}
			}
		}
	}

	//==============================================================================


	void GlContextHolder::componentBeingDeleted(juce::Component& component)
	{
		auto cast = dynamic_cast<OpenGLComponent*>(&component);
		const int index = findClientIndexForComponent(cast);

		if (index >= 0)
		{
			Client* client = clients.getUnchecked(index);

			// You didn't call unregister before deleting this component
			jassert(client->nextState == Client::State::suspended);
			client->nextState = Client::State::suspended;

			component.removeComponentListener(this);
			context.executeOnGLThread([this](juce::OpenGLContext&)
			{
				checkComponents(false, false);
			}, true);

			client->c = nullptr;

			clients.remove(index);
		}
	}

	//==============================================================================

	void GlContextHolder::newOpenGLContextCreated()
	{
		checkComponents(false, false);
	}

	void GlContextHolder::renderOpenGL()
	{



		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);
		//juce::OpenGLHelpers::clear(backgroundColour);
		checkComponents(false, true);
	}

	void GlContextHolder::openGLContextClosing()
	{
		checkComponents(true, false);
	}

	//==============================================================================

	int GlContextHolder::findClientIndexForComponent(OpenGLComponent* comp) const
	{
		const int n = clients.size();
		for (int i = 0; i < n; ++i)
			if (comp == clients.getUnchecked(i)->c)
			{
				return i;
			}
		return -1;
	}

	GlContextHolder::Client* GlContextHolder::findClientForComponent(OpenGLComponent* comp) const
	{
		const int index = findClientIndexForComponent(comp);
		if (index >= 0)
		{
			return clients.getUnchecked(index);
		}


		return nullptr;
	}

	GlContextHolder::Client::Client(OpenGLComponent* comp, State nextStateToUse)
		: c(comp), currentState(State::suspended), nextState(nextStateToUse)
	{

	}
	int GlContextHolder::ClientSorter::compareElements(Client* first, Client* second)
	{
		return first->c->glDepthOrder - second->c->glDepthOrder;
	}
} // namespace bdsp
