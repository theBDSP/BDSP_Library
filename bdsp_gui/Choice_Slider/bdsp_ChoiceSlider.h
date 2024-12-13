#pragma once




namespace bdsp
{

	class ChoiceSlider : public juce::Component, public ComponentCore, public ChoiceComponentCore, public GUI_Universals::Listener
	{
	public:

		ChoiceSlider(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, juce::StringArray dispNames = juce::StringArray(), std::function<void(int)> changFunc = std::function<void(int)>(), juce::UndoManager* undo = nullptr);
		~ChoiceSlider() = default;

		// Inherited via ChoiceComponentCore
		virtual void parameterChanged(int idx) override;


		void paint(juce::Graphics& g) override;
		void resized() override;

		void setColors(NamedColorsIdentifier track, NamedColorsIdentifier thumb, NamedColorsIdentifier text, NamedColorsIdentifier selectedText);

		void mouseUp(const juce::MouseEvent& e) override;



	private:

		juce::Slider s;

		;

		std::function<void(int)> onChangeFunc;

		juce::OwnedArray<Label> labels;

		float thumbH = 0, labelX = 0;

		NamedColorsIdentifier trackColor, thumbColor, textColor, selectedTextColor;


		// Inherited via Listener
		void GUI_UniversalsChanged() override;

	};

} // namespace bdsp