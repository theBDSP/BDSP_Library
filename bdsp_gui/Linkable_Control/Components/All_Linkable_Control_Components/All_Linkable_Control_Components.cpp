#include "All_Linkable_Control_Components.h"

namespace bdsp
{
	//================================================================================================================================================================================================



	LinkableControlComponents::LinkableControlComponents(dsp::SampleSourceList<float>* list, GUI_Universals* universalsToUse, juce::ValueTree* macroNameValueLocation, BPMComponent* BPMComp)
	{
		Macros = std::make_unique<MacroSection>(universalsToUse, BDSP_COLOR_DARK, macroNameValueLocation);
		LFOs = std::make_unique<LFOSection>(universalsToUse, BPMComp, BDSP_COLOR_DARK, macroNameValueLocation);
		envelopeFollowers = std::make_unique<EnvelopeFollowerSection<float>>(universalsToUse, list, BDSP_COLOR_DARK, macroNameValueLocation);

		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			controls.add(Macros->getMacro(i));
		}

		for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
		{
			controls.add(LFOs->getLFO(i));
		}

		for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
		{
			controls.add(envelopeFollowers->getFollower(i));
		}

	

		for (int i = 0; i < universalsToUse->Fonts.size(); ++i)
		{
			float max = 0;

			for (auto c : controls)
			{
				auto w = universalsToUse->Fonts[i].getFont().getStringWidthFloat(c->name);
				if (w > max)
				{
					max = w;
					maxNames.set(i, c->name);
				}
			}
		}
	}



	LinkableControlComponents::~LinkableControlComponents()
	{
	}

	juce::String LinkableControlComponents::getMaxName(int idx)
	{
		return maxNames[idx];
	}

	MacroSection* LinkableControlComponents::getMacros()
	{
		return Macros.get();
	}

	LFOSection* LinkableControlComponents::getLFOs()
	{
		return LFOs.get();
	}

	EnvelopeFollowerSection<float>* LinkableControlComponents::getEnvelopeFolowers()
	{
		return envelopeFollowers.get();
	}

} // namespace bdsp