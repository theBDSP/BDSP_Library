#pragma once



namespace bdsp
{
	class FilterVisualizerInternal final : public OpenGLFunctionVisualizer
	{
	public:

		FilterVisualizerInternal(GUI_Universals* universalsToUse, dsp::FilterBase<float>* filterToUse, BaseSlider* typeSlider, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* mixSlider, int numSamples);
		~FilterVisualizerInternal();


		void resized() override;
	private:



		void newFrameInit() override;

		BaseSlider* freq, * type, * q, * mix;
		dsp::FilterBase<float>* filter;


		float db_scale = 36.0f;
		float mixVal = 1.0f;






		// Inherited via OpenGLFunctionVisualizer
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		void calculateZeroLine() override;

	};
	class FilterVisualizer : public OpenGlComponentWrapper<FilterVisualizerInternal>
	{
	public:
		FilterVisualizer(GUI_Universals* universalsToUse, dsp::FilterBase<float>* filterToUse, BaseSlider* typeSlider, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* mixSlider = nullptr, int numSamples = 200);
		void resized() override;



	};

} // namespace bdsp