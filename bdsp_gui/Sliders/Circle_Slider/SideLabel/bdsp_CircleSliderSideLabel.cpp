#include "bdsp_CircleSliderSideLabel.h"

namespace bdsp
{





	CircleSliderSideLabel::CircleSliderSideLabel(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit)
		:CircleSlider(universalsToUse, baseName, isEdit)
	{
		label.setJustificationType(juce::Justification::centredLeft);
		label.getTitle().setJustificationType(juce::Justification::centredLeft);
	}



	void CircleSliderSideLabel::setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds, juce::RectanglePlacement placement)
	{
		if (!newBounds.isEmpty())
		{
			setBounds(shrinkRectangleToInt(newBounds.withWidth(newBounds.getHeight())));

			label.setBounds(newBounds.withLeft(getWidth() * (1 + BDSP_CIRCLE_SLIDER_SIDE_LABEL_GAP)));
			resized();
		}
	}

	void CircleSliderSideLabel::resized()
	{

		forceAspectRatio(1);

		label.setBounds(getRight() + getWidth() * BDSP_CIRCLE_SLIDER_SIDE_LABEL_GAP, getY(), getWidth() * BDSP_CIRCLE_SLIDER_SIDE_LABEL_DEFAULT_WIDTH, getHeight());


	}



}// namnepace bdsp
