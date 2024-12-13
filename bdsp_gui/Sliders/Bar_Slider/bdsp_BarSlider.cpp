#include "bdsp_BarSlider.h"

namespace bdsp
{
	BarSlider::BarSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:BaseSlider(universalsToUse, baseName), label(this)
	{

		setInterceptsMouseClicks(true, true);



		resetLookAndFeels(universalsToUse);

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
		bool isRanged = dynamic_cast<RangedSlider*>(getParentComponent()) != nullptr;
		if (!isRanged)
		{
			paintLinearSlider(g, this, getLocalBounds(), true, true, false, true);
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


}// namnepace bdsp

