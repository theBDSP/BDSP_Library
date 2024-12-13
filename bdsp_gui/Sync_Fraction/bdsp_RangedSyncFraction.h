#pragma once




namespace bdsp
{
	class RangedSyncFraction : public SyncFraction<RangedCircleSlider>
	{
	public:

		RangedSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate = false);
		~RangedSyncFraction();

		void swap(RangedSyncFraction* other);


		void paint(juce::Graphics& g) override;

		void resized() override;

		float getTimeInMs(double bpm, bool isRate = false) override;

		void setSnapButton(juce::Button* buttonToLink);


	private:
		juce::Button* snapButton = nullptr;

	};
}// namnepace bdsp

