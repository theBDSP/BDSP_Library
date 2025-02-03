#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Basic Gain unit, simply applies a gain to the signal, the gain value is smoothed to avoid unwanted clicks and pops.
		 */
		template <typename SampleType>
		class Gain : public BaseProcessingUnit<SampleType>
		{
		public:
			Gain() = default;
			~Gain() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);

				BaseProcessingUnit<SampleType>::initMix(1);
			}

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return smoothedGain.getCurrentValue() * inputSample;
			}

			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				return StereoSample<SampleType>(processSample(0, inputSample.left), processSample(1, inputSample.right));
			}

			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock); // copy the input block to the output block
				BaseProcessingUnit<SampleType>::internalWetBlock.multiplyBy(smoothedGain); // apply gain

				BaseProcessingUnit<SampleType>::applyDryWetMix();
			}


			void setGain(SampleType newValue)
			{
				smoothedGain.setTargetValue(newValue);
			}

			void initGain(SampleType initValue)
			{
				smoothedGain.setCurrentAndTargetValue(initValue);
			}

			/**
			 * Starts a fade in from zero to the current target gain value
			 */
			void startFadeFromZero()
			{
				SampleType target = smoothedGain.getTargetValue();
				smoothedGain.setCurrentAndTargetValue(0);
				smoothedGain.setTargetValue(target);
			}

			SampleType getGain()
			{
				return smoothedGain.getCurrentValue();
			}


			void inline updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
				smoothedGain.getNextValue();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
				smoothedGain.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}
		protected:
			juce::SmoothedValue<SampleType> smoothedGain;
		};

	} //namespace dsp
} //namespace bdsp