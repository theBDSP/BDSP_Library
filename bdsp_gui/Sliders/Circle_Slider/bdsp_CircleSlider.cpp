#include "bdsp_CircleSlider.h"

namespace bdsp
{


	CircleSlider::CircleSlider(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit)
		:BaseSlider(universalsToUse, baseName), label(this)
	{
		//universals = universalsToUse;
		rotary = true;
		isEditable = isEdit;
		setInterceptsMouseClicks(true, false);


		setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
		setTextBoxIsEditable(isEditable);
		setSliderSnapsToMousePosition(true);

		setSliderColors(BDSP_COLOR_KNOB, BDSP_COLOR_COLOR, BDSP_COLOR_DARK, BDSP_COLOR_LIGHT, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, BDSP_COLOR_COLOR, NamedColorsIdentifier(BDSP_COLOR_COLOR).withMultipliedAlpha(universals->lowOpacity));




		//influenceLabel.addMouseListener(this,true);
		label.setEditable(false, isEditable, true);
		label.setAsMainLabel();
		label.setJustificationType(juce::Justification::centred);

		//addHoverPartners(&influenceLabel.getTitle());
		//influenceLabel.getTitle().addMouseListener(this, true);

		updateLabel();



		//setSliderColors(universals->neutralLightest, universals->mainAccent, universals->altAccent);



		label.setVisible(false);




	}

	CircleSlider::CircleSlider(const CircleSlider& other)
		:CircleSlider(other.universals, other.getName(), other.isEditable)
	{
		setLookAndFeel(&other.getLookAndFeel());
		setValue(other.getValue());
	}
	CircleSlider::~CircleSlider()
	{
		label.removeMouseListener(this);

		setLookAndFeel(nullptr);


	}

	BaseSlider* CircleSlider::getSliderReference()
	{
		return this;
	}




	bool CircleSlider::hitTest(int x, int y)
	{
		bool lBool = isHoverPartner(&label) && label.hitTest(x, y);
		bool tBool = isHoverPartner(&label.getTitle()) && label.getTitle().hitTest(x, y);


		return lBool || tBool || juce::Component::hitTest(x, y);
		//return true;

	}

	void CircleSlider::updateLabel()
	{
		if (pathFromValueFunction.operator bool())
		{
			label.setVisible(false);

			//labelPath = pathFromValueFunction(getActualValue());
			labelPath = pathFromValueFunction(getValue());
		}
		//auto textUp = getTextFromValue(getActualValue());
		auto text = getTextFromValue(getValue());

		label.setText(text, juce::dontSendNotification);
		//influenceLabel.setText(textUp.retainCharacters("0123456789./-"), juce::NotificationType::dontSendNotification);
		//suffix.setText(textUp.removeCharacters("0123456789./-"), juce::dontSendNotification);
	}




	void CircleSlider::setBoundsToIncludeLabel(int x, int y, int w, int h, juce::RectanglePlacement placement)
	{
		setBoundsToIncludeLabel(juce::Rectangle<int>(x, y, w, h), placement);
	}

	void CircleSlider::setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds, juce::RectanglePlacement placement)
	{
		if (!newBounds.isEmpty())
		{

			auto sliderAndLabelBounds = confineToAspectRatio(newBounds.toFloat(), 1 / (1 + BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO + BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO), placement);
			auto confinedBounds = confineToAspectRatio(sliderAndLabelBounds, 1, juce::RectanglePlacement::xMid | juce::RectanglePlacement::yTop);

			labelWRatio = newBounds.toFloat().getWidth() / confinedBounds.getWidth();

			setBounds(shrinkRectangleToInt(confinedBounds));

			resized();
		}


	}

	void CircleSlider::resized()
	{

		forceAspectRatio(1);
		jassert(getWidth() == getHeight());
		auto overshoot = (labelWRatio - 1) * getWidth() / 2;
		label.setBounds(getX() - overshoot, getBottom() + getHeight() * BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO, getWidth() + 2 * overshoot, getHeight() * BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO);


		//BaseSlider::resized();
	}

	void CircleSlider::moved()
	{
		resized();
	}

	juce::Rectangle<int> CircleSlider::getTrackBounds(bool absolute)
	{
		auto centerX = getWidth() / 2.0;
		auto centerY = getHeight() / 2.0;
		auto r = juce::jmin(centerX, centerY);
		return (absolute ? getBounds() : getLocalBounds()).withHeight(r + r * abs(cos(getRotaryParameters().startAngleRadians)));

	}

	juce::Rectangle<int> CircleSlider::getKnobBounds(bool absolute)
	{
		auto centerX = getWidth() / 2.0;
		auto centerY = getHeight() / 2.0;
		auto r = juce::jmin(centerX, centerY);
		auto innerR = r * (1 - BDSP_CIRCULAR_SLIDER_TRACK_RATIO);

		auto knobR = innerR * BDSP_CIRCULAR_SLIDER_KNOB_RATIO;

		juce::Rectangle<int> out = getLocalBounds().withSizeKeepingCentre(2 * knobR, 2 * knobR);

		if (absolute)
		{
			out.translate(getX(), getY());
		}
		return out;
	}





	void CircleSlider::initText()
	{
		updateLabel();
	}

	void CircleSlider::setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight)
	{
		auto& textNew = text.isEmpty() ? BDSP_COLOR_COLOR : text;
		auto& textEditNew = textEdit.isEmpty() ? BDSP_COLOR_COLOR : textEdit;
		auto& textHighlightNew = textHighlight.isEmpty() ? BDSP_COLOR_COLOR : textHighlight;
		auto& caretNew = caret.isEmpty() ? BDSP_COLOR_COLOR : caret;

		const auto& colorLow = NamedColorsIdentifier(BDSP_COLOR_COLOR).withMultipliedAlpha(universals->lowOpacity);
		const auto& highlightNew = highlight.isEmpty() ? colorLow : highlight;




		BaseSlider::setSliderColors(knob, value, valueTrack, valueTrackInside, textNew, textHighlightNew, textEditNew, caretNew, highlightNew);
	}




	void CircleSlider::showTextEditor()
	{
		if (pathFromValueFunction.operator bool())
		{
			label.setVisible(true);
			labelPath.clear();

			label.onEditorHide = [=]()
			{
				label.setVisible(false);
			};
		}
		else
		{
			BaseSlider::showTextEditor();
		}
	}



	void CircleSlider::paint(juce::Graphics& g)
	{
		bool isRanged = dynamic_cast<RangedSlider*>(getParentComponent()) != nullptr;
		g.saveState();

		//================================================================================================================================================================================================
		auto newBounds = getLocalBounds().withHeight(getWidth());
		if (isRanged)
		{
			newBounds = expandRectangleToInt(newBounds.toFloat().getProportion(juce::Rectangle<float>(BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO)));
		}


		auto rotaryStartAngle = getRotaryParameters().startAngleRadians;
		auto rotaryEndAngle = getRotaryParameters().endAngleRadians;
		auto sliderPos = getNormalisableRange().convertTo0to1(getValue());
		auto actualSliderPos = getNormalisableRange().convertTo0to1(getActualValue());

		auto sliderAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
		auto actualSliderAngle = rotaryStartAngle + actualSliderPos * (rotaryEndAngle - rotaryStartAngle);




		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;



		auto center = newBounds.toFloat().getCentre();

		auto r = newBounds.getWidth() / 2.0;
		auto innerR = r * (1 - BDSP_CIRCULAR_SLIDER_TRACK_RATIO);
		auto w = (r - innerR);
		auto trackWidth = w * BDSP_CIRCULAR_SLIDER_TRACK_WIDTH;
		auto trackWidthInside = trackWidth * (1 - BDSP_CIRCULAR_SLIDER_TRACK_INSIDE_RATIO);

		auto trackW = trackWidthInside;
		auto knobR = innerR * BDSP_CIRCULAR_SLIDER_KNOB_RATIO;

		jassert(trackWidth <= w);



		//================================================================================================================================================================================================
		//Knob
		juce::Path knob;
		knob.addEllipse(expandRectangleToInt(juce::Rectangle<float>(2 * knobR, 2 * knobR).withCentre(center)).toFloat());
		auto color = (universals->hoverAdjustmentFunc(findColour(juce::Slider::backgroundColourId), isHovering()).withMultipliedAlpha(alpha));
		juce::ColourGradient cg(color.withMultipliedLightness(1.1f).withMultipliedAlpha(alpha), knob.getBounds().getCentre().getX(), knob.getBounds().getY(), color, center.x, center.y, true);
		g.setGradientFill(cg);
		g.fillPath(knob);

		//================================================================================================================================================================================================
		//Thumb

		juce::PathStrokeType ptrStroke(knobR * BDSP_CIRCULAR_SLIDER_THUMB_RATIO, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
		juce::Path ptr;


		if (getType() == BaseSlider::CenterMirrored)
		{
			auto angle1 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 + sliderPos) / 2.0;
			auto angle2 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 - sliderPos) / 2.0;



			ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, angle1));
			ptr.lineTo(center.getPointOnCircumference(knobR, angle1));

			ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, angle2));
			ptr.lineTo(center.getPointOnCircumference(knobR, angle2));


			ptrStroke.createStrokedPath(ptr, ptr);

		}
		else
		{

			ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, sliderAngle));


			ptr.lineTo(center.getPointOnCircumference(knobR, sliderAngle));

			ptrStroke.createStrokedPath(ptr, ptr);
		}




		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(BDSP_COLOR_PURE_BLACK), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(ptr);

		//================================================================================================================================================================================================
		//Track
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueTrackColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(createCenteredPieSegment(center, r - w / 2 - trackWidth / 2, r - w / 2 + trackWidth / 2, rotaryStartAngle, rotaryEndAngle));

		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueTrackInsideColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(createCenteredPieSegment(center, r - w / 2 - trackWidthInside / 2, r - w / 2 + trackWidthInside / 2, rotaryStartAngle, rotaryEndAngle));






		//================================================================================================================================================================================================
		//Value
		float midAngle = (rotaryStartAngle + rotaryEndAngle) / 2;
		juce::Path  p;


		switch (getType())
		{
		case BaseSlider::Normal:
			p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, rotaryStartAngle, actualSliderAngle);

			break;

		case BaseSlider::CenterZero:

			p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle, actualSliderAngle);

			break;

		case BaseSlider::CenterMirrored:
			auto angleDelta = (actualSliderAngle - rotaryStartAngle) / 2.0;
			p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle - angleDelta, midAngle + angleDelta);


			break;
		}


		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(valueColor), isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(p);

		//================================================================================================================================================================================================


		g.restoreState();
	}

	void CircleSlider::lookAndFeelChanged()
	{
		label.setLookAndFeel(&getLookAndFeel());
	}




	void CircleSlider::parentHierarchyChanged()
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



}// namnepace bdsp
