#include "bdsp_LFOVisualizer.h"

namespace bdsp
{


	LFOWaveVisualizerInternal::LFOWaveVisualizerInternal(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLFunctionVisualizer(universalsToUse, true, true, 100)
	{
		parent = parentComp;
		controllerObject = dynamic_cast<LFOControllerObject*>(parent->control);

		lookups = lookupsToUse;


		newFrameInit();
	}


	LFOWaveVisualizerInternal::~LFOWaveVisualizerInternal()
	{
	}


	void LFOWaveVisualizerInternal::paint(juce::Graphics& g)
	{

	}

	void LFOWaveVisualizerInternal::resized()
	{
		OpenGLFunctionVisualizer::resized();
		lineThicknessX.set(0, lineThicknessX.getFirst());
		lineThicknessY.set(0, lineThicknessY.getFirst());
	}
	inline float LFOWaveVisualizerInternal::calculateAlpha(float x, float y)
	{
		float p = scalingX * ((2 * progress) - 1);
		float scale = 4;

		if (x >= (p - progressW) && x <= p)
		{
			return (-(scale - 1) * powf(abs(x - p) / progressW, 1.0 / 3.0) + scale) * OpenGLFunctionVisualizer::calculateAlpha(x, y);
		}
		else if (x >= (p - progressW + 2))
		{
			return (-(scale - 1) * powf(abs(x - p - 2) / progressW, 1.0 / 3.0) + scale) * OpenGLFunctionVisualizer::calculateAlpha(x, y);
		}
		else
		{
			return OpenGLFunctionVisualizer::calculateAlpha(x, y);
		}

	}
	void LFOWaveVisualizerInternal::generateVertexBuffer()
	{
		OpenGLFunctionVisualizer::generateVertexBuffer();
		
	}
	void LFOWaveVisualizerInternal::newFrameInit()
	{
		progress = controllerObject->getProgress();
		progressIncrement = controllerObject->getProgressIncrement();
		progressW = 5 * powf(progressIncrement, 1.0 / 3.0);

		shape = controllerObject->getShape();
		skew = controllerObject->getSkew();
		amplitude = controllerObject->getAmplitude();
		setBipolar(controllerObject->getIsBipolar());
	}

	inline float LFOWaveVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		return lookups->waveLookups->getLFOValue(shape, skew, normX, true, amplitude) + (isBipolar ? 0 : (amplitude - 1));
	}










	//================================================================================================================================================================================================




	LFOWaveVisualizer::LFOWaveVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGlComponentWrapper<LFOWaveVisualizerInternal>(universalsToUse, parentComp, lookupsToUse)
	{

	}







	//================================================================================================================================================================================================



	LFOResultVisualizerInternal::LFOResultVisualizerInternal(GUI_Universals* universalsToUse, LinkableControl* control)
		:OpenGLControlValuesOverTime(control, universalsToUse, false, false)
	{
		forceFullRange = true;
		setThicknessRamp(0,0);
	}

	LFOResultVisualizerInternal::~LFOResultVisualizerInternal()
	{
	}

	void LFOResultVisualizerInternal::paint(juce::Graphics& g)
	{
	}

	void LFOResultVisualizerInternal::resized()
	{
		OpenGLControlValuesOverTime::resized();
	}

	inline float LFOResultVisualizerInternal::calculateAlpha(float x, float y)
	{
		return 0.0f;
	}


	inline float LFOResultVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		return 0.0f;
	}


	LFOResultVisualizer::LFOResultVisualizer(GUI_Universals* universalsToUse, LinkableControl* control)
		:OpenGlComponentWrapper<LFOResultVisualizerInternal>(universalsToUse, control)
	{

	}



	//================================================================================================================================================================================================

	LFOVisualizer::LFOVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse)
		:Component(universalsToUse),
		wave(universalsToUse, parentComp, lookupsToUse),
		result(universalsToUse, parentComp->control)
	{
		setInterceptsMouseClicks(false, false);
		wave.getVis()->setScaling(1.0f, 0.9f);
		result.getVis()->setScaling(1.0f, 0.9f);

		addAndMakeVisible(wave);
		addAndMakeVisible(result);
	}

	void LFOVisualizer::resized()
	{
		wave.setBoundsRelative(0, 0, 0.5, 1);
		result.setBoundsRelative(0.5, 0, 0.5, 1);

	}

	void LFOVisualizer::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		//g.fillRoundedRectangle(wave.getBounds().toFloat(), universals->roundedRectangleCurve);
		//g.fillRoundedRectangle(result.getBounds().toFloat(), universals->roundedRectangleCurve);
		juce::Path p;
		//p.addRoundedRectangle(0, 0, getWidth(), getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, true, false, true, false);
		p.addRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		g.fillPath(p);
	}
	LFOWaveVisualizer* LFOVisualizer::getWaveComponent()
	{
		return &wave;
	}
	LFOResultVisualizer* LFOVisualizer::getResultComponent()
	{
		return &result;
	}
	void LFOVisualizer::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity)
	{
		wave.setColor(c, line, topCurveOpacity, botCurveOpacity);
		result.setColor(c, line);
	}

} // namespace bdsp