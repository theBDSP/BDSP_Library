#pragma once




namespace bdsp
{

	template<typename SampleType>
	class LevelMeterController
	{
	public:
		LevelMeterController(dsp::SampleSource<SampleType>* sourceToTrack, SampleType attackMs = 0.0, SampleType decayMs = 25.0);
		~LevelMeterController() = default;

		dsp::StereoSample<SampleType> getStereoLevel();
		void prepare(const juce::dsp::ProcessSpec& spec);

	private:
		dsp::SampleSourceWeakReference<SampleType> source;
		juce::dsp::BallisticsFilter<SampleType> filter;
		int numSamples;
	};

	template<typename SampleType>
	class LevelMeter : public OpenGLComponent
	{
	public:
		LevelMeter(GUI_Universals* universalsToUse, LevelMeterController<SampleType>* controllerToUse, bool isVertical = true, int roundingTriangles = BDSP_OPEN_GL_ROUNDED_RECTANGLE_TRIANGLE_NUMBER);
		~LevelMeter();
		void setColors(const NamedColorsIdentifier& mainColor);
		void resized() override;



	private:
		bool vertical;
		int cornerTriangles;
		int progressDelta = 0;
		int n; // number of indecies for one side of on bar excluding rounding centers

		OpenGLColor color;


		// Inherited via OpenGLCopmponent
		void generateVertexBuffer() override;
		float cornerX = 0, cornerY = 0;

		float border = 0.1;
		LevelMeterController<SampleType>* controller = nullptr;

	};


} // namespace bdsp