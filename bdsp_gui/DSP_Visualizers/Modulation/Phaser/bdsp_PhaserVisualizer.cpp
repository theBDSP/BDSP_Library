#include "bdsp_PhaserVisualizer.h"

namespace bdsp
{


	PhaserVisualizer::PhaserVisualizer(GUI_Universals* universalsToUse, dsp::Phaser<float>* phaserToUse, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLSpectrumVisualizer(universalsToUse, 100, true, true)
	{
		phaser = phaserToUse;
		lookup = lookupsToUse;





	}

	PhaserVisualizer::~PhaserVisualizer()
	{

	}
	inline float PhaserVisualizer::calculateFrequencyResponse(int channel, float freq)
	{
		float mag = std::abs(phaser->calculateResponseForFrequency(channel, freq));

		/*float decibels = juce::Decibels::gainToDecibels(mag);
		return juce::jmap(decibels, -32.0f, 32.0f, 0.0f, 1.0f);*/
		return mix * (1 - juce::jlimit(0.0f, 1.0f, (mag / PI)));
	}
	inline void PhaserVisualizer::newFrameInit()
	{
		numStages = numStagesParam != nullptr ? juce::jmap(numStagesParam->getValue(), 1.0f, BDSP_PHASER_MAX_POLES / 2.0f) : 1;
		allpassQ = allpassQParam != nullptr ? allpassQParam->getActualValue() : BDSP_FILTER_DEFAULT_Q;

		minPhase = minPhaseParam != nullptr ? minPhaseParam->getActualValue() : 0;
		maxPhase = maxPhaseParam != nullptr ? maxPhaseParam->getActualValue() : 1;


		center = centerParam != nullptr ? centerParam->getActualValue() : 0.5f;
		depth = depthParam != nullptr ? depthParam->getActualValue() : 1;


		stereoWidth = stereoWidthParam != nullptr ? stereoWidthParam->getActualValue() : 0;
		feedback = feedbackParam != nullptr ? feedbackParam->getActualValue() : 0;


		mix = mixParam != nullptr ? mixParam->getActualValue() : 0.5f;

	}




} // namespace bdsp