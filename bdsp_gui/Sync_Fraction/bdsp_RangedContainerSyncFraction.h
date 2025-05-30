#pragma once



namespace bdsp
{

	class RangedContainerSyncFraction : public SyncFractionBase
	{
	public:


		RangedContainerSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate = false);
		~RangedContainerSyncFraction();



		void attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterIDBase) override;


		void resized() override;






		void setFracColors(const NamedColorsIdentifier& background, const NamedColorsIdentifier& value, const NamedColorsIdentifier& comboBKGD, const NamedColorsIdentifier& comboText, const NamedColorsIdentifier& outline);








		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

	

		RangedContainerSlider* getFraction();
		RangedContainerSlider* getMsSlider();



		ComboBox* getDivision();

		PathButton* getLockButton();

		void setCorners(CornerCurves newCurves);







	protected:
		NamedColorsIdentifier lockBKGD, outlineColor;


		std::unique_ptr<RangedContainerSlider> containerFrac, containerTime;



		std::unique_ptr<ComboBox> divisionCombo;

		std::unique_ptr<PathButton> lockButton;

		CornerCurves corners = CornerCurves::left;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RangedContainerSyncFraction)


	};


}// namnepace bdsp

