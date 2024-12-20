#include "bdsp_SquareButton.h"


namespace bdsp
{
	SquareButton::SquareButton(GUI_Universals* universalsToUse)
		:Button(universalsToUse)
	{
		setClickingTogglesState(true);
		setButtonColors(BDSP_COLOR_BLACK, BDSP_COLOR_DARK, BDSP_COLOR_LIGHT);
		universals = universalsToUse;
	}
	SquareButton::~SquareButton()
	{
		setLookAndFeel(nullptr);
	}


	void SquareButton::resized()
	{
		forceAspectRatio(1);
	}

	void SquareButton::setButtonColors(NamedColorsIdentifier ON, NamedColorsIdentifier OFF, NamedColorsIdentifier Text)
	{
		on = ON;
		off = OFF;
		text = Text;
	}

	void SquareButton::setText(juce::String textToUse)
	{
		t = textToUse;
	}

	void SquareButton::setIcon(juce::Path p)
	{
		icon = p;
	}

	void SquareButton::setIconColor(NamedColorsIdentifier c)
	{
		iconColor = c;
	}



	void SquareButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;
		auto bounds = getLocalBounds().toFloat();






		if (shouldDrawButtonAsDown || getToggleState())
		{
			bounds.reduce(bounds.getWidth() * BDSP_SQUARE_BUTTON_SHRINK_MULTIPLIER * BDSP_SQUARE_BUTTON_SHRINK, bounds.getHeight() * BDSP_SQUARE_BUTTON_SHRINK_MULTIPLIER * BDSP_SQUARE_BUTTON_SHRINK); // inner button is smaller when down
		}
		else
		{
			bounds.reduce(bounds.getWidth() * BDSP_SQUARE_BUTTON_SHRINK, bounds.getHeight() * BDSP_SQUARE_BUTTON_SHRINK);
		}


		juce::Path p;
		p.addEllipse(bounds);


		auto c = getHoverColor(getColor(on), getColor(off), shouldDrawButtonAsDown || getToggleState(), shouldDrawButtonAsHighlighted, universals->hoverMixAmt).withMultipliedAlpha(alpha);
		g.setColour(c);
		g.fillPath(p);


		bounds.reduce(bounds.getWidth() * BDSP_SQUARE_BUTTON_SHRINK, bounds.getHeight() * BDSP_SQUARE_BUTTON_SHRINK); // shrink again for icon

		if (icon != juce::Path())
		{
			icon.scaleToFit(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), true);
			g.setColour(getColor(iconColor).withMultipliedAlpha(alpha));
			g.fillPath(icon);
		}


		g.setColour(getColor(text));
		g.setFont(universals->Fonts[getFontIndex()].getFont().withHeight(bounds.getHeight() * 0.8));
		drawText(g,g.getCurrentFont(),t, bounds);

	}

}// namnepace bdsp
