#pragma once


#define BDSP_FLANGER_BASE_DELAY_MIN_MS 0.5
#define BDSP_FLANGER_BASE_DELAY_MAX_MS 10

#define BDSP_FLANGER_DELAY_CHANGE_MAX_MS 10

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class Flanger : public BaseProcessingUnit<SampleType>
		{
		public:
			Flanger(DSP_Universals<SampleType>* lookupToUse)
				:delayL(4, lookupToUse),
				delayR(4, lookupToUse)
			{
				lookup = lookupToUse;

				lookup->waveLookups;


			}
			~Flanger() = default;

			//==============================================================================
			/** Initialises the processor. */
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				const juce::dsp::ProcessSpec monoSpec = { spec.sampleRate,spec.maximumBlockSize,1 };
				delayL.prepare(monoSpec);
				delayR.prepare(monoSpec);

				maxDelay = (BDSP_FLANGER_BASE_DELAY_MAX_MS + BDSP_FLANGER_DELAY_CHANGE_MAX_MS) / 1000.0 * spec.sampleRate + 1;
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

				modPosL = 0;
				modPosR = 0;
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
				modPosL *= !!modInc; // if change amt is 0 reset otherwise modPos can get stuck at intermediate value

				float depth = delayChangeMax.getCurrentValue();
				modPosL = modf(modPosL + modInc, &tmp);
				auto modValL = lookup->waveLookups->lookupSin(0.5, modPosL, false, depth);
				delayL.snapDelay(BaseProcessingUnit<SampleType>::sampleRate / 1000 * (baseDelay.getCurrentValue() + modValL));

				modPosR = modf(modPosL + stereoSpread.getCurrentValue() * 0.5 + modInc, &tmp);
				auto modValR = lookup->waveLookups->lookupSin(0.5, modPosR, false, depth);
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

			void initDelayChangeMax(SampleType newBase)
			{
				delayChangeMax.setCurrentAndTargetValue(newBase);
			}
			void setDelayChangeMax(SampleType newBase)
			{
				delayChangeMax.setTargetValue(newBase);
			}


			void initDelayChangeRate(SampleType newBase)
			{
				delayChangeRate.setCurrentAndTargetValue(newBase);
			}
			void setDelayChangeRate(SampleType newBase)
			{
				delayChangeRate.setTargetValue(newBase);
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

			SampleType getModPos() const
			{
				return modPosL;
			}


			SampleType getCurrentModPosProportionally(bool left)
			{
				return (left ? delayL.getTargetDelay() : delayR.getTargetDelay()) / maxDelay;
			}

			juce::dsp::Complex<SampleType> calculateResponseForNormalizedFrequency(int channel, SampleType normalizedFrequency)
			{

				SampleType d = channel == 0 ? delayL.getDelay() : delayR.getDelay();
				//juce::dsp::Complex<SampleType> jw = std::exp(juce::dsp::Complex<SampleType>(0, -d * 2 * PI * normalizedFrequency);
				juce::dsp::Complex<SampleType> jw = std::polar(SampleType(1), SampleType(-d * 2 * PI * normalizedFrequency));


				return SampleType(1) / (SampleType(1) - (feedback.getCurrentValue() * jw));
			}

			juce::dsp::Complex<SampleType> calculateResponseForFrequency(int channel, SampleType frequency)
			{

				return calculateResponseForNormalizedFrequency(channel, frequency / BaseProcessingUnit<SampleType>::sampleRate);
			}


			juce::dsp::Complex<SampleType> approximateResponseForNormalizedFrequency(int channel, SampleType normalizedFrequency, SampleType maxBands)
			{

				SampleType d = channel == 0 ? delayL.getDelay() : delayR.getDelay();
				d = juce::jmap(d, SampleType(BDSP_FLANGER_BASE_DELAY_MIN_MS / 1000 * BaseProcessingUnit<SampleType>::sampleRate), SampleType(BDSP_FLANGER_BASE_DELAY_MAX_MS + BDSP_FLANGER_DELAY_CHANGE_MAX_MS) / 1000 * BaseProcessingUnit<SampleType>::sampleRate, SampleType(2), 2 * maxBands);
				//juce::dsp::Complex<SampleType> jw = std::exp(juce::dsp::Complex<SampleType>(0, -d * 2 * PI * normalizedFrequency);
				juce::dsp::Complex<SampleType> jw = std::polar(SampleType(1), SampleType(-d * 2 * PI * normalizedFrequency));


				return SampleType(1) / (SampleType(1) - (feedback.getCurrentValue() * jw));
			}

			juce::dsp::Complex<SampleType> approximateResponseForFrequency(int channel, SampleType frequency, SampleType maxBands)
			{

				return approximateResponseForNormalizedFrequency(channel, frequency / BaseProcessingUnit<SampleType>::sampleRate, maxBands);
			}



		protected:

			DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd> delayL, delayR;


			juce::SmoothedValue<SampleType> baseDelay, delayChangeMax, delayChangeRate;
			juce::SmoothedValue<SampleType> stereoSpread;

			juce::SmoothedValue<SampleType> feedback;
			SampleType modPosL = 0, modPosR = 0;

			DSP_Universals<SampleType>* lookup;


		private:
			SampleType maxDelay;
		};




	}// namespace dsp
}// namnepace bdsp


