#pragma once


namespace bdsp
{

	class PathButton : public Button
	{
	public:

		PathButton(GUI_Universals* universalsToUse, bool toggle = false, bool on = false);

		~PathButton();

		PathButton& operator = (const PathButton& oldButton);


		void resized() override;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

		void setPath(juce::Path down, juce::Path up = juce::Path());

		void setColor(const NamedColorsIdentifier& down, const NamedColorsIdentifier& up = NamedColorsIdentifier());

		void setPathBorder(float newBorder);

	private:


		juce::Path upPath, downPath;


		NamedColorsIdentifier upColor, downColor;

		float pathBorder = 0;



	};
}// namnepace bdsp


