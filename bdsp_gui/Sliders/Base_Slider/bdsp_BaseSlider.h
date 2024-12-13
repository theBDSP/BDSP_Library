#pragma once



#define BDSP_FINE_MOUSE_WHEEL_REDUCTION_FACTOR 20

namespace bdsp
{
	//provides basic double click functionality
	//override showTextEditor fucntion to alter double click functionality
	class BaseSlider : public juce::Slider, public ComponentCore, public GUI_Universals::Listener, juce::AudioParameterFloat::Listener
	{
	public:
		BaseSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		BaseSlider(const BaseSlider& other);
		~BaseSlider();




		enum Type { Normal, CenterZero, CenterMirrored };
		void setType(Type newType);
		Type getType();

		enum KnobPlacement { Above, Below, Left, Right };
		void setKnobPlacement(KnobPlacement p);
		KnobPlacement getKnobPlacement();

		void setUnboundedMouseDrag(bool state);

		juce::Rectangle<int> getBoundsToIncludeLabel(); // retrun bounds expanded to fit influenceLabel if there is one

		virtual void setNamesAndIDs(const juce::String& baseName);

		virtual void showTextEditor();
		virtual void attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterID);
		juce::RangedAudioParameter* getParameter();

		virtual void attachLabel();
		virtual void updateLabel();
		bool hasLabel();

		double getDefaultValue();

		double getActualValue(); // for templated subclasses when actual value is needed and it is unknown whether influence is ranged or not (SHOULD ONLY BE CALLED FROM THE MESSAGE THREAD)



		double snapValue(double inputVal, juce::Slider::DragMode mode) override;

		void addCatchValues(const juce::Array<double>& newValues, bool isNormalized = true);


		std::function<double(double, juce::Slider::DragMode)> snapToValueFunction;


		void enablementChanged() override;
		void visibilityChanged() override;


		void mouseDown(const juce::MouseEvent& e) override;
		void mouseUp(const juce::MouseEvent& e) override;
		void mouseDrag(const juce::MouseEvent& e) override;
		void mouseDoubleClick(const juce::MouseEvent& e) override;
		void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw) override;


		void setChangeNotificationOnlyOnRelease(bool onlyNotifyOnRelease);

		void modifierKeysChanged(const juce::ModifierKeys& mod) override;


		virtual void setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack = NamedColorsIdentifier(), const NamedColorsIdentifier& valueTrackInside = NamedColorsIdentifier(), const NamedColorsIdentifier& text = NamedColorsIdentifier(), const NamedColorsIdentifier& textHighlight = NamedColorsIdentifier(), const NamedColorsIdentifier& textEdit = NamedColorsIdentifier(), const NamedColorsIdentifier& caret = NamedColorsIdentifier(), const NamedColorsIdentifier& highlight = NamedColorsIdentifier());

		void setSliderColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& caret, float altOpactiy = -1.0f); // keps all other defaults but changes value color and other colors associated with value


		std::function<void()> onAttach;
		std::function<double()> getValueFunction;
		std::function<juce::Path(double)> pathFromValueFunction;

		juce::Path labelPath;
		bool paintPointer = true;

		bool rotary = false;



		void setFontIndex(int idx) override;
		class Label : public bdsp::Label, juce::Label::Listener, juce::Slider::Listener
		{
		public:
			Label(BaseSlider* s);

			~Label();
			void setAsMainLabel();


			void mouseDoubleClick(const juce::MouseEvent& e) override;

			void labelTextChanged(juce::Label* labelThatHasChanged) override;
			void editorShown(juce::Label* labelThatHasChanged, juce::TextEditor& te) override;
			void editorHidden(juce::Label* labelThatHasChanged, juce::TextEditor& te) override;

			void sliderValueChanged(juce::Slider* slider) override;

			juce::TextEditor* createEditorComponent() override;

			// textUp updated externally b/c of ranged sliders messing things up

			void setHasNoTitle(bool state); // true to make them both invisible false to set title back to visible
			bool getHasNoTitle(); 

			void visibilityChanged() override;

			void parentHierarchyChanged() override;

			void lookAndFeelChanged() override;

			void resized() override;

			void setTitle(const juce::String& s);
			bdsp::Label& getTitle();

			void colourChanged() override;

			void paint(juce::Graphics& g) override;


			bool desiredHoverState = false;
		protected:





			bdsp::Label title;

			bool hasNoTitle = false;

			BaseSlider& parentSlider;

		};

		;


		BaseSlider::Label* mainLabel = nullptr; // holds a pointer to the influenceLabel that is being used in place of the textBox

	protected:


		juce::RangedAudioParameter* param = nullptr;

		Type type = Normal;
		KnobPlacement place;

		bool unboundedMouseDrag = false;


		std::unique_ptr<SliderAttachment> attachment;

		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

		juce::Array<double> catchValues; // values at which the slider should pause while dragging
	public:
		NamedColorsIdentifier knobColor, valueColor, valueTrackColor, valueTrackInsideColor, textColor, textHighlightColor, textEditColor, caretColor, highlightColor;

		std::unique_ptr<SliderPopupMenu> popup;




		// Inherited via Listener
		void parameterValueChanged(int parameterIndex, float newValue) override;

		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;



	private:
		juce::Point<float> lastDragPosition;
		double dragDelta = 0, dragStartProp = 0, catchDragDelta = 0;
		double  catchEscape = 0.05;
		int currentCatchRange = -1;
		
		double dragCatchCheck();

		bool notifyOnRelease = false;

		juce::Time lastMouseWheelTime;
	};

}// namnepace bdsp




