#pragma once


namespace bdsp
{

	class RepaintThreadChecker // needs to be owned outside of the editor to check if repaint is getting called on a deleted component
	{
	public:

		RepaintThreadChecker()
		{

		}

		void callRepaint(juce::WeakReference<juce::Component> compToRepaint)
		{

			std::function<void()> func = [=]()
			{
				if (compToRepaint != nullptr)
				{
					compToRepaint->repaint();
				}

			};
			juce::MessageManager::getInstance()->callAsync(func);
		}


	};

	enum SliderKeyBindings { SingleClickReset, SingleClickFineControl, DoubleClickTextEdit };
	struct GUI_Universals // stuff that every/most GUI components will need (colors, fonts, etc.)
	{
		GUI_Universals()
		{
			listeners.clear();
			lnf.universals = this;
		}

		~GUI_Universals()
		{

		}



		class Listener
		{
		public:

			Listener(GUI_Universals* universalsToUse)
			{
				universalsPTR = universalsToUse;
				universalsPTR->addListener(this);
			}
			virtual ~Listener()
			{
				universalsPTR->removeListener(this);
			}
			virtual void GUI_UniversalsChanged() = 0;
			virtual void keyBindingChanged(int idx) {};


			GUI_Universals* universalsPTR;
		private:
		};

		void addListener(Listener* l)
		{
			listeners.addIfNotAlreadyThere(l);
		}

		void removeListener(Listener* l)
		{
			listeners.removeAllInstancesOf(l);
		}

		void schemeChanged()
		{
			for (auto l : listeners)
			{
				l->GUI_UniversalsChanged();
			}
		}
		void bindingChanged(int idx)
		{
			for (auto l : listeners)
			{
				l->keyBindingChanged(idx);
			}
		}



		ColorScheme colors;
		juce::Array<Font> Fonts; // default to first font in array
		juce::Array<juce::ModifierKeys::Flags> bindings;



		CommonPaths commonPaths;
		struct ControlUniversalValues
		{
			float containerSliderHeightRatio = 0.5f;
			float visualizerHieghtRatio = 0.45f;

		}controlUniversals;

		std::function<juce::Colour(const juce::Colour&, bool)> hoverAdjustmentFunc = [=](const juce::Colour& c, bool isHover)
		{
			return c.withMultipliedBrightness(isHover ? 1.15f : 1.0f);
		};

		std::function<juce::Colour(const juce::Colour&)> textureHighlightAdjustmentFunc = [=](const juce::Colour& c)
		{
			return c.withBrightness(c.getBrightness() - 0.075);
		};

		std::function<juce::Colour(const juce::Colour&)> textHighlightFunc = [=](const juce::Colour& c)
		{
			return c.withAlpha(0.25f);
		};



		struct SliderClipboard  // stores values and modulation values copied to clipboard
		{
			double normalizedValue = -1;
			juce::Array<double> modulationAmounts;

		}sliderClipboard;

		NamedColorsIdentifier sliderHoverMenuBGKD = BDSP_COLOR_BLACK;
		NamedColorsIdentifier sliderHoverMenuOutline = BDSP_COLOR_LIGHT;

		juce::NormalisableRange<float> freqRange;

		//================================================================================================================================================================================================



		float disabledAlpha = 0.25f;
		float hoverMixAmt = 0.4f;
		float buttonDesaturation = 0.25f;
		float roundedRectangleCurve = 0;
		float dividerSize = 0;
		float listBorderSize = 0;
		float rectThicc = 0;
		float dividerBorder = 0;
		float lowOpacity = 0.2f;
		float midOpacity = 0.5f;
		float visualizerLineThickness = 0.0f;
		double expectedFrameTimeMS = 1000.0 / 100.0;

		float sliderPopupWidth = 0;

		float systemScaleFactor = 1;


		int influenceHoverMenuHeight = 0;


		juce::Image texture;


		//Owned by the Processor, have pointer here for components to use
		RepaintThreadChecker* repaintChecker = nullptr;
		juce::AudioProcessorValueTreeState* APVTS = nullptr;
		PropertiesFolder* propertiesFolder = nullptr;

		//================================================================================================================================================================================================
		//Owned by the Editor, have pointer here for components to use


		juce::UndoManager* undoManager = nullptr;

		GlContextHolder* contextHolder = nullptr;
		juce::OpenGLContext* openGLContext = nullptr;
		ControlTransferPopupMenu* controlPopup = nullptr;
		LinkableControlComponents* controlComps;

		//================================================================================================================================================================================================
		
		std::unique_ptr<HintBar> hintBar;


		DesktopComponentManager desktopManager;

		NotePaths MasterNotePaths;


		juce::Array<LinkableControl*> controls;





		AnimationTimer animationTimer;

		LookAndFeel lnf;

	private:

		juce::Array<Listener*> listeners;
	};



}// namnepace bdsp









