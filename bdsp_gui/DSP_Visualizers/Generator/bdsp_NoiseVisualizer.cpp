#include "bdsp_NoiseVisualizer.h"

namespace bdsp
{





	bdsp::NoiseVisualizerInternal::NoiseVisualizerInternal(GUI_Universals* universalsToUse)
		:OpenGLFunctionVisualizer(universalsToUse, false)
	{
		randomSamples.resize(samplePoints);


		for (int i = 0; i < randomSamples.size(); ++i)
		{
			randomSamples.set(i, rand.nextFloat() * (1 - minmax));
		}
	}

	NoiseVisualizerInternal::~NoiseVisualizerInternal()
	{
	}



	void NoiseVisualizerInternal::newFrameInit()
	{
		gainVal = gainParam == nullptr ? 1 : gainParam->getActualValue();
		mixVal = mixParam == nullptr ? 1 : mixParam->getActualValue();
		colorVal = colorParam == nullptr ? 1 : colorParam->getActualValue();

		//topAlpha->set(topCurveAlpha * mixVal);
		//botAlpha->set(botCurveAlpha * mixVal);

	}

	float NoiseVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float out = 0;
		if (colorVal < 0)
		{
			out = gainVal * (1 + colorVal * normX - randomSamples[sampleNumber]);
		}
		else
		{
			out = gainVal * (1 + colorVal * (normX - 1) - randomSamples[sampleNumber]);
		}
		return minmax * (2 * out - 1);

	}

	NoiseVisualizer::NoiseVisualizer(GUI_Universals* universalsToUse)
		:OpenGlComponentWrapper<NoiseVisualizerInternal>(universalsToUse)
	{
	}

} // namespace bdsp
