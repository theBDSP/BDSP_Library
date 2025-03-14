#include "bdsp_LFOVisualizer.h"

namespace bdsp
{


	LFOWaveVisualizerInternal::LFOWaveVisualizerInternal(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse)
		:OpenGLFunctionVisualizer(universalsToUse, true, true, 100)
	{
		parent = parentComp;
		controllerObject = dynamic_cast<LFOControllerObject*>(parent->control);

		lookups = lookupsToUse;

		setJointType(0, OpenGLLineRenderer::JointType::Rounded);
		setCapType(0, OpenGLLineRenderer::CapType::Butt);


		newFrameInit();
	}


	LFOWaveVisualizerInternal::~LFOWaveVisualizerInternal()
	{
	}



	void LFOWaveVisualizerInternal::generateVertexBuffer()
	{
		OpenGLFunctionVisualizer::generateVertexBuffer();

	}
	void LFOWaveVisualizerInternal::newFrameInit()
	{
		if (fillShaderProgram.operator bool())
		{

			progress = controllerObject->getProgress();
			pos->set(progress);

			progressIncrement = controllerObject->getProgressIncrement();
			progressW = 0.4f * powf(progressIncrement / 0.4f, 0.25f);
			width->set(progressW);

			shape = controllerObject->getShape();
			skew = controllerObject->getSkew();
			amplitude = controllerObject->getAmplitude();
			setBipolar(controllerObject->getIsBipolar());
		}

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
		setThicknessRamp(0, 0);
		setJointType(0, OpenGLLineRenderer::JointType::Rounded);
		setCapType(0, OpenGLLineRenderer::CapType::Round);
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
		result.setBoundsRelative(0, 0, 0.5, 1);
		wave.setBoundsRelative(0.5, 0, 0.5, 1);

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
	void LFOVisualizer::setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor, const NamedColorsIdentifier& newTopCurveColor, const NamedColorsIdentifier& newBotCurveColor, const NamedColorsIdentifier& newPosColor)
	{
		wave.getVis()->setColor(newLineColor, newZeroLineColor, newTopCurveColor, newBotCurveColor, newPosColor);
		result.getVis()->setColor(newLineColor, newZeroLineColor);
	}

} // namespace bdsp