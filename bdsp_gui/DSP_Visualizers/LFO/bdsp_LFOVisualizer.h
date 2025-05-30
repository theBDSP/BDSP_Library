#pragma once



namespace bdsp
{
	class LFOWaveVisualizerInternal : public OpenGLFunctionVisualizer
	{
	public:
		LFOWaveVisualizerInternal(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse);
		~LFOWaveVisualizerInternal();

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



	//================================================================================================================================================================================================
	class LFOResultVisualizerInternal : public OpenGLControlValuesOverTime
	{
	public:
		LFOResultVisualizerInternal(GUI_Universals* universalsToUse, LinkableControl* control);
		~LFOResultVisualizerInternal();

		void paint(juce::Graphics& g) override;

		void resized() override;



		
		inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX) override;


	};




	class LFOVisualizer : public Component
	{
	public:
		LFOVisualizer(GUI_Universals* universalsToUse, LFOComponent* parentComp, dsp::DSP_Universals<float>* lookupsToUse);
		virtual ~LFOVisualizer() = default;
		void resized() override;
		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		LFOWaveVisualizerInternal* getWaveComponent();
		LFOResultVisualizerInternal* getResultComponent();


		void setOutlineColor(const NamedColorsIdentifier& color)
		{
			outlineColor = color;
			repaint();
		}

		void setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newTopCurveColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newBotCurveColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newPosColor = NamedColorsIdentifier());
	private:
		LFOWaveVisualizerInternal wave;
		LFOResultVisualizerInternal result;
		NamedColorsIdentifier outlineColor = BDSP_COLOR_DARK;

	};

} // namespace bdsp