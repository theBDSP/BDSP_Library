#include "bdsp_ChoiceSlider.h"
namespace bdsp
{

	ChoiceSlider::ChoiceSlider(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, juce::StringArray dispNames, std::function<void(int)> changFunc, juce::UndoManager* undo)
		:ComponentCore(this, universalsToUse),
		ChoiceComponentCore(param, this),
		GUI_Universals::Listener(universalsToUse)
	{

		;
		onChangeFunc = changFunc;

		s.setSliderStyle(juce::Slider::LinearVertical);
		s.setRange(0, getNumItems() - 1, 1);
		s.setDoubleClickReturnValue(false, 0, juce::ModifierKeys::commandModifier);
		s.setSliderSnapsToMousePosition(false);

//		s.setLookAndFeel(&universalsToUse->MasterChoiceSliderLNF);

		addMouseListener(&s, true);

		addAndMakeVisible(s);

		s.onValueChange = [=]()
		{
			selectItem(getNumItems() - 1 - s.getValue());
			repaint();
		};


		for (int i = 0; i < param->choices.size(); ++i)
		{
			labels.add(new Label(universals));
			labels.getLast()->setText("  " + param->choices[i], juce::dontSendNotification);
//			labels.getLast()->setLookAndFeel(&universals->MasterChoiceSliderLNF);
			labels.getLast()->setEditable(false);
			labels.getLast()->getProperties().set("Selected", false);
			labels.getLast()->getProperties().set("Index", i);
			labels.getLast()->addMouseListener(this, false);
			addAndMakeVisible(labels.getLast());
		}

		//attach(param, undo);

		auto Dark = BDSP_COLOR_DARK;
		NamedColorsIdentifier DarkMid;
		DarkMid.ID = BDSP_COLOR_DARK;
		DarkMid.adjustmentFunction = [=](juce::Colour c)
		{
			return c.withAlpha(universals->midOpacity);
		};

		setColors(DarkMid, Dark, DarkMid, Dark);
	}

	void ChoiceSlider::parameterChanged(int idx)
	{

		s.setValue(getNumItems() - 1 - idx, juce::dontSendNotification);

		for (auto l : labels)
		{
			l->getProperties().set("Selected", false);
		}

		labels[idx]->getProperties().set("Selected", true);

		if (onChangeFunc.operator bool())
		{
			onChangeFunc(idx);
		}
		repaintThreadChecked();
	}

	void ChoiceSlider::paint(juce::Graphics& g)
	{
		auto alpha = isEnabled() ? 1.0f :universals->disabledAlpha;

		g.setColour(universals->hoverAdjustmentFunc(s.findColour(juce::Slider::trackColourId),s.isMouseOver()).withMultipliedAlpha(alpha));
		g.fillRoundedRectangle(s.getBounds().toFloat().reduced(s.getWidth() * 2.0 / 5.0, 0).withY(0).withBottom(getHeight()),universals->roundedRectangleCurve);


		g.setColour(universals->hoverAdjustmentFunc(s.findColour(juce::Slider::thumbColourId), s.isMouseOver()).withMultipliedAlpha(alpha));
		g.fillRoundedRectangle(juce::Rectangle<float>(s.getX(), s.proportionOfHeight(1 - s.valueToProportionOfLength(s.getValue())) + s.getY() - thumbH / 2.0, s.getWidth(), thumbH),universals->roundedRectangleCurve);

		for (auto l : labels)
		{
			g.drawHorizontalLine(l->getBounds().getCentreY(), s.getRight() + s.getWidth(), labelX - s.getWidth());
		}
	}

	void ChoiceSlider::resized()
	{
		s.setBoundsRelative(0, 0.1, 0.1, 0.8);

		thumbH = s.getWidth()/5.0;
		labelX = s.getRight() + 3 * s.getWidth();

		auto labelBounds = juce::Rectangle<int>(getWidth(), (float)s.getHeight() / getNumItems());
		for (int i = 0; i < labels.size(); ++i)
		{
			labels[i]->setBounds(labelBounds.withY(s.proportionOfHeight(s.getNormalisableRange().convertTo0to1(i)) + s.getY() - labelBounds.getHeight() / 2));
			labels[i]->getProperties().set("X", labelX);
		}
	}

	void ChoiceSlider::setColors(NamedColorsIdentifier track, NamedColorsIdentifier thumb, NamedColorsIdentifier text, NamedColorsIdentifier selectedText)
	{

		trackColor = track;
		thumbColor = thumb;
		textColor = text;
		selectedTextColor = selectedText;

		GUI_UniversalsChanged();


	}



	void ChoiceSlider::mouseUp(const juce::MouseEvent& e)
	{
		if (e.mods.isCommandDown())
		{
			reset();
		}
		else
		{
			for (auto l : labels)
			{
				if (l->contains(e.getEventRelativeTo(l).getPosition()))
				{
					selectItem(l->getProperties().getWithDefault("Index", 0));
					break;
				}
			}
		}


	}

	void ChoiceSlider::GUI_UniversalsChanged()
	{
		s.setColour(juce::Slider::trackColourId, getColor(trackColor));
		s.setColour(juce::Slider::thumbColourId, getColor(thumbColor));

		for (auto l : labels)
		{
			l->setColour(juce::Label::textColourId, getColor(textColor));
			l->setColour(juce::Label::textWhenEditingColourId, getColor(selectedTextColor));
		}

		repaint();
	}



} // namespace bdsp