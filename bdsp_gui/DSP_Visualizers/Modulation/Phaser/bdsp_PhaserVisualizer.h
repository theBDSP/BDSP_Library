#pragma once



namespace bdsp
{
	class PhaserVisualizer : public OpenGLSpectrumVisualizer
	{
	public:

		PhaserVisualizer(GUI_Universals* universalsToUse, dsp::Phaser<float>* phaserToUse, dsp::DSP_Universals<float>* lookupsToUse);
		~PhaserVisualizer();



	private:
		inline float calculateFrequencyResponse(int channel, float freq) override;

		inline void newFrameInit() override;

		dsp::Phaser<float>* phaser;


	public:
		juce::RangedAudioParameter* numStagesParam = nullptr;
		Parameter* allpassQParam = nullptr;
		Parameter* minPhaseParam = nullptr, * maxPhaseParam = nullptr;
		Parameter* centerParam = nullptr, * depthParam = nullptr;
		Parameter* stereoWidthParam = nullptr, * feedbackParam = nullptr;
		Parameter* mixParam = nullptr;

	private:
		int numStages = 2;
		float allpassQ;
		float minPhase, maxPhase;
		float center, depth;
		float stereoWidth, feedback;
		float mix;

		dsp::DSP_Universals<float>* lookup;
	};


} // namespace bdsp