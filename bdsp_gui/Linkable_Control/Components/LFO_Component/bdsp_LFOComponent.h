#pragma once



namespace bdsp
{

	class LFOParameterListener
	{
	public:
		LFOParameterListener(dsp::DSP_Universals<float>* lookupsToUse);
		~LFOParameterListener();
		void setParameters(ControlParameter* rateTimeParam, ControlParameter* rateFracParam, juce::AudioParameterChoice* rateDivParam, ControlParameter* shape, ControlParameter* skew, ControlParameter* amplitude, juce::AudioParameterBool* polarity, BPMFollower* bpmFollow);



		void updateVariables(float progressDelta);

		float getV()
		{
			return v;
		}
		float getShape()
		{
			return shape;
		}
		float getSkew()
		{
			return skew;
		}
		float getAmplitude()
		{
			return amplitude;
		}
		float getProgress()
		{
			return progress;
		}
		float getProgressIncrement()
		{
			return progressIncrement;
		}
		float getPeriodMs()
		{
			return periodMs;
		}
		bool getIsBipolar()
		{
			return isBipolar;
		}

		void setProgress(float newProgress) // called when host restarts playing
		{
			progress = newProgress;
		}

		dsp::DSP_Universals<float>* lookups;
	protected:
		float v = 0;// cannot be named value b/c its ambiguous w. variable in linkable control
		float shape = 0;
		float skew = 0.5;
		float amplitude = 1;
		float progress = 0;
		float progressIncrement = 0;
		float periodMs = 0;
		bool isBipolar = true; // cannot be named polarity b/c its ambiguous w/ variable in linkable control
		BPMFollower* BPM = nullptr;

		juce::AudioParameterChoice* rateDivisionParameter;
		ControlParameter* shapeParameter = nullptr, * skewParameter = nullptr, * amplitudeParameter = nullptr, * rateMSParameter = nullptr, * rateFracParameter = nullptr;
		juce::AudioParameterBool* polarityParameter = nullptr;

		bool parametersSet = false;
	};

	class BPMComponent;
	class LFOVisualizer;
	class LFOControllerObject : public LinkableControl, public LFOParameterListener
	{
	public:
		LFOControllerObject(dsp::DSP_Universals<float>* lookupsToUse);
		~LFOControllerObject();

		float update() override;



	};

	class LFOComponent : public GeneralControlComponent
	{
	public:
		LFOComponent(GUI_Universals* universalsToUse, BPMComponent* BPMComp, int lfoIDX, juce::ValueTree* nameValueLocation);
		~LFOComponent();
		void colorSet() override;
		void setControlObject(LinkableControl* newControlObject) override;

		void attach();
		void resized() override;
		void paint(juce::Graphics& g) override;
		void paintOverChildren(juce::Graphics& g) override;

		Component* getVisualizer() override;


	private:

		std::unique_ptr<RangedContainerSyncFraction> rate;
		std::unique_ptr<RangedContainerSlider> shape, skew, amplitude;
		std::unique_ptr<PathButton> polarity;


		std::unique_ptr<Label> rateLabel, shapeLabel, skewLabel, ampLabel;
		int LFOIndex = 0;


		BPMComponent* BPM;

		std::unique_ptr<LFOVisualizer> visualizer;

		juce::Rectangle<float> buttonRect;

		// Inherited via LinkableControlComponent
		juce::Array<juce::WeakReference<HoverableComponent>> getComponentsThatShareHover() override;








	};




	class LFOSection : public TexturedContainerComponent
	{
	public:

		LFOSection(GUI_Universals* universalsToUse, BPMComponent* BPMComp, const NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation);
		virtual ~LFOSection() = default;
		LFOComponent* getLFO(int idx);



		void resized() override;
		void paint(juce::Graphics& g) override;


		void setTitleFontIndex(int index);

		void visibilityChanged() override;

	private:
		juce::OwnedArray<LFOComponent> LFOs;
		juce::OwnedArray<Component> LFOHolders;

		;

	};




}// namnepace bdsp
