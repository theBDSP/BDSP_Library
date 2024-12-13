#include "bdsp_ToggleSlider.h"

bdsp::ToggleSlider::ToggleSlider(GUI_Universals* universalsToUse, const juce::String& descriptionText, bool labelUnder)
	:Button(universalsToUse)
{
	label = descriptionText;
	setFontIndex(0);
	setClickingTogglesState(true);

	setLabelPos(labelUnder);
}

bdsp::ToggleSlider::~ToggleSlider()
{
}

void bdsp::ToggleSlider::setColors(const NamedColorsIdentifier& thumbColor, const NamedColorsIdentifier& textColor, const NamedColorsIdentifier& outlineColor, const NamedColorsIdentifier& backgroundColor)
{

	thumb = thumbColor;
	text = textColor;

	outline = outlineColor.isEmpty() ? text : outlineColor;
	background = backgroundColor;
}

void bdsp::ToggleSlider::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;
	juce::Rectangle<float> sliderRect, textRect;
	if (label.isEmpty())
	{
		sliderRect = confineToAspectRatio(getLocalBounds().toFloat(), 1.75);
	}
	else
	{

		if (labelUnder)
		{
			sliderRect = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 0, 1, 0.5));
			sliderRect = sliderRect.withSizeKeepingCentre(sliderRect.getHeight() * 1.75, sliderRect.getHeight());

			textRect = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 0.5, 1, 0.5)).getProportion(juce::Rectangle<float>(0, 1.0 / 3.0, 1, 2.0 / 3.0));
		}
		else
		{
			sliderRect = getLocalBounds().toFloat().withWidth(getHeight() * 1.75);
			textRect = getLocalBounds().toFloat().withLeft(getHeight() * 2.0);
		}
	}


	juce::PathStrokeType outlineStroke(sliderRect.getHeight(), juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
	juce::PathStrokeType backgroundStroke(sliderRect.getHeight() * 0.95, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
	juce::Path track, trackOutline;

	trackOutline.startNewSubPath(sliderRect.getX() + outlineStroke.getStrokeThickness() / 2, sliderRect.getCentreY());
	trackOutline.lineTo(sliderRect.getRight() - outlineStroke.getStrokeThickness() / 2, sliderRect.getCentreY());

	track.startNewSubPath(sliderRect.getX() + outlineStroke.getStrokeThickness() / 2, sliderRect.getCentreY());
	track.lineTo(sliderRect.getRight() - outlineStroke.getStrokeThickness() / 2, sliderRect.getCentreY());

	g.setColour(universals->colors.getColor(outline).withAlpha(alpha));
	g.strokePath(trackOutline, outlineStroke);

	g.setColour(universals->colors.getColor(background).withAlpha(alpha));
	g.strokePath(track, backgroundStroke);

	g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(thumb).withAlpha(getToggleState() ? 1.0f : universals->midOpacity).withMultipliedAlpha(alpha), shouldDrawButtonAsHighlighted));
	g.fillEllipse(juce::Rectangle<float>(backgroundStroke.getStrokeThickness() * 0.75, backgroundStroke.getStrokeThickness() * 0.75).withCentre(juce::Point<float>(getToggleState() ? sliderRect.getRight() - outlineStroke.getStrokeThickness() / 2 : sliderRect.getX() + outlineStroke.getStrokeThickness() / 2, sliderRect.getCentreY())));

	g.setColour(universals->colors.getColor(text).withAlpha(alpha));

	//juce::GlyphArrangement ga;
	//ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), label, 0, 0);
	//juce::Path p;
	//ga.createPath(p);
	//scaleToFit(p, getLocalBounds().withTop(sliderRect.getBottom()));
	//g.fillPath(p);


	juce::GlyphArrangement ga;
	ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), label, 0, 0);
	juce::Path p;
	ga.createPath(p);

	scaleToFit(p, textRect);
	g.fillPath(p);

}

void bdsp::ToggleSlider::setLabelPos(bool isUnder)
{
	labelUnder = isUnder;
	repaint();
}
