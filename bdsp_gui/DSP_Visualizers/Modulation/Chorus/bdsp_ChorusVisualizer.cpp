#include "bdsp_ChorusVisualizer.h"

namespace bdsp
{


	ChorusVisualizer::ChorusVisualizer(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this)
	{
		chorus = chorusToUse;
		lookup = lookupsToUse;

		int numPoints = 50;
		subClasses.add(new OpenGLLineRenderer(universals, 2 * numPoints)); // dry
		dry = dynamic_cast<OpenGLLineRenderer*>(subClasses.getLast());

		dry->lineVertexBuffer.getFirst()->init(2 * numPoints);
		dryPoints.resize(2 * numPoints);
		for (int i = 0; i < dryPoints.size(); ++i)
		{
			float x = 2.0f * i / (dryPoints.size() - 1.0f) - 1.0f;
			dryPoints.set(i, { x,sin(PI * x) });
		}

		for (int i = 0; i < BDSP_CHORUS_MAX_VOICES; ++i)
		{
			subClasses.add(new OpenGLFunctionVisualizer(universals, false, true, numPoints)); // i-th left voice
			curves.add(dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getLast()));

			subClasses.add(new OpenGLFunctionVisualizer(universals, false, true, numPoints)); // i-th right voice
			curves.add(dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getLast()));
		}

		initSubClasses();

		for (auto* c : curves)
		{
			c->setBipolar(true);
		}

	}

	ChorusVisualizer::~ChorusVisualizer()
	{

	}

	void ChorusVisualizer::generateVertexBuffer()
	{
		numVoices = numVoicesParam != nullptr ? juce::jmap(numVoicesParam->getValue(), (float)BDSP_CHORUS_MIN_VOICES, (float)BDSP_CHORUS_MAX_VOICES) : BDSP_CHORUS_MIN_VOICES;

		depth = depthParam != nullptr ? depthParam->getActualValue() / BDSP_CHORUS_DEPTH_MAX_MS : 1;

		stereoWidth = stereoWidthParam != nullptr ? stereoWidthParam->getActualValue() : 0;

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

		auto fillColor = color.getColorID(true).withMultipliedAlpha(mix / numVoices);
		auto fillColorDisabled = color.getColorID(true).withMultipliedAlpha(mix / numVoices * universals->disabledAlpha);

		int num = curves.getFirst()->lineVertexBuffer.getFirst()->getNumVerticies(); // num vertices in each curve
		for (int i = 0; i < numVoices; ++i)
		{
			curves[2 * i]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i]->topCurve.setColors(fillColor, fillColorDisabled);
			curves[2 * i]->setThickness(-1, universals->visualizerLineThickness / numVoices);

			curves[2 * i + 1]->botCurve.setColors(fillColor.withMultipliedAlpha(0.0f), fillColorDisabled.withMultipliedAlpha(0.0f));
			curves[2 * i + 1]->topCurve.setColors(fillColor, fillColorDisabled);
			curves[2 * i + 1]->setThickness(-1, universals->visualizerLineThickness / numVoices);

			float offset = (float)i / numVoices * sin(2 * PI * chorus->getModPhase()); // distance to offset this voice
			for (int j = 0; j < num; ++j)
			{
				float x = xScaling * (float)j / num;
				float L = yScaling * sin(PI * (-x - depth * offset));
				float R = yScaling * sin(PI * (x - depth * offset - stereoWidth));
				curves[2 * i]->lineVertexBuffer[0]->set(j, { -x,L, r,g,b,a });
				curves[2 * i + 1]->lineVertexBuffer[0]->set(j, { x,R, r,g,b,a });
			}
		}

	}

	void ChorusVisualizer::renderOpenGL()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		juce::OpenGLHelpers::clear(background);

		generateVertexBuffer();

		dry->renderWithoutGenerating();

		for (int i = 0; i < numVoices; ++i) // only render up to the number of voices
		{
			curves[2 * i]->renderWithoutGenerating();
			curves[2 * i + 1]->renderWithoutGenerating();
		}
	}

	void ChorusVisualizer::setColor(const NamedColorsIdentifier& newLineColor)
	{
		color.setColors(newLineColor, newLineColor.withMultipliedAlpha(universals->disabledAlpha));
	}

	void ChorusVisualizer::resized()
	{
		OpenGLCompositeComponent::resized();
		dry->setThickness(-1, universals->visualizerLineThickness);
	}






} // namespace bdsp