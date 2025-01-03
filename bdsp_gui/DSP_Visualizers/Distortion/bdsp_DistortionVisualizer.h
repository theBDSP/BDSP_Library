#pragma once



namespace bdsp
{
	class DistortionVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		DistortionVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypeBase<float>* initType = nullptr);
		~DistortionVisualizerInternal();


		void resized() override;
		void setType(dsp::DistortionTypeBase<float>* newType, dsp::DistortionTypeBase<float>* newNegType = nullptr);


	private:
		//TODO switch to parameter references

		dsp::DSP_Universals<float>* lookup;
		BaseSlider* drive, * mix;
		juce::Button* autoGain;

		void newFrameInit() override;

		double mixVal;


		// Inherited via OpenGLFunctionVisualizer
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		dsp::DistortionTypeBase<float>* type;
		dsp::DistortionTypeBase<float>* negType;

	};

	class DistortionVisualizer : public OpenGlComponentWrapper<DistortionVisualizerInternal>
	{
	public:
		DistortionVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypeBase<float>* initType = nullptr);
		void resized() override;
		void setType(dsp::DistortionTypeBase<float>* newType, dsp::DistortionTypeBase<float>* newNegType = nullptr);

	};



} // namespace bdsp