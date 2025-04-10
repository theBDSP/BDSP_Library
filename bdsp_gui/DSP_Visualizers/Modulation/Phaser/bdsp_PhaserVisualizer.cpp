#include "bdsp_PhaserVisualizer.h"

namespace bdsp
{


	PhaserVisualizer::PhaserVisualizer(GUI_Universals* universalsToUse, dsp::Phaser<float>* phaserToUse, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this)
	{
		phaser = phaserToUse;
		lookup = lookupsToUse;

		int numPoints = 50;

		subClasses.add(new OpenGLLineRenderer(universals, 2 * numPoints));
		dry = dynamic_cast<OpenGLLineRenderer*>(subClasses.getLast());


		dry->lineVertexBuffer.getFirst()->init(2 * numPoints);
		dryPoints.resize(2 * numPoints);
		for (int i = 0; i < dryPoints.size(); ++i)
		{
			float x = 2.0f * i / (dryPoints.size() - 1.0f) - 1.0f;
			dryPoints.set(i, { x,sin(PI * x) });
		}

		for (int i = 0; i < BDSP_PHASER_MAX_POLES; ++i)
		{
			subClasses.add(new OpenGLFunctionVisualizer(universals, false, true, numPoints));
			curves.add(dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getLast()));
			curves.getLast()->setBipolar(true);

			subClasses.add(new OpenGLFunctionVisualizer(universals, false, true, numPoints));
			curves.add(dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getLast()));
			curves.getLast()->setBipolar(true);

			stageFactors.add(1.0 / sqrt((i + 1)));
		}

		initSubClasses();


	}

	PhaserVisualizer::~PhaserVisualizer()
	{

	}

	void PhaserVisualizer::generateVertexBuffer()
	{
		numStages = numStagesParam != nullptr ? juce::jmap(numStagesParam->getValue(), 1.0f, BDSP_PHASER_MAX_POLES / 2.0f) : 4.0f;

		feedback = feedbackParam != nullptr ? feedbackParam->getActualValue() : 0;

		mix = mixParam != nullptr ? mixParam->getActualValue() : 0.5f;

		float r, g, b, a;
		color.getComponents(r, g, b, a);
		float dryA = a * (1 - mix);
		a *= mix;
		float lPos = phaser->getModPosition(true);
		float rPos = phaser->getModPosition(false);

		auto fillColor = color.getColorID(true).withMultipliedAlpha(mix / numStages * abs(feedback));
		auto fillColorDisabled = color.getColorID(true).withMultipliedAlpha(mix / numStages * universals->disabledAlpha * abs(feedback));


		float gray = (r + g + b) / 3.0f;
		for (int i = 0; i < dryPoints.size(); ++i)
		{
			dry->lineVertexBuffer[0]->set(i, { xScaling * dryPoints[i].x, yScaling * dryPoints[i].y,gray,gray,gray,dryA });
		}

		for (int i = 0; i < 2 * numStages; ++i)
		{
			curves[2 * i]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i]->topCurve.setColors(fillColor, fillColorDisabled);

			curves[2 * i + 1]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i + 1]->topCurve.setColors(fillColor, fillColorDisabled);

			for (int j = 0; j < curves[i]->lineVertexBuffer.getFirst()->getNumVerticies(); ++j)
			{
				float x = xScaling * (float)j / curves[i]->lineVertexBuffer.getFirst()->getNumVerticies();
				float L = yScaling * sin(PI * (-x - (i + 1) * lPos / numStages + (i + 1.0f) / (2 * numStages))) * stageFactors[i];
				float R = yScaling * sin(PI * (x - (i + 1) * rPos / numStages + (i + 1.0f) / (2 * numStages))) * stageFactors[i];
				curves[2 * i]->lineVertexBuffer[0]->set(j, { -x,L, r,g,b,a });
				curves[2 * i + 1]->lineVertexBuffer[0]->set(j, { x,R, r,g,b,a });
			}
		}

	}

	void PhaserVisualizer::setColor(const NamedColorsIdentifier& newLineColor)
	{
		color.setColors(newLineColor, newLineColor.withMultipliedAlpha(universals->disabledAlpha));
	}

	void PhaserVisualizer::resized()
	{
		OpenGLCompositeComponent::resized();
		dry->setThickness(-1, universals->visualizerLineThickness);
		for (int i = 0; i < curves.size(); ++i)
		{
			curves[i]->setThickness(-1, universals->visualizerLineThickness / (i / 2 + 1));
		}
	}






} // namespace bdsp