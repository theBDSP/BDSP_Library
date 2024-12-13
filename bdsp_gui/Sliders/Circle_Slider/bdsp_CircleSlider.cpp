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


		resetLookAndFeels(universalsToUse);

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
        jassert(getWidth()==getHeight());
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
		paintCircularSlider(g, this, getLocalBounds().withHeight(getWidth()), !isRanged, true, isRanged, true);
		//bool isRanged = dynamic_cast<RangedSlider*>(getParentComponent()) != nullptr;
		//paintCircularSlider(g, this, getLocalBounds().toFloat().withHeight(getWidth()), !isRanged, true, isRanged);


	}

	void CircleSlider::lookAndFeelChanged()
	{
		label.setLookAndFeel(&getLookAndFeel());
	}

	void CircleSlider::resetLookAndFeels(bdsp::GUI_Universals* universalsToUse)
	{
//		setLookAndFeel(&universalsToUse->MasterCircleSliderLNF);
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
