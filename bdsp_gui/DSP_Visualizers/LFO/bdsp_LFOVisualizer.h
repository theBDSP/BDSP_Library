#pragma once



namespace bdsp
{
	class LFOWaveVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		LFOWaveVisualizerInternal(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse);
		~LFOWaveVisualizerInternal();

		void paint(juce::Graphics& g) override;

		void resized() override;

		inline float calculateAlpha(float x, float y) override;

		void generateVertexBuffer() override;


		float progress = 0, progressIncrement = 0, shape = 0, skew = 0.5, amplitude = 1.0;
		float progressW = 0;


		dsp::DSP_Universals<float>* lookups;
		LFOComponent* parent;
		LFOControllerObject* controllerObject;

		void newFrameInit() override;
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;
	private:

	};

	class LFOWaveVisualizer : public OpenGlComponentWrapper<LFOWaveVisualizerInternal>
	{
	public:
		LFOWaveVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse);

	};


	//================================================================================================================================================================================================
	class LFOResultVisualizerInternal : public OpenGLControlValuesOverTime
	{
	public:
		LFOResultVisualizerInternal(GUI_Universals* universalsToUse, LinkableControl* control);
		~LFOResultVisualizerInternal();

		void paint(juce::Graphics& g) override;

		void resized() override;

		inline float calculateAlpha(float x, float y) override;





		

		
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;


	};

	class LFOResultVisualizer : public OpenGlComponentWrapper<LFOResultVisualizerInternal>
	{
	public:
		LFOResultVisualizer(GUI_Universals* universalsToUse, LinkableControl* control);

	};


	class LFOVisualizer : public Component
	{
	public:
		LFOVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse);
		virtual ~LFOVisualizer() = default;
		void resized() override;
		void paint(juce::Graphics& g) override;

		LFOWaveVisualizer* getWaveComponent();
		LFOResultVisualizer* getResultComponent();

		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float topCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float botCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA);
	private:
		LFOWaveVisualizer wave;
		LFOResultVisualizer result;
	};

} // namespace bdsp