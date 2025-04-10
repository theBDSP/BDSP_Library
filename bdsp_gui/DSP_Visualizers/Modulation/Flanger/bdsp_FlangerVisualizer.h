#pragma once
namespace bdsp
{
	/**
	 * Displays a general visual representation of a flanger's state
	 */
	class FlangerVisualizer : public OpenGLCompositeComponent
	{
	public:

		FlangerVisualizer(GUI_Universals* universalsToUse, dsp::Flanger<float>* FlangerToUse, dsp::DSP_Universals<float>* lookupsToUse, int numSamplePoints = 50);
		~FlangerVisualizer();

		void setColor(const NamedColorsIdentifier& newColor);
		void generateVertexBuffer() override;
		void resized() override;

	private:
		
		dsp::Flanger<float>* flanger;

		OpenGLColor color;

	public:

		Parameter* feedbackParam = nullptr;
		Parameter* mixParam = nullptr;

	private:
		float stereoWidth, feedback;
		float mix;

		float xScaling = 1.0f, yScaling = 0.9f;

		dsp::DSP_Universals<float>* lookup;

		juce::Array<OpenGLFunctionVisualizer*> curves;
		OpenGLLineRenderer* dry;
		juce::Array<juce::Point<float>> dryPoints;

		int numEchos = 8; // number of echos to display
	};


} // namespace bdsp