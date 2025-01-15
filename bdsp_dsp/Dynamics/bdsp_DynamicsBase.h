#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * Base class for all processors that do basic dynamics processing.
		 * This class manages a ballistic filter that calculates the signal's amplitude envelope.
		 * Derived classes overwrite the applyDynamics function to modify the input signal based on the calculated envelope value.
		 * Currently enevelope calculation can only be done on the mono-summed input signal, I plan to add more options in the future.
		 */
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


			/* Sets the envelope calculation to either peak or RMS*/
			void setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType newCalculationType)
			{
				envFilter.setLevelCalculationType(newCalculationType);
			}

			//==============================================================================
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);

				postGain.prepare(spec);

				juce::dsp::ProcessSpec monoSpec{ spec.sampleRate,spec.maximumBlockSize, 1 }; // envelope calculations are done on the mono-summed input signal
				envFilter.prepare(monoSpec);

				envGain.prepare(monoSpec);

				reset();
			}

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



			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);


				const auto& sideChainBuffer = sideChain.getBuffer(numChannels, numSamples);

				for (size_t i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					//================================================================================================================================================================================================
					// calculate envelope
					if (sideChain.get() == nullptr)
					{
						envValue = envFilter.processSample(0, envGain.processSample(0, (BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i) + BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)) / 2));
					}
					else
					{
						envValue = envFilter.processSample(0, envGain.processSample(0, (sideChainBuffer.getSample(0, i) + sideChainBuffer.getSample(1, i)) / 2));
					}
					//================================================================================================================================================================================================


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


			/**
			 * Set the gain of signal driving the envelope calculation with smoothing
			 */
			void setEnvGain(SampleType newValue) 
			{
				envGain.setGain(newValue);
			}


			/**
			 * Set the gain of signal driving the envelope calculation without smoothing
			 */
			void initEnvGain(SampleType initValue)
			{
				envGain.initGain(initValue);
			}


			/**
			 * Set the gain of resulting output signal with smoothing
			 */
			void setPostGain(SampleType newValue)
			{
				postGain.setGain(newValue);
			}

			/**
			 * Set the gain of resulting output signal without smoothing
			 */
			void initPostGain(SampleType initValue)
			{
				postGain.initGain(initValue);
			}

			/**
			 * @return The current value of the amplitude envelope
			 */
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
