#pragma once


namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType>
		class DynamicsBase : public BaseProcessingUnit<SampleType>
		{
		public:
			DynamicsBase() = default;
			virtual ~DynamicsBase() = default;

			void setAttack(SampleType attackTimeMs)
			{
				envFilter.setAttackTime(attackTimeMs);
			}



			void setRelease(SampleType releaseTimeMs)
			{
				envFilter.setReleaseTime(releaseTimeMs);
			}


			void setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType newCalculationType)
			{
				envFilter.setLevelCalculationType(newCalculationType);
			}

			//==============================================================================
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);

				postGain.prepare(spec);

				juce::dsp::ProcessSpec monoSpec{ spec.sampleRate,spec.maximumBlockSize, 1 };
				envFilter.prepare(monoSpec);

				envGain.prepare(monoSpec);

				reset();
			}

			/** Resets the internal state variables of the filter. */
			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();

				envGain.reset();
				postGain.reset();
				envFilter.reset();
			}


			void setSideChain(SampleSource<SampleType>* newSideChain)
			{
				sideChain = newSideChain;
			}



			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SampleType();
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


				const auto& sideChainBuffer = sideChain.getBuffer(numChannels,numSamples);

				for (size_t i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					if (sideChain.get() == nullptr)
					{
						envValue = envFilter.processSample(0, envGain.processSample(0, (BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i) + BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)) / 2));
					}
					else
					{
						envValue = envFilter.processSample(0, envGain.processSample(0, (sideChainBuffer.getSample(0, i) + sideChainBuffer.getSample(1, i)) / 2));
					}


					auto smp = postGain.processSampleStereo(applyDynamics(StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)), envValue)); // calculate each sample
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, smp.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, smp.right);

				}
                BaseProcessingUnit<SampleType>::applyDryWetMix();
			}



			inline void updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
				envGain.updateSmoothedVariables();
				postGain.updateSmoothedVariables();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
				envGain.setSmoothingTime(timeInSeconds);
				postGain.setSmoothingTime(timeInSeconds);
			}
			virtual StereoSample<SampleType> applyDynamics(const StereoSample<SampleType>& inputSample, const SampleType& envValue) = 0;



			void setEnvGain(SampleType newValue) // gain of signal driving the compression  (not the input signal)
			{
				envGain.setGain(newValue);
			}

			void initEnvGain(SampleType initValue)
			{
				envGain.initGain(initValue);
			}

			void setPostGain(SampleType newValue) // gain of output signal
			{
				postGain.setGain(newValue);
			}

			void initPostGain(SampleType initValue)
			{
				postGain.initGain(initValue);
			}

			SampleType getEnvValue()
			{
				return envValue;
			}

		protected:
			juce::dsp::BallisticsFilter<SampleType> envFilter;
			SampleSourceWeakReference<SampleType> sideChain;


			Gain<SampleType> envGain, postGain;

			SampleType envValue;


		};


	} // namespace dsp
} // namespace bdsp
