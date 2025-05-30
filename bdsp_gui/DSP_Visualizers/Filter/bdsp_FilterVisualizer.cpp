#include "bdsp_FilterVisualizer.h"

namespace bdsp
{


	FilterVisualizerInternal::FilterVisualizerInternal(GUI_Universals* universalsToUse, dsp::FilterBase<float>* filterToUse, BaseSlider* typeSlider, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* mixSlider, int numSamples)
		:OpenGLFunctionVisualizer(universalsToUse, false, true, numSamples)
	{
		setBipolar(false);
		filter = filterToUse;


		type = typeSlider;

		//================================================================================================================================================================================================
		freq = freqSlider;

		//================================================================================================================================================================================================
		q = qSlider;

		//================================================================================================================================================================================================
		mix = mixSlider;


		newFrameInit();

	}

	FilterVisualizerInternal::~FilterVisualizerInternal()
	{

	}




	void FilterVisualizerInternal::resized()
	{
		if (getWidth() > 0)
		{
			OpenGLFunctionVisualizer::resized();
			newFrameInit();
		}


	}


	void FilterVisualizerInternal::newFrameInit()
	{

		filter->visualizerParameters->frequency = freq->getActualValue();
		filter->visualizerParameters->q = q->getActualValue();
		filter->visualizerParameters->type = type->getActualValue();

		if (mix != nullptr)
		{
			mixVal = mix->getActualValue();
		}

		//decibels = juce::jlimit(-db_scale, db_scale, decibels);

		// create the y pos



		//openGLContext.triggerRepaint();
	}

	inline float FilterVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float mag = filter->getMagnitudeForFrequency(filter->visualizerParameters.get(), freq->getNormalisableRange().convertFrom0to1(normX));
		float decibels = juce::Decibels::gainToDecibels(mag);
		return juce::jmap(decibels * mixVal, -db_scale, db_scale, -1.0f, 1.0f);

	}

	void FilterVisualizerInternal::calculateZeroLine()
	{
		zeroY = -1.0f;
	}






	//================================================================================================================================================================================================


	FilterVisualizer::FilterVisualizer(GUI_Universals* universalsToUse, dsp::FilterBase<float>* filterToUse, BaseSlider* typeSlider, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* mixSlider, int numSamples)
		:OpenGlComponentWrapper<FilterVisualizerInternal>(universalsToUse, filterToUse, typeSlider, freqSlider, qSlider, mixSlider, numSamples)
	{
	}




} // namespace bdsp