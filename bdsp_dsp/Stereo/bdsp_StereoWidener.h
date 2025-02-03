#pragma once
#define BDSP_STEREO_WIDTH_MAX_DELAY_MS 30

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Simple stereo widener with built in panning before widening, slightly delayR one channel of the signal to create stereo seperation.
		 */
		template <typename SampleType>
		class StereoWidener : public StereoPanner<SampleType>
		{
		public:
			StereoWidener(DSP_Universals<SampleType>* lookupToUse)
				:StereoPanner<SampleType>(lookupToUse)
			{

			}
			~StereoWidener() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				StereoPanner<SampleType>::prepare(spec);

				juce::dsp::ProcessSpec monoSpec = juce::dsp::ProcessSpec({ spec.sampleRate,  spec.maximumBlockSize, 1 });

				maxDelay = spec.sampleRate * BDSP_STEREO_WIDTH_MAX_DELAY_MS / 1000.0;

				delayL = std::make_unique< DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>>(maxDelay, StereoPanner<SampleType>::lookup);
				delayR = std::make_unique< DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>>(maxDelay, StereoPanner<SampleType>::lookup);
				delayL->prepare(monoSpec);
				delayR->prepare(monoSpec);
			}



			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);


				for (int i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();

					StereoSample<SampleType> panned = StereoPanner<SampleType>::processSampleStereo(StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)));
					delayL->pushSample(0, panned.left);
					delayR->pushSample(0, panned.right);

					auto popped = StereoSample<SampleType>(delayL->popSampleUpdateRead(0), delayR->popSampleUpdateRead(0));

					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, popped.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, popped.right);
				}


                BaseProcessingUnit<SampleType>::applyDryWetMix();

			}

			int getLatency() override
			{
				return !BaseProcessingUnit<SampleType>::isBypassed(); // 0 if bypassed, 1 smp otherwise
			}

			void initWidth(SampleType newWidth)
			{
				smoothedWidth.setCurrentAndTargetValue(newWidth);
			}
			void setWidth(SampleType newWidth)
			{
				smoothedWidth.setTargetValue(newWidth);
			}

			SampleType getWidth()
			{
				return smoothedWidth.getCurrentValue();
			}

			void inline updateSmoothedVariables() override
			{
				StereoPanner<SampleType>::updateSmoothedVariables();
				smoothedWidth.getNextValue();

				delayLVal = juce::jmax(SampleType(1), -smoothedWidth.getCurrentValue() * maxDelay);
				delayRVal = juce::jmax(SampleType(1), smoothedWidth.getCurrentValue() * maxDelay);

				delayL->snapDelay(delayLVal);
				delayR->snapDelay(delayRVal);

				delayL->updateSmoothedVariables();
				delayR->updateSmoothedVariables();

			}
			void setSmoothingTime(SampleType timeInSeconds) override
			{
				StereoPanner<SampleType>::setSmoothingTime(timeInSeconds);

				smoothedWidth.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
			}

		protected:
			std::unique_ptr<DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>> delayL, delayR;
			juce::SmoothedValue<SampleType> smoothedWidth;
			SampleType maxDelay, delayLVal, delayRVal;
		};

	}// namespace dsp
}// namnepace bdsp


