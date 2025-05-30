#pragma once



namespace bdsp
{

	// wraps juce textUp button in BDSP context and adds functionality
	class TextButton : public Button
	{
	public:
		TextButton(GUI_Universals* universalsToUse);
		~TextButton() = default;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override;
		void paintBackground(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown);
		void paintText(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown);

		void setTextHeightRatio(float newRatio);
		float getTextHeightRatio() const;

		void setText(const juce::String& newText);
		void setText(const juce::String& on, const juce::String& off);
		void setCorners(CornerCurves newCorners);
		void setCornerRadius(float newRadius);
		void setGradientBackground(bool newValue);
		void setJustification(juce::Justification newJustification);
		void setTextBorder(const juce::BorderSize<int>& newBorder);
		void setTwoColorScheme(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& textColor);

		juce::Font getFont();

		void setHasOutline(bool newValue);
		juce::String getText();

		float getTextWidth();
		float getNonTextWidth();// gets width of component that is not covered by the textUp

		juce::Rectangle<float> getTextBounds();

		NamedColorsIdentifier backgroundUp, backgroundDown, textUp, textDown;

		std::function<juce::String()> getMaxText;
	protected:
		GUI_Universals* universals;
		juce::String onText, offText;


		CornerCurves corners = CornerCurves(CornerCurves::topLeft | CornerCurves::topRight | CornerCurves::bottomLeft | CornerCurves::bottomRight);
		float cornerRadius = -1;
		bool hasOutline = true;

		bool gradientBackground = false;

		juce::Justification justification;
		juce::BorderSize<int> textBorder;

		float textHRatio = 0.9;
	};

	class TextToggleButton : public TextButton
	{
	public:

		TextToggleButton(GUI_Universals* universalsToUse);
		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override;

		void setIndicatorColor(const NamedColorsIdentifier& on, const NamedColorsIdentifier& off = NamedColorsIdentifier());
		NamedColorsIdentifier indicatorOn, indicatorOff;
	};

}// namnepace bdsp