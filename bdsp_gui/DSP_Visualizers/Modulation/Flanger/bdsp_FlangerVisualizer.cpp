#include "bdsp_FlangerVisualizer.h"

namespace bdsp
{


	FlangerVisualizer::FlangerVisualizer(GUI_Universals* universalsToUse, dsp::Flanger<float>* FlangerToUse, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLSpectrumVisualizer(universalsToUse, 200)
	{
		flanger = FlangerToUse;
		lookup = lookupsToUse;

	}

	FlangerVisualizer::~FlangerVisualizer()
	{

	}

	inline float FlangerVisualizer::calculateFrequencyResponse(int channel, float freq)
	{
		float mag = std::abs(flanger->calculateResponseForFrequency(channel, freq));

		//float decibels = juce::Decibels::gainToDecibels(mag);
		//return juce::jmap(decibels, -32.0f, 32.0f, 0.0f, 1.0f);

		//return juce::jmap(mag, 0.5f, 20.0f, 0.0f, 1.0f);
		return mix * juce::jlimit(0.0f, 1.0f, mag / 2);
	}

	inline void FlangerVisualizer::newFrameInit()
	{
		mix = flanger->getWetMix();
	}




} // namespace bdsp