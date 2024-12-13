#include "bdsp_DistortionVisualizer.h"

namespace bdsp
{


	DistortionVisualizerInternal::DistortionVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypes initType)
		:OpenGLFunctionVisualizer(universalsToUse)
	{
		setBipolar(true);
		drive = driveSlider;
		mix = mixSlider;
		autoGain = autoGainButton;


		lookup = lookups;
		lookup->distortionLookups->generateSingleTable(initType);

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

	void DistortionVisualizerInternal::setType(dsp::DistortionTypes newType, dsp::DistortionTypes newNegType)
	{
		type = newType;
		if (newNegType == dsp::DistortionTypes(0))
		{
			negType = type;
		}
		else
		{
			negType = newNegType;
		}
		lookup->distortionLookups->generateSingleTable(newType);
		lookup->distortionLookups->generateSingleTable(newNegType);


	}







	void DistortionVisualizerInternal::newFrameInit()
	{
		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());

		//openGLContext.triggerRepaint();
	}

	inline float DistortionVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float wet;
		if (type == dsp::DistortionTypes::BitCrush)
		{
			auto bitrate = juce::jmap(drive->getActualValue(), 4.0, 0.0);
			auto p = pow(2, bitrate);
			wet = round(openGL_X * p) / p;
		}
		else
		{
			wet = lookup->distortionLookups->getDistortion(openGL_X, drive->getActualValue(), autoGain->getToggleState(), openGL_X <= 0 ? negType : type);

		}
		return wet * mixVal + openGL_X * (1 - mixVal);
	}


	//================================================================================================================================================================================================



	DistortionVisualizer::DistortionVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypes initType)
		:OpenGlComponentWrapper<DistortionVisualizerInternal>(universalsToUse, lookups, driveSlider, mixSlider, autoGainButton, initType)
	{
		vis->setScaling(1, 0.9);
	}

	void DistortionVisualizer::resized()
	{
		//forceAspectRatio(1);
		OpenGlComponentWrapper::resized();
	}

	void DistortionVisualizer::setType(dsp::DistortionTypes newType, dsp::DistortionTypes newNegType)
	{

		vis->setType(newType, newNegType);
	}

} // namespace bdsp