#pragma once



namespace bdsp
{

	class GUI_Universals;
	class LookAndFeel : public juce::LookAndFeel_V4
	{
	public:
		LookAndFeel();

		~LookAndFeel();



		juce::Label* createSliderTextBox(juce::Slider&) override;


		void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider&) override;

		void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;


		juce::Slider::SliderLayout getSliderLayout(juce::Slider& s) override;

		void drawLabel(juce::Graphics& g, juce::Label& label) override;

		void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

		juce::CaretComponent* createCaretComponent(juce::Component* keyFocusOwner) override;

		void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& te) override;

		void drawTextEditorOutline(juce::Graphics&, int width, int height, juce::TextEditor&) override;

		GUI_Universals* universals;




		//// Inherited via Listener
		//virtual void GUI_UniversalsChanged() override
		//{
		//	setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));
		//	setColour(juce::Slider::textBoxHighlightColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));
		//	setColour(juce::Slider::backgroundColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));
		//	setColour(juce::Slider::trackColourId, universals->colors.getColor(BDSP_COLOR_DARK).withAlpha(universals->midOpacity));




		//	setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));
		//	setColour(juce::Label::textColourId, universals->colors.getColor(BDSP_COLOR_DARK));
		//	setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));
		//	setColour(juce::Label::textWhenEditingColourId, findColour(juce::Label::textColourId));


		//	setColour(juce::TextEditor::textColourId, findColour(juce::Label::textColourId));
		//	setColour(juce::TextEditor::outlineColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));
		//	setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));
		//	setColour(juce::TextEditor::outlineColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));
		//	setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));
		//	setColour(juce::TextEditor::highlightColourId, universals->colors.getColor(BDSP_COLOR_DARK).withAlpha(universals->lowOpacity));
		//	setColour(juce::TextEditor::highlightedTextColourId, findColour(juce::Label::textColourId));
		//	setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));


		//	setColour(juce::CaretComponent::caretColourId, universals->colors.getColor(BDSP_COLOR_DARK));
		//}

	};


	//================================================================================================================================================================================================


}// namnepace bdsp
