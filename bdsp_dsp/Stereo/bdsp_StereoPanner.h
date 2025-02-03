#pragma once


namespace bdsp
{
	namespace dsp
	{
		/**
		 * Panner that feeds both channels into both channels instead of just adjusting gain of each channel.
		 * This assures output given an input regardless of panning
		 */
		template <typename SampleType>
		class StereoPanner : public Gain<SampleType>
		{
		public:
			StereoPanner(DSP_Universals<SampleType>* lookupToUse)
			{
				lookup = lookupToUse;

				lookup->panningLookups.operator->(); // load lookup tables if necessary
			}
			~StereoPanner() = default;


			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				Gain<SampleType>::prepare(spec);

				smoothedPan.setCurrentAndTargetValue(0.0);
			}


			/**
			 * Attempts to maintain consistent output power as the panning value changes
			 * https://www.desmos.com/calculator/w1n0vupv8g
			 */
			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				SampleType pan = smoothedPan.getCurrentValue();

				left = (inputSample.left * lookup->panningLookups->getPanLL(pan) + inputSample.right * lookup->panningLookups->getPanRL(pan));
				right = (inputSample.left * lookup->panningLookups->getPanLR(pan) + inputSample.right * lookup->panningLookups->getPanRR(pan));

				return Gain<SampleType>::processSampleStereo(StereoSample<SampleType>(left, right));
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

				for (size_t i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					auto smp = processSampleStereo(StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalWetBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalWetBlock.getSample(1, i))); // calculate each sample
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, smp.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, smp.right);
				}
				BaseProcessingUnit<SampleType>::applyDryWetMix();
			}

			void updateSmoothedVariables() override
			{
				Gain<SampleType>::updateSmoothedVariables();
				smoothedPan.getNextValue();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				Gain<SampleType>::setSmoothingTime(timeInSeconds);
				smoothedPan.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}

			void setPan(SampleType newValue)
			{
				jassert(abs(newValue) <= 1);
				smoothedPan.setTargetValue(newValue);
			}

			void initPan(SampleType initValue)
			{
				jassert(abs(initValue) <= 1);
				smoothedPan.setCurrentAndTargetValue(initValue);
			}

			SampleType getPan()
			{
				return smoothedPan.getCurrentValue();
			}



		protected:
			juce::SmoothedValue<SampleType> smoothedPan;

			SampleType panL, panR, left, right;

			DSP_Universals<SampleType>* lookup;
		};
	}// namespace dsp
}// namnepace bdsp


