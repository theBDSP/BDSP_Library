#include "bdsp_BarSlider.h"

namespace bdsp
{
	BarSlider::BarSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:BaseSlider(universalsToUse, baseName), label(this)
	{

		setInterceptsMouseClicks(true, true);




		setVertical(true);
		setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		setSliderSnapsToMousePosition(true);


		auto border = juce::BorderSize<int>(0);
		label.setBorderSize(border);



		label.setJustificationType(juce::Justification::centred);
		label.setEditable(false, true, true);



		label.setName("");
		label.setAsMainLabel();

		label.setVisible(false);

		//onValueChange = [this]()
		//{

		//	label.setText(getTextFromValue(getActualValue()), juce::dontSendNotification);

		//};




	}

	BarSlider::BarSlider(const BarSlider& other)
		:BarSlider(nullptr, other.getName())
	{
		setLookAndFeel(&other.getLookAndFeel());
		setValue(other.getValue());
	}

	BarSlider::~BarSlider()
	{
		setLookAndFeel(nullptr);
	}

	void BarSlider::setVertical(bool isVertical)
	{
		setSliderStyle(isVertical ? RotaryVerticalDrag : RotaryHorizontalDrag);
	}

	void BarSlider::setBoundsToIncludeLabel(int x, int y, int w, int h)
	{
		setBoundsToIncludeLabel(juce::Rectangle<int>(x, y, w, h));
	}

	void BarSlider::setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds)
	{
		auto aspect = newBounds.toDouble().getAspectRatio();
		auto desired = aspect / (1 - BDSP_BAR_SLIDER_LABEL_BORDER_RATIO - BDSP_BAR_SLIDER_LABEL_HEIGHT_RATIO);



		setBounds(newBounds.getX(), newBounds.getY(), newBounds.getWidth(), newBounds.getWidth() / desired);

		labelBottom = newBounds.getBottom();

		resized();
	}


	void BarSlider::resized()
	{
		label.setBounds(juce::Rectangle<int>().leftTopRightBottom(getX(), getBottom(), getRight(), labelBottom));

		BaseSlider::resized();
	}







	void BarSlider::lookAndFeelChanged()
	{
		label.setLookAndFeel(&getLookAndFeel());

	}
	void BarSlider::setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight)
	{

		auto& valueTrackNew = valueTrack.isEmpty() ? knob : valueTrack;
		auto& textNew = text.isEmpty() ? value : text;
		auto& textEditNew = textEdit.isEmpty() ? value : textEdit;
		auto& textHighlightNew = textHighlight.isEmpty() ? value : textHighlight;
		auto& caretNew = caret.isEmpty() ? value : caret;


		const std::function<juce::Colour(const juce::Colour&)> func = [=](const juce::Colour& c)
		{
			return c.withAlpha(universals->lowOpacity);
		};
		const auto& colorLow = NamedColorsIdentifier(value.ID, func);
		const auto& highlightNew = highlight.isEmpty() ? colorLow : highlight;




		BaseSlider::setSliderColors(knob, value, valueTrackNew, valueTrackInside, textNew, textHighlightNew, textEditNew, caretNew, highlightNew);
	}

	void BarSlider::disableLabels()
	{
		label.setHasNoTitle(true);
	}
	void BarSlider::paint(juce::Graphics& g)
	{
		switch (visStyle)
		{
		case bdsp::BarSlider::Standard:
			paintStandard(g);
			break;
		case bdsp::BarSlider::Protuded:
			paintProtruded(g);
			break;
		default:
			break;
		}

	}

	void BarSlider::paintStandard(juce::Graphics& g)
	{
		auto bounds = getLocalBounds().toFloat();

		auto sliderPos = valueToProportionOfLength(getValue());
		auto actualSliderPos = valueToProportionOfLength(getActualValue());



		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;

		auto ptrGap = BDSP_LINEAR_SLIDER_POINTER_SIZE / (2 * (BDSP_LINEAR_SLIDER_POINTER_SIZE + 1)); // need so value ptr at extremes doesn't get clipped out

		bool vertical = isVertical();

		auto reduced = vertical ?
			bounds.getProportion(juce::Rectangle<float>(0.5 - (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT) / 2, ptrGap, (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT), 1 - 2 * ptrGap)) :
			bounds.getProportion(juce::Rectangle<float>(ptrGap, 0.5 - (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT) / 2, 1 - 2 * ptrGap, (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT)));

		auto ptrSize = (vertical ? reduced.getHeight() : reduced.getWidth()) * BDSP_LINEAR_SLIDER_POINTER_SIZE;
		juce::PathStrokeType ptrStroke(ptrSize, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
		auto& outlineStroke = ptrStroke;
		outlineStroke.setStrokeThickness(ptrStroke.getStrokeThickness() * 0.75);
		ptrSize += outlineStroke.getStrokeThickness();

		juce::Path track, knob, value, valuePTR;
		//================================================================================================================================================================================================
		//Value
		if (getType() == BaseSlider::CenterMirrored)
		{
			if (vertical)
			{
				value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.0, 0.5 - actualSliderPos / 2 + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2), reduced.getRelativePoint(0.0, 0.5 + actualSliderPos / 2 - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2)));
			}
			else
			{
				value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.5 - actualSliderPos / 2 + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0), reduced.getRelativePoint(0.5 + actualSliderPos / 2 - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 1.0)));
			}
		}
		else if (getType() == BaseSlider::CenterZero)
		{

			if (vertical)
			{
				value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.0, 1 - actualSliderPos + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2), reduced.getRelativePoint(1.0, 0.5)));
			}
			else
			{
				value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(actualSliderPos - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0), reduced.getRelativePoint(0.5, 1.0)));
			}
		}
		else
		{
			if (vertical)
			{
				value.addRectangle(reduced.withTop(reduced.getRelativePoint(0.0, 1 - actualSliderPos + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2).y));
			}
			else
			{
				value.addRectangle(reduced.withX(reduced.getRelativePoint(actualSliderPos - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0).x));
			}
		}

		//================================================================================================================================================================================================
		//Knob
		if (getType() == BaseSlider::CenterMirrored)
		{
			if (vertical)
			{
				knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - sliderPos / 2).y));
				knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - sliderPos / 2).y));

				knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + sliderPos / 2).y));
				knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + sliderPos / 2).y));

			}
			else
			{
				knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 - sliderPos / 2, 0.0).x, ptrSize / 2));
				knob.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 - sliderPos / 2, 0.0).x, bounds.getCentreY() - ptrSize / 2));

				knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 + sliderPos / 2, 0.0).x, ptrSize / 2));
				knob.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 + sliderPos / 2, 0.0).x, bounds.getCentreY() - ptrSize / 2));
			}
		}
		else
		{
			if (vertical)
			{
				knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 1 - sliderPos).y));
				knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 1 - sliderPos).y));

			}
			else
			{
				knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(sliderPos, 0.0).x, ptrSize / 2));
				knob.lineTo(juce::Point<float>(reduced.getRelativePoint(sliderPos, 0.0).x, bounds.getCentreY() - ptrSize / 2));
			}
		}
		ptrStroke.createStrokedPath(knob, knob);

		//================================================================================================================================================================================================
		//Track

		if (vertical)
		{
			track.addRectangle(reduced.getProportion(juce::Rectangle<float>(0, BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 1, 1 - BDSP_LINEAR_SLIDER_POINTER_SIZE)));
		}
		else
		{
			track.addRectangle(reduced.getProportion(juce::Rectangle<float>(BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0, 1 - BDSP_LINEAR_SLIDER_POINTER_SIZE, 1)));
		}




		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueTrackColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(track);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(value);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueTrackColor), isHovering()).withMultipliedAlpha(alpha));
		g.strokePath(knob, outlineStroke);
		g.strokePath(valuePTR, outlineStroke);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(valuePTR);

		//================================================================================================================================================================================================

		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(knobColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(knob);
	}

	void BarSlider::paintProtruded(juce::Graphics& g)
	{
		bool isVertical = juce::Slider::isVertical();

		auto sliderPos = valueToProportionOfLength(getValue());
		auto actualSliderPos = valueToProportionOfLength(getActualValue());

		float relativeTrackSize = isHovering() ? 0.25f : 0.15f;
		juce::Rectangle<float>track, thumb;
		g.setColour(getColor(valueTrackColor));

		if (isVertical)
		{
			track = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>((1 - relativeTrackSize) / 2, 0, relativeTrackSize, 1));
			g.fillRect(track);

			thumb = juce::Rectangle<float>(getWidth(), track.getWidth());
		}
		else
		{
			track = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, (1 - relativeTrackSize) / 2, 1, relativeTrackSize));
			g.fillRect(track);



			thumb = juce::Rectangle<float>(track.getHeight(), getHeight());
		}

		if (getType() == BaseSlider::CenterMirrored)
		{
			auto delta = actualSliderPos / 2.0;
			if (isVertical)
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(0, 0.5 - delta, 1, 0.5 + delta)));

			}
			else
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(0.5 - delta, 0, 0.5 + delta, 1)));

			}
		}
		else if (getType() == BaseSlider::CenterZero)
		{
			if (isVertical)
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(0, juce::jmin(1 - actualSliderPos, 0.5), 1, juce::jmax(1 - actualSliderPos, 0.5))));

			}
			else
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(juce::jmin(actualSliderPos, 0.5), 0, juce::jmax(actualSliderPos, 0.5), 1)));

			}
		}
		else
		{
			if (isVertical)
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(0, (1 - actualSliderPos), 1, 1)));

			}
			else
			{
				g.setColour(getColor(valueColor));
				g.fillRect(track.getProportion(juce::Rectangle<float>().leftTopRightBottom(0, 0, actualSliderPos, 1)));

			}
		}

		g.setColour(getColor(knobColor));
		if (getType() == BaseSlider::CenterMirrored)
		{
			auto delta = sliderPos / 2.0;
			if (isVertical)
			{
				g.fillRect(thumb.withCentre(juce::Point<float>(getWidth() / 2.0f, (0.5 - delta) * getHeight())));
				g.fillRect(thumb.withCentre(juce::Point<float>(getWidth() / 2.0f, (0.5 + delta) * getHeight())));
			}
			else
			{
				g.fillRect(thumb.withCentre(juce::Point<float>((0.5 - delta) * getWidth(), getHeight() / 2.0f)));
				g.fillRect(thumb.withCentre(juce::Point<float>((0.5 + delta) * getWidth(), getHeight() / 2.0f)));
			}
		}
		else
		{
			if (isVertical)
			{
				g.fillRect(thumb.withCentre(juce::Point<float>(getWidth() / 2.0f, (1 - sliderPos) * getHeight())));
			}
			else
			{
				g.fillRect(thumb.withCentre(juce::Point<float>(sliderPos * getWidth(), getHeight() / 2.0f)));
			}
		}








	}

	void BarSlider::parentHierarchyChanged()
	{

		auto parent = getParentComponent();
		if (parent != nullptr)
		{
			parent->addChildComponent(label);
		}
		else
		{
			parent = label.getParentComponent();
			if (parent != nullptr)
			{
				parent->removeChildComponent(&label);
			}
		}


	}

	void BarSlider::setVisualStyle(VisualStyles newStyle)
	{
		visStyle = newStyle;
		repaint();
	}


}// namnepace bdsp

