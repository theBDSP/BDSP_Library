#pragma once
namespace bdsp
{
	/**
	 * Displays a general visual representation of a chours's state
	 */
	class ChorusVisualizerInternal : public OpenGLCompositeComponent
	{
	public:

		ChorusVisualizerInternal(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse);
		~ChorusVisualizerInternal();


		void setColor(const NamedColorsIdentifier& newColor);
		void generateVertexBuffer() override;
		void renderOpenGL() override;
		void resized() override;
	private:

		dsp::Chorus<float>* chorus;

		OpenGLColor color;


	public:
		juce::RangedAudioParameter* numVoicesParam = nullptr;
		Parameter* depthParam = nullptr;
		Parameter* stereoWidthParam = nullptr;
		Parameter* mixParam = nullptr;

	private:
		int numVoices = 1;
		float depth;
		float stereoWidth;
		float mix;

		float xScaling = 1.0f, yScaling = 0.9f;

		dsp::DSP_Universals<float>* lookup;

		juce::Array<OpenGLFunctionVisualizer*> curves; // the representation of each voice
		OpenGLLineRenderer* dry; // the representation dry signal
		juce::Array<juce::Point<float>> dryPoints;


	};

	class ChorusVisualizer : public OpenGlComponentWrapper<ChorusVisualizerInternal>
	{
	public:
		ChorusVisualizer(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse)
			:OpenGlComponentWrapper<ChorusVisualizerInternal>(universalsToUse, chorusToUse, lookupsToUse)
		{

		}

	};


} // namespace bdsp
