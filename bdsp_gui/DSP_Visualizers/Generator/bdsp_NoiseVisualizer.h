#pragma once



namespace bdsp
{
	class NoiseVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		NoiseVisualizerInternal(GUI_Universals* universalsToUse);
		~NoiseVisualizerInternal();


		void newFrameInit() override;



		Parameter* gainParam = nullptr;
		Parameter* mixParam = nullptr;
		Parameter* colorParam = nullptr;

	private:









		float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;

		float calculateAlpha(float x, float y) override; // calculates alpha of area under curve based on height of function





		juce::Array<float> randomSamples;
		juce::Random rand;

		float gainVal = 0, colorVal = 0, mixVal = 1;

		float minmax = 0.8;

	};

	class NoiseVisualizer : public OpenGlComponentWrapper<NoiseVisualizerInternal>
	{
	public:
		NoiseVisualizer(GUI_Universals* universalsToUse);
		virtual ~NoiseVisualizer() = default;



	};




} // namespace bdsp