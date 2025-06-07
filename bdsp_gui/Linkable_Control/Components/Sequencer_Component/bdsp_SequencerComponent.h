#pragma once

constexpr int BDSP_SEQUENCER_STEPS = 16;

namespace bdsp
{

	enum SequencerShapes :int { Empty, SawDown, AccDown, DecDown, SawUp, AccUp, DecUp, SquareFull, SquareHalf, Triangle, NUM };

	class SequencerParameterListener
	{
	public:
		SequencerParameterListener(dsp::DSP_Universals<float>* lookupsToUse);
		~SequencerParameterListener();
		void setParameters(juce::Array<juce::AudioParameterChoice*> shapes, juce::Array<Parameter*> amts, ControlParameter* rateFracParam, juce::AudioParameterChoice* rateDivParam, juce::AudioParameterInt* numStepsParameter, BPMFollower* bpmFollow);

		float getValueForShape(SequencerShapes shape, float proportion);

		void updateVariables(float progressDelta);

		float getV()
		{
			return v;
		}

		void setProgress(float newProgress) // called when host restarts playing
		{
			progress = newProgress;
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
		int getNumStep()
		{
			return numSteps;
		}

		SequencerShapes getShapeForStep(int step)
		{
			return SequencerShapes(shapeParameters[step]->getIndex());
		}

		juce::AudioParameterChoice* getShapeParameter(int step)
		{
			return shapeParameters[step];
		}
		float getAmtForStep(int step)
		{
			return amtParamters[step]->getActualValue();
		}

		Parameter* getAmtParameter(int step)
		{
			return amtParamters[step];
		}

		dsp::DSP_Universals<float>* lookups;

	protected:
		float v = 0;// cannot be named value b/c its ambiguous w. variable in linkable control
		BPMFollower* BPM = nullptr;
		float progress = 0;
		float progressIncrement = 0;
		float periodMs = 0;
		int numSteps = BDSP_SEQUENCER_STEPS;

		juce::Array<juce::AudioParameterChoice*> shapeParameters;
		juce::Array<Parameter*> amtParamters;
		juce::AudioParameterChoice* rateDivisionParameter;
		ControlParameter* rateFracParameter = nullptr;
		juce::AudioParameterInt* stepsParameter = nullptr;

		bool parametersSet = false;
	};

	class BPMComponent;
	class SequencerVisualizer;
	class SequencerVisualizerNumbers;
	class SequencerControllerObject : public LinkableControl, public SequencerParameterListener
	{
	public:
		SequencerControllerObject(dsp::DSP_Universals<float>* lookupsToUse);
		~SequencerControllerObject();

		float update() override;



	};

	class SequencerSection;

	class SequencerComponent : public GeneralControlComponent
	{
	public:
		SequencerComponent(GUI_Universals* universalsToUse, BPMComponent* BPMComp, int seqIDX, juce::ValueTree* nameValueLocation);
		~SequencerComponent();
		void colorSet() override;
		void setControlObject(LinkableControl* newControlObject) override;

		void attach();
		void resized() override;
		void paint(juce::Graphics& g) override;

		Component* getVisualizer() override;

		void setParentSection(SequencerSection* p);
		SequencerSection* getParentSection();






	private:

		int SequencerIndex = 0;


		BPMComponent* BPM;

		std::unique_ptr<SequencerVisualizer> visualizer;

		std::unique_ptr<RangedContainerSyncFraction> rateFrac;
		std::unique_ptr<NumberSlider> stepsSlider;

		// Inherited via LinkableControlComponent
		juce::Array<juce::WeakReference<HoverableComponent>> getComponentsThatShareHover() override;

		SequencerSection* parentSection = nullptr;
	};




	class SequencerSection : public TexturedContainerComponent
	{
	public:

		SequencerSection(GUI_Universals* universalsToUse, BPMComponent* BPMComp, const NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation);
		virtual ~SequencerSection() = default;
		SequencerComponent* getSequencer(int idx);



		void resized() override;
		void paint(juce::Graphics& g) override;


		void setTitleFontIndex(int index);

		void visibilityChanged() override;

		int getSelectedShape();
		void colorsChanged() override;
		void setVertical(bool shouldBeVertical);

	private:
		juce::OwnedArray<SequencerComponent> Sequencers;
		juce::OwnedArray<Component> SequencerHolders;
		juce::OwnedArray<PathButton> shapeButtons;

		juce::Rectangle<float> buttonRect;
		bool isVertical = true;
	};




}// namnepace bdsp
