#pragma once


#define BDSP_CHORUS_DELAY_MIN_MS 10
#define BDSP_CHORUS_DELAY_MAX_MS 20


#define BDSP_CHORUS_DEPTH_MAX_MS (BDSP_CHORUS_DELAY_MAX_MS- BDSP_CHORUS_DELAY_MIN_MS) * 0.5

#define BDSP_CHORUS_BASE_DELAY_MS (BDSP_CHORUS_DELAY_MAX_MS + BDSP_CHORUS_DELAY_MIN_MS) * 0.5



namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class Chorus : public BaseProcessingUnit<SampleType>
		{
		public:
			Chorus(DSP_Universals<SampleType>* lookupToUse)
				:modPhases(lookupToUse->chorusLookups->getChorusPhases()),
				modMults(lookupToUse->chorusLookups->getChorusVelocityMults())
			{
				lookup = lookupToUse;

				lookup->waveLookups;



				for (int i = 0; i < BDSP_CHORUS_MAX_VOICES; ++i)
				{
					delayL.add(new DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>(4, lookup));
					delayR.add(new DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>(4, lookup));
				}

			}
			~Chorus() = default;

			//==============================================================================
			/** Initialises the processor. */
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				const juce::dsp::ProcessSpec monoSpec = { spec.sampleRate,spec.maximumBlockSize,1 };
				maxDelay = (BDSP_CHORUS_DELAY_MAX_MS) / 1000.0 * spec.sampleRate;
				for (auto d : delayL)
				{
					d->prepare(monoSpec);
					d->setMaxDelay(maxDelay);
				}

				for (auto d : delayR)
				{
					d->prepare(monoSpec);
					d->setMaxDelay(maxDelay);
				}

				modPosL.resize(delayL.size());




				inputDelay.prepare(spec);
				auto inputDelaySmps = BDSP_CHORUS_BASE_DELAY_MS / 1000.0 * spec.sampleRate;
				inputDelay.setMaxDelay(inputDelaySmps);
				inputDelay.setDelay(inputDelaySmps);

				BaseProcessingUnit<SampleType>::prepare(spec);

				reset();
			}


			void reset() override
			{
				for (auto d : delayL)
				{
					d->reset();
				}

				BaseProcessingUnit<SampleType>::reset();

				for (int i = 0; i < BDSP_CHORUS_MAX_VOICES; ++i)
				{
					modPosL.set(i, modPhases.getUnchecked(i));
				}
			}

			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				auto gain = (depth.getCurrentValue() / pow(numVoices, 0.75) + (1 - depth.getCurrentValue()) / numVoices);

				SampleType l = 0;
				SampleType r = 0;
				for (int i = 0; i < delayL.size(); ++i)
				{
					bool inSum = i < numVoices;
					auto currL = delayL.getUnchecked(i);
					auto currR = delayR.getUnchecked(i);


					l += inSum * currL->popSampleUpdateRead(0);
					r += inSum * currR->popSampleUpdateRead(0);




					currL->pushSample(0, inputSample.left);
					currR->pushSample(0, inputSample.right);
				}


				l *= gain;
				r *= gain;

				//================================================================================================================================================================================================


				auto inL = inputDelay.popSampleUpdateRead(0);
				auto inR = inputDelay.popSampleUpdateRead(1);

				inputDelay.pushSample(0, inputSample.left);
				inputDelay.pushSample(1, inputSample.right);


				//================================================================================================================================================================================================



				return BaseProcessingUnit<SampleType>::smoothedDryMix.getCurrentValue() * StereoSample<SampleType>(inL, inR) + BaseProcessingUnit<SampleType>::smoothedWetMix.getCurrentValue() * StereoSample<SampleType>(l, r);
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

			}



			void updateSmoothedVariables() override
			{


				delayChangeRate.getNextValue();
				depth.getNextValue();

				stereoWidth.getNextValue();

				float tmp;
				auto modInc = delayChangeRate.getCurrentValue() / (BaseProcessingUnit<SampleType>::sampleRate);

				for (int i = 0; i < BDSP_CHORUS_MAX_VOICES; ++i)
				{
					auto pos = modf(modPosL.getUnchecked(i) + modInc * modMults.getUnchecked(i), &tmp);
					modPosL.set(i, pos);

					auto modValL = lookup->waveLookups->getLFOValue(0, 0.5, pos, true, depth.getCurrentValue());
					delayL.getUnchecked(i)->snapDelay(BaseProcessingUnit<SampleType>::sampleRate / 1000 * (BDSP_CHORUS_BASE_DELAY_MS + modValL));

					//================================================================================================================================================================================================


					pos = modf(pos + stereoWidth.getCurrentValue() / 4, &tmp);
					auto modValR = lookup->waveLookups->getLFOValue(0, 0.5, pos, true, depth.getCurrentValue());
					delayR.getUnchecked(i)->snapDelay(BaseProcessingUnit<SampleType>::sampleRate / 1000 * (BDSP_CHORUS_BASE_DELAY_MS + modValR));


					delayL.getUnchecked(i)->updateSmoothedVariables();
					delayR.getUnchecked(i)->updateSmoothedVariables();
				}
				//================================================================================================================================================================================================


				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				for (auto d : delayL)
				{
					d->setSmoothingTime(timeInSeconds);
				}


				for (auto d : delayR)
				{
					d->setSmoothingTime(timeInSeconds);
				}




				delayChangeRate.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				depth.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);

				stereoWidth.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);

			}


			// new Rate in Hz
			void initDelayChangeRate(SampleType newRate)
			{
				delayChangeRate.setCurrentAndTargetValue(newRate);
			}

			// new Rate in Hz
			void setDelayChangeRate(SampleType newRate)
			{
				delayChangeRate.setTargetValue(newRate);
			}

			void initDepth(SampleType newBase)
			{
				depth.setCurrentAndTargetValue(newBase);
			}
			void setDepth(SampleType newBase)
			{
				depth.setTargetValue(newBase);
			}


			void initStereoWidth(SampleType newBase)
			{
				stereoWidth.setCurrentAndTargetValue(newBase);
			}
			void setStereoWidth(SampleType newBase)
			{
				stereoWidth.setTargetValue(newBase);
			}


			void setNumVoices(int newnumVoices)
			{
				numVoices = newnumVoices;
			}



			SampleType getModPos(int voice)
			{
				return modPosL.getUnchecked(voice);
			}

			SampleType getCurrentModPosProportionally(int voice, bool left)
			{
				auto msFactor = BaseProcessingUnit<SampleType>::sampleRate / 1000;

				return juce::jmap((left ? delayL.getUnchecked(voice)->getTargetDelay() : delayR.getUnchecked(voice)->getTargetDelay()), SampleType(BDSP_CHORUS_DELAY_MIN_MS * msFactor), SampleType(BDSP_CHORUS_DELAY_MAX_MS * msFactor), SampleType(-1), SampleType(1));
			}

		protected:

			juce::OwnedArray<DelayLine<SampleType, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>> delayL, delayR;


			DelayLineBase<SampleType> inputDelay;

			juce::SmoothedValue<SampleType>  delayChangeRate, depth;
			juce::SmoothedValue<SampleType> stereoWidth;

			int numVoices = 4;

			juce::Array<SampleType> modPosL;

			DSP_Universals<SampleType>* lookup;




			juce::Array<SampleType>& modMults, & modPhases;


		private:
			SampleType maxDelay;
		};




	}// namespace dsp
}// namnepace bdsp


