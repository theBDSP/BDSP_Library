#include "bdsp_FlangerVisualizer.h"
namespace bdsp
{
	FlangerVisualizerInternal::FlangerVisualizerInternal(GUI_Universals* universalsToUse, dsp::Flanger<float>* FlangerToUse, dsp::DSP_Universals<float>* lookupsToUse, int numSamplePoints)
		:OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this)
	{
		flanger = FlangerToUse;
		lookup = lookupsToUse;


		for (int i = 0; i < numSamplePoints; ++i)
		{
			float x = juce::jmap((i / (numSamplePoints - 1.0f)), -2.0f / numEchos, 2.0f / numEchos);
			dryPoints.set(i, { x,sin(numEchos * PI * abs(x)) });
		}

		subClasses.add(new OpenGLLineRenderer(universals,1, numSamplePoints)); //dry
		dry = dynamic_cast<OpenGLLineRenderer*>(subClasses.getLast());


		for (int i = 0; i < 2 * numEchos; ++i)
		{
			subClasses.add(new OpenGLFunctionVisualizer(universals, false, true, numSamplePoints));
			curves.add(dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getLast()));
			curves.getLast()->setBipolar(true);
		}

		initSubClasses();
	}

	FlangerVisualizerInternal::~FlangerVisualizerInternal()
	{

	}

	void FlangerVisualizerInternal::setColor(const NamedColorsIdentifier& newColor)
	{
		color.setColors(newColor, newColor.withMultipliedAlpha(universals->disabledAlpha));
	}

	void FlangerVisualizerInternal::generateVertexBuffer()
	{
		feedback = feedbackParam != nullptr ? feedbackParam->getActualValue() : 0.5;


		mix = mixParam != nullptr ? mixParam->getActualValue() : 0.5f;

		float r, g, b, a;
		color.getComponents(r, g, b, a);
		float dryA = a * (1 - mix);
		a *= mix;

		float gray = (r + g + b) / 3.0f;
		for (int i = 0; i < dryPoints.size(); ++i)
		{
			dry->lineVertexBuffer[0]->set(i, { xScaling * dryPoints[i].x, yScaling * dryPoints[i].y,gray,gray,gray,dryA });
		}

		auto fillColor = color.getColorID(true).withMultipliedAlpha(mix * abs(feedback));
		auto fillColorDisabled = color.getColorID(true).withMultipliedAlpha(mix * abs(feedback) * universals->disabledAlpha);

		float offsetL = 0;
		float offsetR = 0;
		float fb = 1;
		for (int i = 0; i < numEchos; ++i)
		{
			curves[2 * i]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i]->topCurve.setColors(fillColor, fillColorDisabled);

			curves[2 * i + 1]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i + 1]->topCurve.setColors(fillColor, fillColorDisabled);

			fb *= sqrt(abs(feedback)); // get the next delay's amplitude
			offsetL += flanger->getModValue(0);
			offsetR += flanger->getModValue(1);

			float startL = offsetL / numEchos;
			float startR = offsetR / numEchos;
			for (int j = 0; j < curves[i]->lineVertexBuffer.getFirst()->getNumVerticies(); ++j)
			{
				float prop = (float)j / curves[i]->lineVertexBuffer.getFirst()->getNumVerticies();
				float xL = xScaling * (startL + prop * 2 / numEchos);
				float xR = xScaling * (startR + prop * 2 / numEchos);
				float v = signum(feedback) * fb * yScaling * sin(2 * PI * prop);
				curves[2 * i]->lineVertexBuffer[0]->set(j, { -xL,v, r,g,b,a });
				curves[2 * i + 1]->lineVertexBuffer[0]->set(j, { xR,v, r,g,b,a });
			}
		}

	}


	void FlangerVisualizerInternal::resized()
	{
		OpenGLCompositeComponent::resized();
		dry->setThickness(-1, universals->visualizerLineThickness / 2);
		for (int i = 0; i < numEchos; ++i)
		{
			curves[2 * i]->setThickness(-1, universals->visualizerLineThickness / (i + 2));
			curves[2 * i + 1]->setThickness(-1, universals->visualizerLineThickness / (i + 2));

		}
	}







} // namespace bdsp
