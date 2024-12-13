#pragma once




namespace bdsp
{
	class ChoiceButtons : public juce::Component, public ChoiceComponentCore, public ComponentCore
	{
	public:
		ChoiceButtons(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames = juce::StringArray(), const std::function<void(int)>& changFunc = std::function<void(int)>());
		~ChoiceButtons() = default;

		void resized() override;

		void setLayout(int rows, int cols);
		void setLayout(juce::Array<int>& newLayout);

		void parameterChanged(int idx) override;

		void setColor(NamedColorsIdentifier bkgd, NamedColorsIdentifier c);

	private:
		juce::Array<int> layout; // each int is how many buttons in that row
		juce::OwnedArray<TextButton> buttons;
		std::function<void(int)> onChangeFunc;

	};
} // namespace bdsp
