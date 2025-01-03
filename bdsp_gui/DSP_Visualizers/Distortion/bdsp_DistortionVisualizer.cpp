#include "bdsp_DistortionVisualizer.h"

namespace bdsp
{


	DistortionVisualizerInternal::DistortionVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypeBase<float>* initType)
		:OpenGLFunctionVisualizer(universalsToUse)
	{
		setBipolar(true);
		drive = driveSlider;
		mix = mixSlider;
		autoGain = autoGainButton;


		lookup = lookups;

		setType(initType);
		newFrameInit();
	}


	DistortionVisualizerInternal::~DistortionVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}




	void DistortionVisualizerInternal::resized()
	{
		OpenGLFunctionVisualizer::resized();
	}

	void DistortionVisualizerInternal::setType(dsp::DistortionTypeBase<float>* newType, dsp::DistortionTypeBase<float>* newNegType)
	{
		type = newType;
		if (newNegType == nullptr)
		{
			negType = type;
		}
		else
		{
			negType = newNegType;
		}

	}







	void DistortionVisualizerInternal::newFrameInit()
	{
		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());

		//openGLContext.triggerRepaint();
	}

	inline float DistortionVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float wet = openGL_X;
		if (openGL_X <= 0 && negType != nullptr)
		{
			wet = negType->processSample(openGL_X, drive->getActualValue(), autoGain->getToggleState());
		}
		else if (type != nullptr)
		{
			wet = type->processSample(openGL_X, drive->getActualValue(), autoGain->getToggleState());
		}

		return wet * mixVal + openGL_X * (1 - mixVal);
	}


	//================================================================================================================================================================================================



	DistortionVisualizer::DistortionVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypeBase<float>* initType)
		:OpenGlComponentWrapper<DistortionVisualizerInternal>(universalsToUse, lookups, driveSlider, mixSlider, autoGainButton, initType)
	{
		vis->setScaling(1, 0.9);
	}


	void DistortionVisualizer::resized()
	{
		//forceAspectRatio(1);
		OpenGlComponentWrapper::resized();
	}

	void DistortionVisualizer::setType(dsp::DistortionTypeBase<float>* newType, dsp::DistortionTypeBase<float>* newNegType)
	{
		vis->setType(newType, newNegType);
	}

} // namespace bdsp