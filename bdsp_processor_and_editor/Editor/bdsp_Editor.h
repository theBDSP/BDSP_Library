#pragma once

#define SettingsButtonOutlineRatio 0.01


namespace bdsp
{
	class Editor : public juce::AudioProcessorEditor, public juce::DragAndDropContainer, public juce::ValueTree::Listener, public juce::FileDragAndDropTarget
	{
	public:
		Editor(Processor& p);

		virtual ~Editor() override;

		//==============================================================================
		void resized() override;
		void paintOverChildren(juce::Graphics& g) override;

		void closeAllMenus();

		void mouseDown(const juce::MouseEvent&)override;
		void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& mw)override;

		void paintDemoSilencingMessage(juce::Graphics& g);

#if BDSP_DEMO_VERSION
		void demoDisable(bool enable); // disables/reenables plugin during demo silences
#endif

		void loadSettings();

		virtual void setNoBPM();

		virtual void setUniversals() = 0;

		virtual void resizeForSidebar(bool expand); // resizing and positioning of settings menu

		void setMainAreaSize(int width, int height);


		void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;


		void setScaleFactor(float newScale) override;


		int prevUIScaleFactor = 2;



		GUI_Universals GUIUniversals;
		std::unique_ptr<LinkableControlComponents> linkables;




		// This reference is provided as a quick way for your editor to
		// access the processor object that created it.
		Processor& audioProcessor;


		double sampleRate = 44100.0;














		juce::ComponentBoundsConstrainer boundsConstrainer;

		class CornerComponent : public juce::ResizableCornerComponent, public ComponentCore
		{
		public:
			CornerComponent(GUI_Universals* universalsToUse, juce::Component* componentToResize, juce::ComponentBoundsConstrainer* constrainer)
				:juce::ResizableCornerComponent(componentToResize, constrainer),
				ComponentCore(this, universalsToUse)
			{

			}
			void paint(juce::Graphics& g) override
			{
				g.setColour(getColor(color));

				juce::Path p;
				p.startNewSubPath(0, getHeight());
				p.lineTo(getWidth(), 0);
				p.lineTo(getWidth(), getHeight());
				p.closeSubPath();
				g.fillPath(p);
			}


			void mouseEnter(const juce::MouseEvent& e) override
			{

				setMouseCursor(juce::MouseCursor::BottomRightCornerResizeCursor);
			}
			void mouseExit(const juce::MouseEvent& e) override
			{
				setMouseCursor(juce::MouseCursor::NormalCursor);
			}
			NamedColorsIdentifier color;

		}corner;


#if BDSP_DEMO_VERSION
		std::unique_ptr<DemoAlert<float>>demoAlert;
		bool demoIsSilencing = false;
#endif

		void dimPlugin(bool dim);





		//class BPMTimer : public juce::Timer
		//{
		//public:
		//	BPMTimer(Editor* p)
		//	{
		//		parent = p;
		//		processor = &parent->audioProcessor;

		//		startTimer(250);
		//	}
		//	void timerCallback()
		//	{
		//		if (processor->firstProcessLoop)
		//		{
		//			if (!processor->hasPlayHead)
		//			{
		//				parent->Alert->addMessageToQueue("Ok", "BPM Not Detected", "Any lines set to tempo sync mode will instead use their non-synced delay times", AlertWindow::AlertItemPriority::Highest);
		//				parent->setNoBPM();
		//			}
		//			stopTimer();

		//		}
		//	}

		//private:
		//	Editor* parent;
		//	Processor* processor;
		//} BPMTime;


		MacroSection* Macros;
		LFOSection* LFOs;
		EnvelopeFollowerSection<float>* EnvelopeFollowers;

	protected:
		void init();
		void startOpenGlAnimation();
		void shutdownOpenGlAnimation();




		class GUI_Listener : public GUI_Universals::Listener
		{
		public:
			GUI_Listener(Editor* e, GUI_Universals* universalsToUse)
				:GUI_Universals::Listener(universalsToUse)
			{
				editor = e;
			}

			void GUI_UniversalsChanged() override
			{
				if (onUniversalsChange.operator bool())
				{
					onUniversalsChange();


					editor->repaint();

				}
			}


			std::function<void()> onUniversalsChange;
		private:
			Editor* editor;
		};

		std::unique_ptr<GUI_Listener> GUIListener;


		class TopLevelGUIComponent : public juce::Component
		{
		public:
			TopLevelGUIComponent(Editor* e);
			virtual ~TopLevelGUIComponent() = default;
			void resized() override;

			void addComponent(juce::Component* c, bool shouldBeVisible = true, bool inMainArea = true);

			void paintDimmedPlugin(juce::Graphics& g);

			double aspectRatio;
			double sidebarAspectRatio;
			bool sidebarOpen = false;
			bool pluginIsDimmed = false;


			std::unique_ptr<MultiShapeButton> sideBarContainerCloser;
			std::unique_ptr<PresetManager> preset;
			std::unique_ptr<SettingsMenu> settings;
			MultiShapeButton settingsButton;

			std::unique_ptr<AlertWindow> Alert;
			std::unique_ptr<UndoRedoButtons> undoRedo;

			std::unique_ptr<BPMComponent> BPM;

			std::unique_ptr<Logo> logo;

			juce::Component mainArea; // hold all of the plugin except settings menu and  preset browser
			std::unique_ptr<TexturedContainerComponent> sideBarContainer;


			std::function<void(juce::Graphics&)> paintOverChildrenFunction;

		private:
			void paintOverChildren(juce::Graphics& g) override;

		};
		std::unique_ptr<TopLevelGUIComponent> topLevelComp;

	public:
		TopLevelGUIComponent* getTopLevelGUIComponent();





		GlContextHolder contextHolder;
		std::unique_ptr<ControlTransferPopupMenu> controlTransferMenu;

	private:

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
			JUCE_DECLARE_WEAK_REFERENCEABLE(Editor)


			// Inherited via FileDragAndDropTarget
			bool isInterestedInFileDrag(const juce::StringArray& files) override;

		void filesDropped(const juce::StringArray& files, int x, int y) override;

	};


}// namnepace bdsp
