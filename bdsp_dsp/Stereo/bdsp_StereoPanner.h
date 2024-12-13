#pragma once


namespace bdsp
{
	namespace dsp
	{

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
				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);


				BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
				BaseProcessingUnit<SampleType>::internalWetBlock.multiplyBy(smoothedGain);

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


		// panner that feeds both channels to both channels instead just adjusting gain of each channel
		// this assures output given an input regardless of panning
		template <typename SampleType>
		class StereoPanner : public Gain<SampleType>
		{
		public:
			StereoPanner(DSP_Universals<SampleType>* lookupToUse)
			{
				lookup = lookupToUse;

				lookup->panningLookups;


			}
			~StereoPanner() = default;

			//==============================================================================
			/** Initialises the processor. */
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				Gain<SampleType>::prepare(spec);

				smoothedPan.setCurrentAndTargetValue(0.0);

			}


			// https://www.desmos.com/calculator/txlxe2lse0
			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				auto postGain = Gain<SampleType>::processSampleStereo(inputSample);

				SampleType pan = juce::jlimit(SampleType(-1), SampleType(1), smoothedPan.getCurrentValue());

				left = (postGain.left * lookup->panningLookups->getPanLL(pan) + postGain.right * lookup->panningLookups->getPanRL(pan));
				right = (postGain.left * lookup->panningLookups->getPanLR(pan) + postGain.right * lookup->panningLookups->getPanRR(pan));



				return StereoSample<SampleType>(left, right);


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


				BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
				BaseProcessingUnit<SampleType>::smoothedWetMix.applyGain(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(0), numSamples);
				BaseProcessingUnit<SampleType>::smoothedWetMix.applyGain(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(1), numSamples);

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


