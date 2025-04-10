#pragma once


#define BDSP_FLANGER_BASE_DELAY_MIN_MS 0.5
#define BDSP_FLANGER_BASE_DELAY_MAX_MS 10

#define BDSP_FLANGER_DELAY_CHANGE_MAX_MS 10

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Basic flanger unit. Has seperate delay lines for each stereo channel.
		 */
		template <typename SampleType>
		class Flanger : public BaseProcessingUnit<SampleType>
		{
		public:
			Flanger(DSP_Universals<SampleType>* lookupToUse)
				:delayL(4, lookupToUse),
				delayR(4, lookupToUse)
			{
				lookup = lookupToUse;

				lookup->waveLookups.operator->(); // creates the lookup object if necessary


			}
			~Flanger() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				const juce::dsp::ProcessSpec monoSpec = { spec.sampleRate,spec.maximumBlockSize,1 };
				delayL.prepare(monoSpec);
				delayR.prepare(monoSpec);

				maxDelay = (BDSP_FLANGER_BASE_DELAY_MAX_MS + BDSP_FLANGER_DELAY_CHANGE_MAX_MS) / 1000.0 * spec.sampleRate + 1; // max delay in samples
				delayL.setMaxDelay(maxDelay);
				delayR.setMaxDelay(maxDelay);


				BaseProcessingUnit<SampleType>::prepare(spec);

				reset();
			}


			void reset() override
			{
				delayL.reset();
				delayR.reset();

				BaseProcessingUnit<SampleType>::reset();

				modPhaseL = 0;
				modPhaseR = 0;
			}



			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				auto l = delayL.popSampleUpdateRead(0);
				auto r = delayR.popSampleUpdateRead(0);

				delayL.pushSample(0, inputSample.left + l * feedback.getCurrentValue());
				delayR.pushSample(0, inputSample.right + r * feedback.getCurrentValue());

				return StereoSample<SampleType>(l, r);
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
					auto smp = processSampleStereo(StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)));

					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, smp.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, smp.right);

				}

				BaseProcessingUnit<SampleType>::applyDryWetMix();
			}



			void updateSmoothedVariables() override
			{
				delayChangeRate.getNextValue();
				delayChangeMax.getNextValue();
				baseDelay.getNextValue();

				stereoSpread.getNextValue();



				feedback.getNextValue();

				float tmp;
				auto modInc = delayChangeRate.getCurrentValue() / (BaseProcessingUnit<SampleType>::sampleRate);
				modPhaseL *= bool(modInc); // if change amt is 0 reset otherwise modPhase can get stuck at intermediate value

				auto depth = delayChangeMax.getCurrentValue();
				modPhaseL = modf(modPhaseL + modInc, &tmp);
				modValL = lookup->waveLookups->lookupSin(0.5, modPhaseL, false, depth);
				delayL.snapDelay(BaseProcessingUnit<SampleType>::sampleRate / 1000 * (baseDelay.getCurrentValue() + modValL));

				modPhaseR = modf(modPhaseL + stereoSpread.getCurrentValue() / 2, &tmp);
				modValR = lookup->waveLookups->lookupSin(0.5, modPhaseR, false, depth);
				delayR.snapDelay(BaseProcessingUnit<SampleType>::sampleRate / 1000 * ((baseDelay.getCurrentValue() + modValR) + stereoSpread.getCurrentValue() * (BDSP_FLANGER_DELAY_CHANGE_MAX_MS - depth) / 2));


				delayL.updateSmoothedVariables();
				delayR.updateSmoothedVariables();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				delayL.setSmoothingTime(timeInSeconds);
				delayR.setSmoothingTime(timeInSeconds);

				baseDelay.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				delayChangeMax.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				delayChangeRate.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);

				stereoSpread.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);

			}


			void initBaseDelay(SampleType newBase)
			{
				baseDelay.setCurrentAndTargetValue(newBase);
			}
			void setBaseDelay(SampleType newBase)
			{
				baseDelay.setTargetValue(newBase);
			}

			/* Set the maximum change in delay time (depth) without smoothing */
			void initDelayChangeMax(SampleType newBase)
			{
				delayChangeMax.setCurrentAndTargetValue(newBase);
			}

			/* Set the maximum change in delay time (depth) with smoothing */
			void setDelayChangeMax(SampleType newBase)
			{
				delayChangeMax.setTargetValue(newBase);
			}


			/* Set new Rate in Hz without smoothing */
			void initDelayChangeRate(SampleType newRate)
			{
				delayChangeRate.setCurrentAndTargetValue(newRate);
			}

			/* Set new Rate in Hz with smoothing */
			void setDelayChangeRate(SampleType newRate)
			{
				delayChangeRate.setTargetValue(newRate);
			}

			void initStereoSpread(SampleType newBase)
			{
				stereoSpread.setCurrentAndTargetValue(newBase);
			}
			void setStereoSpread(SampleType newBase)
			{
				stereoSpread.setTargetValue(newBase);
			}





			void initFeedback(SampleType newBase)
			{
				feedback.setCurrentAndTargetValue(newBase);
			}
			void setFeedback(SampleType newBase)
			{
				feedback.setTargetValue(newBase);
			}

			juce::dsp::Complex<SampleType> calculateResponseForNormalizedFrequency(int channel, SampleType normalizedFrequency)
			{
				SampleType d = channel == 0 ? delayL.getDelay() : delayR.getDelay();
				juce::dsp::Complex<SampleType> jw = std::polar(SampleType(1), SampleType(-d * 2 * PI * normalizedFrequency));

				return SampleType(1) / (SampleType(1) - (feedback.getCurrentValue() * jw));
			}

			juce::dsp::Complex<SampleType> calculateResponseForFrequency(int channel, SampleType frequency)
			{
				return calculateResponseForNormalizedFrequency(channel, frequency / BaseProcessingUnit<SampleType>::sampleRate);
			}


			/**
			 * Returns the depth of a channel's flanging relative to the maximum depth possible
			 */
			float getModValue(int channel)
			{
				return (baseDelay.getCurrentValue() + (channel == 0 ? modValL : modValR)) / BDSP_FLANGER_DELAY_CHANGE_MAX_MS;
			}

		protected:

			DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd> delayL, delayR;


			juce::SmoothedValue<SampleType> baseDelay, delayChangeMax, delayChangeRate;
			juce::SmoothedValue<SampleType> stereoSpread;

			juce::SmoothedValue<SampleType> feedback;
			SampleType modPhaseL = 0, modPhaseR = 0;
			SampleType modValL = 0, modValR = 0;
			DSP_Universals<SampleType>* lookup;


		private:
			SampleType maxDelay;
		};




	}// namespace dsp
}// namnepace bdsp


