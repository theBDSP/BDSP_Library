#include "bdsp_RingModVisualizer.h"

namespace bdsp
{

	RingModVisualizer::RingModVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookupsToUse, float sampleRateToUse)
		:OpenGlComponentWrapper<RingModVisualizerInternal>(universalsToUse, lookupsToUse, sampleRateToUse)
	{
	}






	RingModVisualizerInternal::RingModVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookupsToUse, float sampleRate)
		:OpenGLFunctionVisualizer(universalsToUse, false, true, BDSP_RING_MOD_VISUALIZER_MAX_PERIODS * 16)
	{
		lookups = lookupsToUse;
		sampleRate = sampleRate;
		setBipolar(true);
		setJointType(-1, OpenGLLineRenderer::JointType::Rounded);
		setCapType(-1, OpenGLLineRenderer::CapType::Round);

	}


	void RingModVisualizerInternal::setSource(dsp::RingModulation<float>::RingModSource src)
	{
		//setEnabled(src == dsp::RingModulation<float>::RingModSource::Tone);
		source = src;
		getParentComponent()->repaint(); // updates background color
	}




	inline float RingModVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		if (source == dsp::RingModulation<float>::RingModSource::Tone)
		{
			return juce::jmap(mix, 1.0f, lookups->waveLookups->getLFOValue(shape, skew, fmodf(normX * freq, 1), true));
		}
		else
		{
			return juce::jmap(mix, 1.0f, signum(openGL_X) * sin(2 * PI * openGL_X * openGL_X) * cos(2 * PI * openGL_X * openGL_X));
		}
	}

	inline void RingModVisualizerInternal::newFrameInit()
	{
		shape = shapeParam == nullptr ? 0 : shapeParam->getActualValue();
		skew = skewParam == nullptr ? 0.5 : skewParam->getActualValue();

		if (freqParam == nullptr)
		{
			freq = BDSP_RING_MOD_VISUALIZER_MAX_PERIODS / 4;
		}
		else
		{
			freq = freqParam->getActualValue();
			freq = juce::jmap(freqParam->convertTo0to1(freq), 0.5f, (float)BDSP_RING_MOD_VISUALIZER_MAX_PERIODS);
		}
		mix = mixParam == nullptr ? 1 : mixParam->getActualValue();
	}

} // namespace bdsp