#pragma once

//adapted from 
// https://forum.juce.com/t/opengl-how-do-3d-with-custom-shaders-and-2d-with-juce-paint-methods-work-together/28026/6
namespace bdsp
{

	class GlContextHolder
		: private juce::ComponentListener,
		private juce::OpenGLRenderer
	{
	public:
		// static GlContextHolder* globalContextuniversals;

		GlContextHolder(juce::Component& topLevelComponent);

		~GlContextHolder();

		//==============================================================================
		// The context universals MUST explicitely call detach in their destructor
		void detach();

		//==============================================================================
		// Clients MUST call unregisterOpenGlRenderer manually in their destructors!!
		void registerOpenGlRenderer(juce::Component* child);

		void unregisterOpenGlRenderer(juce::Component* child);

		void setBackgroundColour(const juce::Colour c);

		bool getComponentRegistrationStatus(juce::Component* c);
		juce::OpenGLContext context;


	private:
		//==============================================================================
		void checkComponents(bool isClosing, bool isDrawing);

		//==============================================================================

		void componentBeingDeleted(juce::Component& component) override;

		//==============================================================================
		void newOpenGLContextCreated() override;

		void renderOpenGL() override;

		void openGLContextClosing() override;

		//==============================================================================
		juce::Component& parent;

		struct Client
		{
			enum class State
			{
				suspended,
				running
			};

			Client(juce::Component* comp, State nextStateToUse = State::suspended);


			juce::Component* c = nullptr;
			State currentState = State::suspended, nextState = State::suspended;
		};

		juce::CriticalSection stateChangeCriticalSection;
		juce::OwnedArray<Client, juce::CriticalSection> clients;

		//==============================================================================
		int findClientIndexForComponent(juce::Component* comp) const;

		Client* findClientForComponent(juce::Component* comp) const;

		//==============================================================================
		juce::Colour backgroundColour{ juce::Colours::black };
	};
} // namespace bdsp
