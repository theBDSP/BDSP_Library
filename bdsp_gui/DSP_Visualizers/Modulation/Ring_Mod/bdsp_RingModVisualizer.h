#pragma once


#define BDSP_RING_MOD_VISUALIZER_MAX_PERIODS 25
namespace bdsp
{




	class RingModVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		RingModVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookupsToUse, float sampleRateToUse);


		void setSource(dsp::RingModulation<float>::RingModSource src);
		Parameter* freqParam = nullptr, * shapeParam = nullptr, * skewParam = nullptr, * mixParam = nullptr;
	private:
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		inline void newFrameInit() override;

		float  freq = 1, shape = 0, skew = 0.5, mix = 1;

		float sampleRate = 44100.0f;
		dsp::DSP_Universals<float>* lookups = nullptr;

	};


	class RingModVisualizer : public OpenGlComponentWrapper<RingModVisualizerInternal>
	{
	public:
		RingModVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookupsToUse, float sampleRate);
		virtual ~RingModVisualizer() = default;
	};

} // namespace bdsp