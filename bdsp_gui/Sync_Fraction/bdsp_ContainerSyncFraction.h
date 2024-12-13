#pragma once



namespace bdsp
{

	class ContainerSyncFraction : public SyncFractionBase
	{
	public:


		ContainerSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate = false);
		~ContainerSyncFraction();





		void resized() override;






		void setFracColors(const NamedColorsIdentifier& background, const NamedColorsIdentifier& value, const NamedColorsIdentifier& comboBKGD, const NamedColorsIdentifier& comboText,const NamedColorsIdentifier& outline);








		void paintOverChildren(juce::Graphics& g) override;





		ContainerSlider* getFraction();
		ContainerSlider* getMsSlider();



		ComboBox* getDivision();











	protected:

		NamedColorsIdentifier outlineColor;


		std::unique_ptr<ContainerSlider> containerFrac, containerTime;



		std::unique_ptr<ComboBox> divisionCombo;


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContainerSyncFraction)


	};


}// namnepace bdsp

