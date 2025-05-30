#pragma once
namespace bdsp
{
	class LinkableControlComponents
	{
	public:
		LinkableControlComponents(dsp::SampleSourceList<float>* list, GUI_Universals* universalsToUse, juce::ValueTree* macroNameValueLocation, BPMComponent* BPMComp);
		~LinkableControlComponents();
		juce::String getMaxName(int idx);

		MacroSection* getMacros();
		LFOSection* getLFOs();
		EnvelopeFollowerSection<float>* getEnvelopeFolowers();

		SequencerSection* getSequencers();

		juce::Array<LinkableControlComponent*> controls;
		std::unique_ptr<MacroSection> Macros;
		std::unique_ptr<LFOSection> LFOs;
		std::unique_ptr<EnvelopeFollowerSection<float>> envelopeFollowers;
		std::unique_ptr<SequencerSection> Sequencers;


	private:
		juce::StringArray maxNames; // holds longest control name for each font defined in universal fonts
	};

} // namespace bdsp