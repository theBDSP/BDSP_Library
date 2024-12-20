#pragma once



namespace bdsp
{
	#define BDSP_SQUARE_BUTTON_SHRINK 0.066
	#define BDSP_SQUARE_BUTTON_SHRINK_MULTIPLIER 1.25
	#define BDSP_SQUARE_BUTTON_CORNER_RATIO 0.125

	class SquareButton : public Button
	{
	public:
		SquareButton(GUI_Universals* universalsToUse);
		~SquareButton();

		void resized() override;
		
		void setButtonColors(NamedColorsIdentifier ON, NamedColorsIdentifier OFF,  NamedColorsIdentifier Text = NamedColorsIdentifier());

		void setText(juce::String textToUse);
		void setIcon(juce::Path p);
		void setIconColor(NamedColorsIdentifier c);

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;





	protected:

		NamedColorsIdentifier on, off, text;
		juce::String t;
		juce::Path icon;
		NamedColorsIdentifier iconColor;

		GUI_Universals* universals;


	};


}// namnepace bdsp
