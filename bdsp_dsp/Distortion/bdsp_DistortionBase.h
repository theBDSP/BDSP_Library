#pragma once
namespace bdsp
{
	namespace dsp
	{

		template <typename SampleType>
		class DistortionBase : public BaseProcessingUnit<SampleType>
		{
		public:
			DistortionBase(DSP_Universals<SampleType>* lookupToUse, DistortionTypes type, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:oversampling(2, oversamplingFactor, juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR, handleLatencyCompensationInternally)
			{
				lookup = lookupToUse;

				lookup->distortionLookups->generateTables(type);

				func = lookup->distortionLookups->getDistortionFunc(type);

				//lookup->generateTables(LookupTableTypes({}, { DistortionLookupTables::ArcSinH }));
			}
			//virtual ~DistortionBase() = default;

			//==============================================================================
			/** Initialises the processor. */
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{

				juce::dsp::ProcessSpec upSepc = { spec.sampleRate * oversampling.getOversamplingFactor(),spec.maximumBlockSize,spec.numChannels };
				BaseProcessingUnit<SampleType>::prepare(upSepc);

				oversampling.prepare(spec);

				smoothedAmt.setCurrentAndTargetValue(1.0);
				smoothedPre.setCurrentAndTargetValue(1.0);

				reset();
			}


			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();
				oversampling.reset();
			}

			void processInternal(bool isBypassed) noexcept override
			{



				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					oversampling.mixWithLatencyCompensation(*BaseProcessingUnit<SampleType>::internalContext.get(), BaseProcessingUnit<SampleType>::smoothedDryMix, BaseProcessingUnit<SampleType>::smoothedWetMix, true);

					return;
				}


				auto upBlock = oversampling.processSamplesUp(BaseProcessingUnit<SampleType>::internalDryBlock);

				int numSamples = upBlock.getNumSamples();
				for (int i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					upBlock.setSample(0, i, this->processSample(0, upBlock.getSample(0, i)));
					upBlock.setSample(1, i, this->processSample(1, upBlock.getSample(1, i)));
				}
				oversampling.processSamplesDown(BaseProcessingUnit<SampleType>::internalWetBlock);


				oversampling.mixWithLatencyCompensation(*BaseProcessingUnit<SampleType>::internalContext.get(), BaseProcessingUnit<SampleType>::smoothedDryMix, BaseProcessingUnit<SampleType>::smoothedWetMix);


			}


			void updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();

				smoothedAmt.getNextValue();
				smoothedPre.getNextValue();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);

				smoothedAmt.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				smoothedPre.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}

			void initAmount(SampleType initValue)
			{
				smoothedAmt.setCurrentAndTargetValue(initValue);
			}

			void setAmount(SampleType newValue)
			{
				smoothedAmt.setTargetValue(newValue);
			}

			float getAmount()
			{
				return smoothedAmt.getTargetValue();
			}

			void initPre(SampleType initValue)
			{
				smoothedPre.setCurrentAndTargetValue(initValue);
			}

			void setPre(SampleType newValue)
			{
				smoothedPre.setTargetValue(newValue);
			}

			int getLatency() override
			{
				return oversampling.getLatency();
			}
			bool isScaled = true;

		protected:
			SampleType(DistortionLookups<SampleType>::* func) (SampleType, SampleType, bool);

			juce::SmoothedValue<SampleType> smoothedAmt, smoothedPre;

			Oversampling<SampleType> oversampling;

			DSP_Universals<SampleType>* lookup;

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{

				SampleType scaledInput = inputSample * smoothedPre.getCurrentValue();
				return  ((lookup->distortionLookups.operator->())->*(func))(scaledInput, smoothedAmt.getCurrentValue(), isScaled);
			}
		};



	}// namespace dsp
}// namnepace bdsp


