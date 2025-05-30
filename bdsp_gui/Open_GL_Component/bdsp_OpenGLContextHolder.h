#pragma once

//adapted from 
// https://forum.juce.com/t/opengl-how-do-3d-with-custom-shaders-and-2d-with-juce-paint-methods-work-together/28026/6
namespace bdsp
{

	class OpenGLComponent;

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
		void registerOpenGlRenderer(OpenGLComponent* child);

		void unregisterOpenGlRenderer(OpenGLComponent* child);

		void setBackgroundColour(const juce::Colour c);

		bool getComponentRegistrationStatus(OpenGLComponent* c);
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

			Client(OpenGLComponent* comp, State nextStateToUse = State::suspended);


			OpenGLComponent* c = nullptr;
			State currentState = State::suspended, nextState = State::suspended;
		};

		juce::CriticalSection stateChangeCriticalSection;
		juce::OwnedArray<Client, juce::CriticalSection> clients;

		//==============================================================================
		int findClientIndexForComponent(OpenGLComponent* comp) const;

		Client* findClientForComponent(OpenGLComponent* comp) const;

		//==============================================================================
		juce::Colour backgroundColour{ juce::Colours::black };

		struct ClientSorter
		{
			int compareElements(Client* first, Client* second);
		};

		ClientSorter sorter;
	};
} // namespace bdsp
