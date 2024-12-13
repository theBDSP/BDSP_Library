#include "bdsp_ChoiceButtons.h"
namespace bdsp
{
	ChoiceButtons::ChoiceButtons(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:ChoiceComponentCore(param, this),
		ComponentCore(this, universalsToUse)
	{
        auto displayNames = dispNames;
		if (displayNames.isEmpty())
		{
			displayNames = param->choices;
		}
		for (int i = 0; i < dispNames.size(); ++i)
		{
			buttons.add(new TextButton(universalsToUse));
			auto current = buttons.getLast();
			current->setText(displayNames[i]);
			current->setClickingTogglesState(true);
			current->setRadioGroupId(1);
			current->onStateChange = [=]()
			{
				if (buttons[i]->getToggleState())
				{
					selectItem(i);
				}
			};
			addAndMakeVisible(current);
			current->addMouseListener(this, false);
		}
		onChangeFunc = changFunc;
		//attach(param, universals->undoManager);

		setLayout(buttons.size(), 1);
	}

	void ChoiceButtons::resized()
	{
		auto b = 0.05;
		auto h = (1.0 - (layout.size() - 1) * b) / layout.size();
		int idx = 0;

		for (int r = 0; r < layout.size(); ++r)
		{
			auto w = (1.0 - (layout[r] - 1) * b) / layout[r];
			for (int c = 0; c < layout[r]; ++c)
			{
				buttons[idx++]->setBoundsRelative(c * (w + b), r * (h + b), w - b, h - b);
			}

		}
	}

	void ChoiceButtons::setLayout(int rows, int cols)
	{
		jassert(rows * cols == buttons.size());
		layout.clear();


		for (int i = 0; i < rows; ++i)
		{
			layout.add(cols);
		}
	}

	void ChoiceButtons::setLayout(juce::Array<int>& newLayout)
	{
		layout = newLayout;
	}

	void ChoiceButtons::parameterChanged(int idx)
	{
		for (auto b : buttons)
		{
			b->setToggleState(false, juce::dontSendNotification);
		}
		buttons[idx]->setToggleState(true, juce::dontSendNotification);


		if (onChangeFunc.operator bool())
		{
			onChangeFunc(idx);
		}
	}

	void ChoiceButtons::setColor(NamedColorsIdentifier bkgd, NamedColorsIdentifier c)
	{
		for (auto b : buttons)
		{
			b->setTwoColorScheme(bkgd, c);
		}
	}






} // namespace bdsp

