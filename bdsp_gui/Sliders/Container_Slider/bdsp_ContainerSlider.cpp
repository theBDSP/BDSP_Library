#include "bdsp_ContainerSlider.h"


namespace bdsp
{
	ContainerSlider::ContainerSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:BaseSlider(universalsToUse, baseName),
		label(this)
	{
		rotary = false;
		setInterceptsMouseClicks(true, false);



		setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		setTextBoxIsEditable(true);
		setSliderSnapsToMousePosition(false);

		setSliderColors(BDSP_COLOR_KNOB, BDSP_COLOR_COLOR, BDSP_COLOR_DARK, BDSP_COLOR_LIGHT, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, NamedColorsIdentifier(BDSP_COLOR_COLOR).withMultipliedAlpha(universals->lowOpacity));




		label.setEditable(false, true, true);
		label.setAsMainLabel();
		label.setJustificationType(juce::Justification::centred);
		label.removeHoverPartner(this);

		updateLabel();




		label.setHasNoTitle(true);

		addAndMakeVisible(label);

	}

	ContainerSlider::ContainerSlider(const ContainerSlider& other)
		:BaseSlider(other.universals, other.getName()),
		label(this)
	{
	}

	ContainerSlider::~ContainerSlider()
	{
		label.removeMouseListener(this);
	}

	void ContainerSlider::resized()
	{
		if (isVertical)
		{
			label.setBoundsRelative(0, 0, BDSP_CONTAINER_SLIDER_LABEL_WIDTH_RATIO, 1);
		}
		else
		{
			label.setBoundsRelative(0, 0, 1, BDSP_CONTAINER_SLIDER_LABEL_HEIGHT_RATIO);
		}
	}


	void ContainerSlider::initText()
	{
		updateLabel();
	}

	void ContainerSlider::setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight)
	{
		auto& textNew = text.isEmpty() ? value : text;
		auto& textEditNew = textEdit.isEmpty() ? value : textEdit;
		auto& textHighlightNew = textHighlight.isEmpty() ? value : textHighlight;
		auto& caretNew = caret.isEmpty() ? BDSP_COLOR_COLOR : caret;

		const auto& highlightNew = highlight.isEmpty() ? textNew.withMultipliedAlpha(universals->lowOpacity) : highlight;




		BaseSlider::setSliderColors(knob, value, valueTrack, valueTrackInside, textNew, textHighlightNew, textEditNew, caretNew, highlightNew);

	}

	void ContainerSlider::paint(juce::Graphics& g)
	{
		bool isRanged = dynamic_cast<RangedSlider*>(getParentComponent()) != nullptr;
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;

		juce::Path background = getRoundedRectangleFromCurveBools(getLocalBounds().toFloat(), curvesVal, universals->roundedRectangleCurve);

		g.setColour(getColor(knobColor).withAlpha(alpha));
		g.fillPath(background);

		g.setColour(getColor(valueTrackColor).withAlpha(alpha));
		g.strokePath(background, juce::PathStrokeType(universals->dividerSize));



		auto actualValueProp = valueToProportionOfLength(getActualValue());
		auto valueProp = valueToProportionOfLength(getValue());

		juce::Point<float> thumbCenter;
		juce::Rectangle<float> thumbBounds;
		if (isVertical)
		{
			auto valueBounds = getLocalBounds().toFloat().withLeft(label.getRight()).reduced(0, universals->roundedRectangleCurve);
			if (isRanged)
			{
				valueBounds = valueBounds.getProportion(juce::Rectangle<float>(0.05, 0, 0.4, 1));
			}
			else
			{
				valueBounds = valueBounds.getProportion(juce::Rectangle<float>(0.05, 0, 0.9, 1));
			}

			thumbBounds = juce::Rectangle<float>(valueBounds.getWidth() * 1.25, valueBounds.getWidth() * 1.25);
			juce::Path value;
			if (type == Normal)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.5, 1.0));
				value.lineTo(valueBounds.getRelativePoint(0.5, 1 - actualValueProp));

				thumbCenter = valueBounds.getRelativePoint(0.5, 1 - valueProp);

			}
			else if (type == CenterZero)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.5, 0.5));
				value.lineTo(valueBounds.getRelativePoint(0.5, 1 - actualValueProp));

				thumbCenter = valueBounds.getRelativePoint(0.5, 1 - valueProp);
			}
			else if (type == CenterMirrored)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.5, 0.5 + actualValueProp / 2));
				value.lineTo(valueBounds.getRelativePoint(0.5, 0.5 - actualValueProp / 2));

				thumbCenter = valueBounds.getRelativePoint(0.5, 0.5 + valueProp / 2);

			}

			g.setColour(getColor(valueColor).withAlpha(alpha));

			g.strokePath(value, juce::PathStrokeType(valueBounds.getWidth() / 2, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded));


			g.fillEllipse(thumbBounds.withCentre(thumbCenter));
			if (type == CenterMirrored)
			{
				thumbCenter.setY(0.5 - valueProp / 2);
				g.fillEllipse(thumbBounds.withCentre(thumbCenter));
			}
		}
		else
		{
			auto valueBounds = getLocalBounds().toFloat().withTop(label.getBottom()).reduced(universals->roundedRectangleCurve, 0);

			if (isRanged)
			{
				valueBounds = valueBounds.getProportion(juce::Rectangle<float>(0, 0.05, 1, 0.4));
			}
			else
			{
				valueBounds = valueBounds.getProportion(juce::Rectangle<float>(0, 0.05, 1, 0.9));
			}
			thumbBounds = juce::Rectangle<float>(valueBounds.getHeight()*1.25, valueBounds.getHeight() * 1.25);
			juce::Path value;
			if (type == Normal)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.0, 0.5));
				value.lineTo(valueBounds.getRelativePoint(actualValueProp, 0.5));

				thumbCenter = valueBounds.getRelativePoint(valueProp, 0.5);

			}
			else if (type == CenterZero)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.5, 0.5));
				value.lineTo(valueBounds.getRelativePoint(actualValueProp, 0.5));

				thumbCenter = valueBounds.getRelativePoint(valueProp, 0.5);
			}
			else if (type == CenterMirrored)
			{
				value.startNewSubPath(valueBounds.getRelativePoint(0.5 + actualValueProp / 2, 0.5));
				value.lineTo(valueBounds.getRelativePoint(0.5 - actualValueProp / 2, 0.5));

				thumbCenter = valueBounds.getRelativePoint(0.5 + valueProp / 2, 0.5);

			}

			g.setColour(getColor(valueColor).withAlpha(alpha));
			g.strokePath(value, juce::PathStrokeType(valueBounds.getHeight() / 2, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded));



			g.fillEllipse(thumbBounds.withCentre(thumbCenter));
			if (type == CenterMirrored)
			{
				thumbCenter.setX(0.5 - valueProp / 2);
				g.fillEllipse(thumbBounds.withCentre(thumbCenter));
			}
		}

	}

	void ContainerSlider::setIsVertical(bool shouldBeVertical)
	{
		isVertical = shouldBeVertical;
		resized();
	}

	bool ContainerSlider::getIsVertical()
	{
		return isVertical;
	}

	void ContainerSlider::setCornerCurves(CornerCurves curveValues)
	{
		curvesVal = curveValues;
	}

} // namespace bdsp