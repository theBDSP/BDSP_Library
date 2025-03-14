#pragma once



namespace bdsp
{
	template <typename SampleType>
	class CompressorVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		CompressorVisualizerInternal(GUI_Universals* universalsToUse, dsp::Compressor<SampleType>* compToVis);
		~CompressorVisualizerInternal();


		void resized() override;

		void newFrameInit() override;

		void generateVertexBuffer() override;
	private:









		float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;





		dsp::Compressor<SampleType>* compressor;

		SampleType envVal = 0, gainReduction = 0;

	};

	template <typename SampleType>
	class CompressorVisualizer : public OpenGlComponentWrapper<CompressorVisualizerInternal<SampleType>>
	{
	public:
		CompressorVisualizer(GUI_Universals* universalsToUse, dsp::Compressor<SampleType>* compToVis);
		virtual ~CompressorVisualizer() = default;



	};






} // namespace bdsp