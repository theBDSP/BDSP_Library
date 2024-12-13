#pragma once

namespace bdsp
{
	class ToggleSlider : public Button
	{
	public:

		ToggleSlider(GUI_Universals* universalsToUse, const juce::String& descriptionText, bool labelUnder = true);
		~ToggleSlider();

		void setColors(const NamedColorsIdentifier& thumbColor, const NamedColorsIdentifier& textColor, const NamedColorsIdentifier& outlineColor = NamedColorsIdentifier(), const NamedColorsIdentifier& backgroundColor = BDSP_COLOR_PURE_BLACK);

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

		void setLabelPos(bool isUnder);
	private:

		NamedColorsIdentifier thumb, background, outline, text;
		juce::String label;

		bool labelUnder = true;
	};
} // namespace bdsp