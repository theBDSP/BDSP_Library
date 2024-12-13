#pragma once
#define BDSP_STEREO_WIDTH_MAX_DELAY_MS 30

namespace bdsp
{
	namespace dsp
	{

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

				delayM = std::make_unique< DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>>(maxDelay, StereoPanner<SampleType>::lookup);
				delayS = std::make_unique< DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>>(maxDelay, StereoPanner<SampleType>::lookup);
				delayM->prepare(monoSpec);
				delayS->prepare(monoSpec);
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
					delayM->pushSample(0, panned.left);
					delayS->pushSample(0, panned.right);

					auto popped = StereoSample<SampleType>(delayM->popSampleUpdateRead(0), delayS->popSampleUpdateRead(0));

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

				delayMVal = juce::jmax(SampleType(1), -smoothedWidth.getCurrentValue() * maxDelay);
				delaySVal = juce::jmax(SampleType(1), smoothedWidth.getCurrentValue() * maxDelay);


				delayM->snapDelay(delayMVal);
				delayS->snapDelay(delaySVal);


				delayM->updateSmoothedVariables();
				delayS->updateSmoothedVariables();

			}
			void setSmoothingTime(SampleType timeInSeconds)
			{
				StereoPanner<SampleType>::setSmoothingTime(timeInSeconds);

				smoothedWidth.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
			}

		protected:
			std::unique_ptr<DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>> delayM, delayS;
			juce::SmoothedValue<SampleType> smoothedWidth;
			SampleType maxDelay, delayMVal, delaySVal;
		};






	}// namespace dsp
}// namnepace bdsp


