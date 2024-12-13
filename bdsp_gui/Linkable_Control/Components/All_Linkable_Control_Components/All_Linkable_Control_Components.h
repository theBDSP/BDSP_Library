#pragma once

//#include "bdsp_gui/Linkable_Control/Components/Envelope_Follower_Component/bdsp_envelopeFollowerComponent.h"

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

		juce::Array<LinkableControlComponent*> controls;
		std::unique_ptr<MacroSection> Macros;
		std::unique_ptr<LFOSection> LFOs;
		std::unique_ptr<EnvelopeFollowerSection<float>> envelopeFollowers;


	private:
		juce::StringArray maxNames; // holds longest control name for each font defined in universal fonts
	};

} // namespace bdsp