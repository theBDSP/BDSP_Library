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
			setFillPos(progress);

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




	//================================================================================================================================================================================================

	LFOVisualizer::LFOVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse)
		:Component(universalsToUse),
		wave(universalsToUse, parentComp, lookupsToUse),
		result(universalsToUse, parentComp->control)
	{
		setInterceptsMouseClicks(false, false);
		wave.setScaling(1.0f, 0.9f);
		result.setScaling(1.0f, 0.9f);

		addAndMakeVisible(wave);
		addAndMakeVisible(result);
	}

	void LFOVisualizer::resized()
	{
		result.setBounds(shrinkRectangleToInt(getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 0, 0.5, 1)).reduced(universals->roundedRectangleCurve)));
		wave.setBounds(shrinkRectangleToInt(getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0.5, 0, 0.5, 1)).reduced(universals->roundedRectangleCurve)));
	}

	void LFOVisualizer::paint(juce::Graphics& g)
	{
		g.setColour(getColor(outlineColor));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);

		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(universals->dividerSize), universals->roundedRectangleCurve - universals->dividerSize);
	}
	void LFOVisualizer::paintOverChildren(juce::Graphics& g)
	{
		g.setColour(getColor(outlineColor));
		g.fillRect(juce::Rectangle<float>(universals->dividerSize, getHeight()).withCentre(getLocalBounds().toFloat().getCentre()));
	}
	LFOWaveVisualizerInternal* LFOVisualizer::getWaveComponent()
	{
		return &wave;
	}
	LFOResultVisualizerInternal* LFOVisualizer::getResultComponent()
	{
		return &result;
	}
	void LFOVisualizer::setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor, const NamedColorsIdentifier& newTopCurveColor, const NamedColorsIdentifier& newBotCurveColor, const NamedColorsIdentifier& newPosColor)
	{
		wave.setColor(newLineColor, newZeroLineColor, newTopCurveColor, newBotCurveColor, newPosColor);
		result.setColor(newLineColor, newZeroLineColor);
	}

} // namespace bdsp
