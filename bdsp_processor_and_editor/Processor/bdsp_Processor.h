#pragma once

#include "BDSP_Library/bdsp_processor_and_editor/bdsp_processor_and_editor.h"


#include<bitset>




namespace bdsp
{
	class Editor;

	template <class paramType, typename T>
	struct ParameterPointer
	{
		ParameterPointer()
		{

		}

		bool operator > (ParameterPointer<paramType, T>& other)
		{
			return loadedValue > other.get();
		}

		void setParameter(paramType* p)
		{
			param = p;
		}

		virtual void load() = 0;


		T get()
		{
			return loadedValue;
		}

	protected:
		paramType* param;
		T loadedValue;
	};

	struct ParameterPointerFloat : public ParameterPointer<juce::AudioParameterFloat, float>
	{
		void load() override
		{
			loadedValue = param->get();
		}
	};

	struct ParameterPointerBool : public ParameterPointer<juce::AudioParameterBool, bool>
	{
		void load() override
		{
			loadedValue = param->get();
		}
	};

	struct ParameterPointerInt : public ParameterPointer<juce::AudioParameterInt, int>
	{
		void load() override
		{
			loadedValue = param->get();
		}
	};

	struct ParameterPointerChoice : public ParameterPointer<juce::AudioParameterChoice, int>
	{
		void load() override
		{
			loadedValue = param->getIndex();
		}
	};


	struct ParameterPointerControl : public ParameterPointer<ControlParameter, float>
	{
		void load() override
		{
			loadedValue = param->getActualValue();
		}

		void loadNormalized()
		{
			loadedValue = param->getActualValueNormalized();
		}
	};


	class Processor : public juce::AudioProcessor
	{
	public:
		//==============================================================================
		Processor(bool hasSidechain = false);
		~Processor() override;


		//==============================================================================
		void prepareToPlay(double sampleRate, int samplesPerBlock) override;


#ifndef JucePlugin_PreferredChannelConfigurations
		juce::AudioProcessor::BusesProperties generateBusesLayout(bool hasSidechain);
#endif


		//=====================================================================================================================
		void checkPlayHead();
		double updateBPM();
		void updateControls();
		virtual void getParameterValues(); // called at beginning of process block to get all paramter values for the block

		void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

		virtual void processBlockInit(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) = 0; // set all dsp values from parameters here
		virtual void processSubBlock() = 0;
		virtual void processSubBlockBypassed() = 0;
		//==============================================================================
		bool hasEditor() const override;

		//==============================================================================
		const juce::String getName() const override;

		bool acceptsMidi() const override;
		bool producesMidi() const override;
		bool isMidiEffect() const override;
		double getTailLengthSeconds() const override;
		void releaseResources() override;

		//==============================================================================
		int getNumPrograms() override;
		int getCurrentProgram() override;
		void setCurrentProgram(int index) override;
		const juce::String getProgramName(int index) override;
		void changeProgramName(int index, const juce::String& newName) override;

		//==============================================================================
		void getStateInformation(juce::MemoryBlock& destData) override;
		void setStateInformation(const void* data, int sizeInBytes) override;

		juce::Array<ControlParameter*>& getControlParameters();

		virtual void initParameters() = 0; // called on processor creation, can also be called when you want parameters to snap to current values without smoothing
		juce::UndoManager undoManager;

		double getControlPeriodMs();// returns the control rate period in ms (1/controlrate)


		void updateUIProperties(float newWidth);

		int getBaseLatency();

	protected:
		void init();
		void createBPMParameter(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int versionHint = 1);



		void createSyncParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, int denominator = 16, bool ranged = true, bool createLockParameter = false, int versionHint = 1, int snapVersionHint = -1);
		void createSyncParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, float defaultMsTime, float defaultFrac, int defaultDivision, int denominator = 16, bool ranged = true, bool createLockParameter = false, int versionHint = 1, int snapVersionHint = -1);

		void createSyncRateParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, bool ranged = true, bool createLockParameter = false, int versionHint = 1, int snapVersionHint = -1);
		void createSyncRateParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::String baseID, const juce::String& baseName, float defaultHzRate, float defaultFrac, int defaultDivision, bool ranged = true, bool createLockParameter = false, int versionHint = 1, int snapVersionHint = -1);

		void createControlParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, juce::Array<int> versionHints = juce::Array<int>()); //  creates parameters for each control component

		virtual juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() = 0;
		virtual void createLinkableIDs();

		virtual void setFactoryPresets() = 0;
		juce::Array<FactoryPreset> factoryPresets;

		//=====================================================================================================================
		std::function<float(const juce::String&)> fracLambda16Text, fracLambda32Text;


		std::function<juce::String(float, int)> fracLambda16, fracLambda32;


		std::function<std::function<juce::String(float, int)>(int)> fracLambda;
		std::function<std::function<float(const juce::String&)>(int)>fracLambdaText;




	public:
		juce::WeakReference<Editor> editor;
		juce::Image texture;

		std::unique_ptr<juce::AudioProcessorValueTreeState> parameters;





		dsp::DSP_Universals<float> lookups;

		std::unique_ptr<StateHandler> Handler;



		std::unique_ptr<dsp::DemoTimer<float>> Demo;


		bdsp::PropertiesFolder propertiesFolder;

		std::unique_ptr<bdsp::BPMFollower> BPMFollow;

		bdsp::Settings settings;


		juce::ValueTree settingsTree;




		std::atomic<float>* BPMVal;

		juce::Array<LinkableControl*> sampleRateLinkableControls;
		juce::Array<LFOParameterListener*> LFOParamListeners;



		double wholeNote;


		juce::AudioPlayHead* APH;
		juce::Optional<juce::AudioPlayHead::PositionInfo> currentAPHPosition;

		juce::AudioPlayHead::PositionInfo defaultAPH;
		bool prevPlayingState = false; //represent whether the APH was playing last proces block
		int64_t prevPlayPosition = 0, currentPlayPosition = 0;

		juce::StringArray linkableControlIDs, linkableControlNames;


		RepaintThreadChecker repaintChecker;

		juce::Array<ControlParameter*> controlParameters;
		juce::OwnedArray<LinkableControl> controlObjects;
		juce::OwnedArray< OpenGLControlValuesListener> controlVisListeners;

		GenericParameterAttributes parameterAttributes;

		dsp::SampleSourceList<float> envelopeSourceList;
		bdsp::dsp::SampleSource<float>* inputSource,* sidechainSource;

		juce::Array<DSPComponent*> dspComps;

		bool specSet = false; // has the process spec been set
		double smpRate;
		std::unique_ptr<juce::dsp::ProcessSpec> subBlockSpec;
		juce::uint32 smpsPerBlock, outputChannels;

		std::atomic<bool> sidechainEnabled = false;
		bool sidechainExists = false;

		juce::AudioBuffer<float> subBlockBuffer, prevSubBlockBuffer, sideChainSubBlockBuffer;
		juce::dsp::AudioBlock<float> subBlock;

		int mainBufferSample;


	private:

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
			unsigned int subBlockCounter = 0;
		unsigned int subBlockTarget = 500;

		juce::Array<juce::Point<float>> rateSnapValues;

		std::function<void()> onParameterCreationCompletion; // function to be done once all parameters have been created and added to the APVTS

	};

}// namnepace bdsp
