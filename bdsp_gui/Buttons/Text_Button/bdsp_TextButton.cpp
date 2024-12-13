#include "bdsp_TextButton.h"

namespace bdsp
{

	TextButton::TextButton(GUI_Universals* universalsToUse)
		:Button(universalsToUse),
		justification(juce::Justification::centred)
	{
		universals = universalsToUse;

		getMaxText = [=]()
		{
			auto font = universals->Fonts[getFontIndex()].getFont();
			return font.getStringWidthFloat(onText) > font.getStringWidthFloat(offText) ? onText : offText;
		};

		setTwoColorScheme(BDSP_COLOR_COLOR, BDSP_COLOR_DARK);
	}
	void TextButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		bool isDown = shouldDrawButtonAsDown || getToggleState();
		bool isOver = shouldDrawButtonAsHighlighted || isHovering();

		paintBackground(g, isOver, isDown);
		paintText(g, isOver, isDown);
	}
	void TextButton::paintBackground(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{

		auto outlineW = hasOutline ? getHeight() * 0.025 : 0.0;

		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;



		juce::Path p = getRoundedRectangleFromCurveBools(getLocalBounds().toFloat().reduced(outlineW), corners, universals->roundedRectangleCurve);


		//================================================================================================================================================================================================




		auto maxText = getMaxText();

		//font = resizeFontToFit(font, p.getBounds().getWidth() * 0.9, p.getBounds().getHeight() * 0.9, maxText);


		//================================================================================================================================================================================================


		if (gradientBackground)
		{

			auto textRatio = getTextWidth() / p.getBounds().getWidth();
			auto c = getColor(shouldDrawButtonAsDown ? backgroundDown : backgroundUp);

			juce::ColourGradient cg(c.withMultipliedAlpha(0.0f), 0, 0, c.withMultipliedAlpha(0.0f), p.getBounds().getRight(), 0, false);
			cg.addColour(0.5 - textRatio / 2, c.withMultipliedAlpha(alpha));
			cg.addColour(0.5 + textRatio / 2, c.withMultipliedAlpha(alpha));

			g.setGradientFill(cg);
			g.fillRect(p.getBounds());

		}
		else
		{
			g.setColour(getHoverColor(getColor(backgroundDown), getColor(backgroundUp), shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted, universals->hoverMixAmt).withMultipliedAlpha(alpha));
			g.fillPath(p);
		}



		//================================================================================================================================================================================================


		g.setColour(getColor(backgroundDown));

		if (hasOutline)
		{
			g.setOpacity(0.5f);
			g.strokePath(p, juce::PathStrokeType(outlineW));
		}


	}
	void TextButton::paintText(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;
		g.setColour(getHoverColor(getColor(textDown), getColor(textUp), shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted, universals->hoverMixAmt).withMultipliedAlpha(alpha));
		drawText(g, getFont() , getText(), textBorder.subtractedFrom(getLocalBounds()), false, justification);
	}
	void TextButton::setTextHeightRatio(float newRatio)
	{
		textHRatio = newRatio;
		repaint();
	}
	float TextButton::getTextHeightRatio() const
	{
		return textHRatio;
	}
	void TextButton::setText(const juce::String& newText)
	{
		setText(newText, newText);
	}
	void TextButton::setText(const juce::String& on, const juce::String& off)
	{
		onText = on;
		offText = off;
		repaint();
	}

	void TextButton::setCorners(CornerCurves newCorners)
	{
		corners = newCorners;
		repaint();
	}

	void TextButton::setGradientBackground(bool newValue)
	{
		gradientBackground = newValue;
		repaint();
	}

	void TextButton::setJustification(juce::Justification newJustification)
	{
		justification = newJustification;
	}

	void TextButton::setTextBorder(const juce::BorderSize<int>& newBorder)
	{
		textBorder = newBorder;
		repaint();
	}


	void TextButton::setTwoColorScheme(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& textColor)
	{
		//jassert(bkgd.getSaturation() > 0.25f);// ensures there's enough satruation to distinguish from the desaturated version

		backgroundUp = bkgd.withMultipliedSaturation(universals->buttonDesaturation);
		backgroundDown = bkgd;

		textDown = textColor;

		textUp = textColor;

	}

	juce::Font TextButton::getFont()
	{
		return universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * textHRatio * BDSP_FONT_HEIGHT_SCALING);
	}

	void TextButton::setHasOutline(bool newValue)
	{
		hasOutline = newValue;
		repaint();
	}

	juce::String TextButton::getText()
	{
		return getToggleState() ? onText : offText;
	}
	float TextButton::getTextWidth()
	{
		return getFont().getStringWidthFloat(getText());
	}
	float TextButton::getNonTextWidth()
	{
		return getWidth() - getTextWidth();
	}

	juce::Rectangle<float> TextButton::getTextBounds()
	{
		return createTextPath(getFont(), getText(), textBorder.subtractedFrom(getLocalBounds()), justification).getBounds();
	}

	//================================================================================================================================================================================================


	TextToggleButton::TextToggleButton(GUI_Universals* universalsToUse)
		:TextButton(universalsToUse)
	{
		setClickingTogglesState(true);
		setHasOutline(false);
	}
	void TextToggleButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;

		TextButton::paintBackground(g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

		bool on = getToggleState() || shouldDrawButtonAsDown;

		juce::Path p;
		auto b = getHeight() * 0.1;
		p.addRoundedRectangle(b, b, getHeight() * 2.0 / 3.0 - 2 * b, getHeight() - 2 * b, universals->roundedRectangleCurve);
		g.setColour(getColor(BDSP_COLOR_PURE_BLACK).withAlpha(alpha));
		g.fillPath(p);

		g.setColour(getColor(on ? indicatorOn : indicatorOff).withAlpha(alpha));

		auto d = getHeight() * 2.0 / 3.0 - 4 * b;
		g.fillEllipse(2 * b, on ? 2 * b : getHeight() - 2 * b - d, d, d);

		g.setColour(getColor(on ? textDown : textUp).withAlpha(alpha));
		drawText(g, getFont(), getText(), getLocalBounds().withTrimmedLeft(p.getBounds().getRight()));
	}
	void TextToggleButton::setIndicatorColor(const NamedColorsIdentifier& on, const NamedColorsIdentifier& off)
	{
		indicatorOn = on;
		if (off.isEmpty())
		{
			indicatorOff = on.withMultipliedAlpha(universals->disabledAlpha);
		}
		else
		{
			indicatorOff = off;
		}
	}
}// namnepace bdsp
