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
	void LookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
	{
		juce::Rectangle<float> thumbBounds;
		juce::Path thumb;
		float thickness;

		if (isScrollbarVertical)
		{
			thickness = width;
			thumbBounds = juce::Rectangle<float>(x, thumbStartPosition, width, thumbSize).reduced(0,thickness);
			thumb.startNewSubPath(thumbBounds.getRelativePoint(0.5, 0.0));
			thumb.lineTo(thumbBounds.getRelativePoint(0.5, 1.0));
		}
		else
		{
			thickness = height;
			thumbBounds = juce::Rectangle<float>(thumbStartPosition, y, thumbSize, height).reduced(thickness,0);
			thumb.startNewSubPath(thumbBounds.getRelativePoint(0.0, 0.5));
			thumb.lineTo(thumbBounds.getRelativePoint(1.0, 0.5));
		}


		auto c = scrollbar.findColour(juce::ScrollBar::ColourIds::thumbColourId);
		g.setColour(getHoverColor(c, c.withMultipliedBrightness(universals->midOpacity), isMouseDown, isMouseOver, universals->hoverMixAmt));
		//g.fillRoundedRectangle(thumbBounds.toFloat(), universals->roundedRectangleCurve);
		g.strokePath(thumb, juce::PathStrokeType(thickness, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded));
	}
} // namespace bdsp