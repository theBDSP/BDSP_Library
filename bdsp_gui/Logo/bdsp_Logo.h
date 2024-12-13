#pragma once




namespace bdsp
{


	class Logo : public juce::Component, public ComponentCore, public GUI_Universals::Listener
	{

	public:
		Logo(GUI_Universals* universalsToUse);

		~Logo();


		void paint(juce::Graphics& g) override;

		void setColors(const juce::Array<NamedColorsIdentifier>& newColors);


		void resized() override;

		juce::Path getPath();


	private:

		juce::Path p;
		juce::Array<NamedColorsIdentifier> colors;
		juce::FillType logoFillType;



		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;

	};




}// namnepace bdsp

