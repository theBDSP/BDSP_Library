#pragma once



namespace bdsp
{

	class ChorusVisualizerInternal : public OpenGLComponent
	{
	public:

		ChorusVisualizerInternal(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse);
		~ChorusVisualizerInternal();


		void setColor(const NamedColorsIdentifier& newColor);
		void generateVertexBuffer() override;

		void setScaling(float xScaling, float yScaling);
	private:

		float getModPosition(int voice, bool left = true);



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

		float xScale = 1, yScale = 1;

		dsp::DSP_Universals<float>* lookup;
	};

	class ChorusVisualizer : public OpenGlComponentWrapper<ChorusVisualizerInternal>
	{
	public:
		ChorusVisualizer(GUI_Universals* universalsToUse, dsp::Chorus<float>* chorusToUse, dsp::DSP_Universals<float>* lookupsToUse)
			: OpenGlComponentWrapper<ChorusVisualizerInternal>(universalsToUse,chorusToUse,lookupsToUse)
		{

		}

	};


} // namespace bdsp