#pragma once



namespace bdsp
{
	class FlangerVisualizer : public OpenGLSpectrumVisualizer
	{
	public:

		FlangerVisualizer(GUI_Universals* universalsToUse, dsp::Flanger<float>* FlangerToUse, dsp::DSP_Universals<float>* lookupsToUse);
		~FlangerVisualizer();




	private:
		inline float calculateFrequencyResponse(int channel, float freq) override;

		inline void newFrameInit() override;

		dsp::Flanger<float>* flanger;


	public:

		Parameter* baseParam = nullptr, * depthParam = nullptr;
		Parameter* stereoWidthParam = nullptr, * feedbackParam = nullptr;
		Parameter* mixParam = nullptr;

	private:
		float base, depth;
		float stereoWidth, feedback;
		float mix;

		dsp::DSP_Universals<float>* lookup;
	};


} // namespace bdsp