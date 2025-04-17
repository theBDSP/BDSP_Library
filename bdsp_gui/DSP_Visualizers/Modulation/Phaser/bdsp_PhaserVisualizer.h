#pragma once
namespace bdsp
{
	class PhaserVisualizer : public OpenGLCompositeComponent
	{
	public:

		PhaserVisualizer(GUI_Universals* universalsToUse, dsp::Phaser<float>* phaserToUse, dsp::DSP_Universals<float>* lookupsToUse);
		~PhaserVisualizer();

		void generateVertexBuffer() override;
		void setColor(const NamedColorsIdentifier& newLineColor);
		void resized() override;

	private:


		dsp::Phaser<float>* phaser;


	public:
		juce::RangedAudioParameter* numStagesParam = nullptr;
		Parameter* minPhaseParam = nullptr, * maxPhaseParam = nullptr;
		Parameter* centerParam = nullptr, * depthParam = nullptr;
		Parameter* stereoWidthParam = nullptr, * feedbackParam = nullptr;
		Parameter* mixParam = nullptr;

	private:
		int numStages = 2;
		float feedback;
		float mix;

		float xScaling = 1.0f;
		float yScaling = 0.9f;

		dsp::DSP_Universals<float>* lookup;

		juce::Array<OpenGLFunctionVisualizer*> curves;
		OpenGLLineRenderer* dry;
		juce::Array<juce::Point<float>> dryPoints;
		juce::Array<float> stageFactors;
		OpenGLColor color;
	};


} // namespace bdsp