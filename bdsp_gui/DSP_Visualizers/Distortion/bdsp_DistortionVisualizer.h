#pragma once



namespace bdsp
{
	class DistortionVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		DistortionVisualizerInternal(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypes initType = dsp::DistortionTypes::ArcSinH);
		~DistortionVisualizerInternal();


		void resized() override;
		void setType(dsp::DistortionTypes newType, dsp::DistortionTypes newNegType = dsp::DistortionTypes(0));


	private:
		//TODO switch to parameter references

		dsp::DSP_Universals<float>* lookup;
		BaseSlider* drive, * mix;
		juce::Button* autoGain;

		void newFrameInit() override;

		double mixVal;


		// Inherited via OpenGLFunctionVisualizer
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
		dsp::DistortionTypes type = bdsp::dsp::DistortionTypes::ArcSinH;
		dsp::DistortionTypes negType = bdsp::dsp::DistortionTypes::ArcSinH;

	};

	class DistortionVisualizer : public OpenGlComponentWrapper<DistortionVisualizerInternal>
	{
	public:
		DistortionVisualizer(GUI_Universals* universalsToUse, dsp::DSP_Universals<float>* lookups, BaseSlider* driveSlider, BaseSlider* mixSlider, juce::Button* autoGainButton, dsp::DistortionTypes initType = dsp::DistortionTypes::ArcSinH);
		void resized() override;
		void setType(dsp::DistortionTypes newType, dsp::DistortionTypes newNegType = dsp::DistortionTypes(0));

	};



} // namespace bdsp