#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Basic Waveshaping distortion.
		 * The tranfer function is implemented through a pointer to a subclass of DistortionTypeBase.
		 */
		template <typename SampleType>
		class DistortionBase : public BaseProcessingUnit<SampleType>
		{
		public:
			/**
			 * Creates a distortion processor
			 * @param lookupToUse Pointer to the main processor's DSP_Universals
			 * @param typeToUse A pointer to the subclass of DistortionTypeBase you want to use for the distortion
			 * @param oversamplingFactor How many stages of oversampling to use when processing the distortion. Each stage doubles the samplerate used.
			 * @param handleLatencyCompensationInternally If true, this processor will delay the dry signal to compensate for latency added by oversampling
			 */
			DistortionBase(DSP_Universals<SampleType>* lookupToUse, DistortionTypeBase<SampleType>* typeToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:oversampling(2, oversamplingFactor, juce::dsp::Oversampling<SampleType>::filterHalfBandPolyphaseIIR, handleLatencyCompensationInternally)
			{
				lookup = lookupToUse;

				type = typeToUse;

				initPre(1);
			}

			/**
			 * Creates a distortion processor
			 * @param lookupToUse Pointer to the main processor's DSP_Universals
			 * @param typeToUse The name of type of distortion you want to use
			 * @param oversamplingFactor How many stages of oversampling to use when processing the distortion. Each stage doubles the samplerate used.
			 * @param handleLatencyCompensationInternally If true, this processor will delay the dry signal to compensate for latency added by oversampling
			 */
			DistortionBase(DSP_Universals<SampleType>* lookupToUse, const juce::String typeToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, lookupToUse->distortionLookups->nameToDistortionType(typeToUse), oversamplingFactor, handleLatencyCompensationInternally)
			{
			}

			virtual ~DistortionBase() = default;

			//================================================================================================================================================================================================

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{

				juce::dsp::ProcessSpec upSepc = { spec.sampleRate * oversampling.getOversamplingFactor(),spec.maximumBlockSize,spec.numChannels }; // process spec after upsampling
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
					// Add latency compensation when bypassed to avoid time jumps
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

			DistortionTypeBase<SampleType>* getType()
			{
				return type;
			}


			bool isScaled = true;

		protected:

			DistortionTypeBase<SampleType>* type;
			juce::SmoothedValue<SampleType> smoothedAmt, smoothedPre;

			Oversampling<SampleType> oversampling;

			DSP_Universals<SampleType>* lookup;

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				if (type != nullptr)
				{
					SampleType scaledInput = inputSample * smoothedPre.getCurrentValue();
					return  type->processSample(scaledInput, smoothedAmt.getCurrentValue(), isScaled);
				}
				else
				{
					return inputSample;
				}
			}
		};



	}// namespace dsp
}// namnepace bdsp


