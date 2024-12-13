#include "bdsp_LookAndFeel.h"

namespace bdsp
{
	LookAndFeel::LookAndFeel()
	{


	}
	LookAndFeel::~LookAndFeel()
	{

	}
	juce::Label* LookAndFeel::createSliderTextBox(juce::Slider&)
	{
		return new Label(universals);
	}
	void LookAndFeel::drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider&)
	{

	}
	void LookAndFeel::drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&)
	{

	}
	juce::Slider::SliderLayout LookAndFeel::getSliderLayout(juce::Slider& s)
	{
		juce::Slider::SliderLayout out;
		out.sliderBounds = s.getLocalBounds();
		return out;
	}
	void LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
	{

	}
	void LookAndFeel::drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{

	}
	juce::CaretComponent* LookAndFeel::createCaretComponent(juce::Component* keyFocusOwner)
	{
		return new Caret(keyFocusOwner);
	}
	void LookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& te)
	{
		g.setColour(te.findColour(juce::TextEditor::backgroundColourId));
		g.fillRoundedRectangle(te.getLocalBounds().toFloat(), universals->roundedRectangleCurve);
	}
	void LookAndFeel::drawTextEditorOutline(juce::Graphics&, int width, int height, juce::TextEditor&)
	{

	}
} // namespace bdsp