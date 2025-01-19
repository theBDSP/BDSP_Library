#pragma once

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Base class for all BDSP processors
		 */
		template <typename SampleType>
		class BaseProcessingUnit
		{
		public:
			BaseProcessingUnit()
			{
				initMix(1);
			}
			virtual ~BaseProcessingUnit() = default;

			/**
			 * Processes a block of audio from a processing context.
			 * You should never have to override or modify this function, instead override the processInternal function to process audio.
			 * @param context The processing context to process (either juce::dsp::ProcessContextReplacing or juce::dsp::ProcessContextNonReplacing)
			 */
			template<class ContextType>
			void process(const ContextType& context)
			{
				initProcessContext(context);

				if (inputSource != nullptr)
				{
					inputSource->trackBlock(context.getInputBlock());
				}
				processInternal(context.isBypassed || bypassed);


				if (outputSource != nullptr)
				{
					outputSource->trackBlock(context.getOutputBlock());
				}
			}


			/**
			 * Initializes the processor, almost certainly called from the main Audio Processor's prepareToPlay function
			 */
			virtual void prepare(const juce::dsp::ProcessSpec& spec)
			{

				sampleRate = spec.sampleRate;
				setSmoothingTime(smoothTime);


				if (linkedProcessor != nullptr)
				{
					linkedProcessor->prepare(spec);
				}

			}

			/**
			 * Resets the processor to its initial state
			 */
			virtual void reset()
			{
				if (linkedProcessor != nullptr)
				{
					linkedProcessor->reset();
				}
			}


			/**
			 * Processes a single sample of a single channel of audio
			 * @param channel The channel to process
			 * @param inputSample The value of the sample to process
			 * @return The processed sample
			 */
			virtual inline SampleType processSample(int channel, const SampleType& inputSample) noexcept
			{
				return inputSample;
			}


			/**
			 * Processes a single sample in the first channel
			 * @param inputSample The value of the sample to process
			 * @return The processed sample
			 */
			virtual inline SampleType processSample(const SampleType& inputSample) noexcept
			{
				return processSample(0, inputSample);
			}

			/**
			 * Processes a single stereo sample. Useful for processors that affect panning.
			 * @param inputSample The value of the sample to process
			 * @return The processed sample
			 */
			virtual inline StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				return smoothedDryMix.getCurrentValue() * inputSample + smoothedWetMix.getCurrentValue() * StereoSample<SampleType>(processSample(0, inputSample.left), processSample(1, inputSample.right));
			}


			/**
			 * Updates all of this processor's smoothed variables to the value for the next sample.
			 * Override this function to add any smoothed variables in you derived class.
			 */
			virtual inline void updateSmoothedVariables()
			{
				smoothedDryMix.getNextValue();
				smoothedWetMix.getNextValue();
				if (linkedProcessor != nullptr)
				{
					linkedProcessor->updateSmoothedVariables();
				}
			}


			/**
			 * Sets that amount of time this processor's smoothed variables take to reach new values after being set.
			 * Override this function to add any smoothed variables in you derived class.
			 * @param timeInSeconds
			 */
			virtual void setSmoothingTime(SampleType timeInSeconds)
			{
				smoothTime = timeInSeconds;
				smoothedDryMix.reset(sampleRate, smoothTime);
				smoothedWetMix.reset(sampleRate, smoothTime);

				if (linkedProcessor != nullptr)
				{
					linkedProcessor->setSmoothingTime(timeInSeconds);
				}

			}


			/**
			 * Converts timeInMs into seconds, then calls setSmoothingTime
			 * @param timeInMS  The smoothing time in milliseconds
			 */
			void setSmoothingTimeMS(SampleType timeInMS)
			{
				setSmoothingTime(timeInMS / 1000.0);
			}

			/**
			 * Converts timeInSamples into seconds, then calls setSmoothingTime
			 * @param timeInSamples  The smoothing time in number of samples
			 */
			void setSmoothingTimeSamples(int timeInSamples)
			{
				setSmoothingTime(timeInSamples / sampleRate);
			}


			/**
			 * Sets the value the dry mix will get smoothed to
			 */
			void setDryMix(SampleType newValue)
			{
				smoothedDryMix.setTargetValue(newValue);
				if (linkedProcessor != nullptr && linkedProcessorCopiesMix)
				{
					linkedProcessor->setDryMix(newValue);
				}
			}


			/**
			 * Sets the dry mix value immediately (no smoothing)
			 */
			void initDryMix(SampleType newValue)
			{
				smoothedDryMix.setCurrentAndTargetValue(newValue);
				if (linkedProcessor != nullptr && linkedProcessorCopiesMix)
				{
					linkedProcessor->initDryMix(newValue);
				}
			}

			/**
			 * Sets the value the wet mix will get smoothed to
			 */
			void setWetMix(SampleType newValue)
			{
				smoothedWetMix.setTargetValue(newValue);
				if (linkedProcessor != nullptr && linkedProcessorCopiesMix)
				{
					linkedProcessor->setWetMix(newValue);
				}
			}


			/**
			 * Sets the wet mix value immediately (no smoothing)
			 */
			void initWetMix(SampleType newValue)
			{
				smoothedWetMix.setCurrentAndTargetValue(newValue);
				if (linkedProcessor != nullptr && linkedProcessorCopiesMix)
				{
					linkedProcessor->initWetMix(newValue);
				}
			}


			/**
			 * Sets the values the dry and wet mix will get smoothed to
			 * @param newValue The value the wet mix will get smoothed to, dry mix will get smoothed to 1-this
			 */
			void setMix(SampleType newValue)
			{
				setWetMix(newValue);
				setDryMix(SampleType(1) - newValue);
			}

			/**
			 * Sets the dry and wet mix values immediately
			 * @param newValue The value to set the wet mix to, dry mix will get set to 1-this
			 */
			void initMix(SampleType newValue)
			{
				initWetMix(newValue);
				initDryMix(SampleType(1) - newValue);
			}

			SampleType getDryMix()
			{
				return smoothedDryMix.getCurrentValue();
			}

			SampleType getWetMix()
			{
				return smoothedWetMix.getCurrentValue();
			}


			/**
			 * Applies the current values for dry mix and wet mix to the provided samples, sums the results together, and returns the sum
			 */
			SampleType applyDryWetMix(SampleType drySample, SampleType wetSample)
			{
				return getDryMix() * drySample + getWetMix() * wetSample;
			}

			/**
			 * Mixes the internal dry and wet blocks and replaces the outBlock with the result
			 */
			void applyDryWetMix()
			{
				applyDryWetMix(internalDryBlock, internalWetBlock);
			}

			/**
			 * Mixes the provided dry and wet buffers and replaces the provided wet buffer with the result
			 */
			void applyDryWetMix(juce::AudioBuffer<SampleType> dryBuffer, juce::AudioBuffer<SampleType>& wetBuffer)
			{
				smoothedDryMix.applyGain(dryBuffer, dryBuffer.getNumSamples());
				smoothedWetMix.applyGain(wetBuffer, wetBuffer.getNumSamples());

				for (int i = 0; i < dryBuffer.getNumChannels(); ++i)
				{
					wetBuffer.addFrom(i, 0, dryBuffer, i, 0, dryBuffer.getNumSamples());
				}
			}

			/**
			 * Mixes the provided dry buffer and wet block and replaces the provided wet block with the result
			 */
			void applyDryWetMix(juce::AudioBuffer<SampleType> dryBuffer, juce::dsp::AudioBlock<SampleType>& wetBlock)
			{
				smoothedDryMix.applyGadry(dryBuffer, dryBuffer.getNumSamples());

				for (int i = 0; i < dryBuffer.getNumChannels(); ++i)
				{
					smoothedWetMix.applyGadry(wetBlock.getChannelPointer(i), wetBlock.getNumSamples());
				}
				juce::dsp::AudioBlock<SampleType> tmpBuffer(dryBuffer);
				wetBlock.replaceWithSumOf(wetBlock, tmpBuffer);
			}



			/**
			 * Mixes the provided dry and wet block and replaces the provided wet block with the result
			 */
			void applyDryWetMix(const juce::dsp::AudioBlock<const SampleType>& dryBlock, juce::dsp::AudioBlock<SampleType>& wetBlock)
			{
				juce::AudioBuffer<SampleType> dryBuffer;
				dryBuffer.setSize(dryBlock.getNumChannels(), dryBlock.getNumSamples());

				for (int i = 0; i < dryBuffer.getNumChannels(); ++i)
				{
					dryBuffer.copyFrom(i, 0, dryBlock.getChannelPointer(i), dryBlock.getNumSamples());
				}

				juce::dsp::AudioBlock<SampleType> dryCopy(dryBuffer);
				applyDryWetMix(dryCopy, wetBlock);
			}


			/**
			 * Mixes the provided dry buffer and wet block and replaces the provided wet block with the result
			 */
			void applyDryWetMix(juce::dsp::AudioBlock<SampleType>& dryBlock, juce::dsp::AudioBlock<SampleType>& wetBlock) // replaces the wetBlock with the mixed dryput and wet block
			{
				auto startDry = smoothedDryMix;
				auto startWet = smoothedWetMix;

				for (int i = 0; i < dryBlock.getNumChannels(); ++i)
				{
					smoothedDryMix = startDry;
					smoothedWetMix = startWet;
					smoothedDryMix.applyGain(dryBlock.getChannelPointer(i), dryBlock.getNumSamples());
					smoothedWetMix.applyGain(wetBlock.getChannelPointer(i), dryBlock.getNumSamples());

				}
				wetBlock.replaceWithSumOf(wetBlock, dryBlock);
			}

			void setBypassed(bool state)
			{
				bypassed = state;
			}

			bool isBypassed()
			{
				return bypassed;
			}

			double getSampleRate()
			{
				return sampleRate;
			}


			/**
			 * @return Latency of processing in samples
			 */
			virtual int getLatency()
			{
				return 0;
			}

			/**
			 * @return  Maximum possible latency of processing in samples
			 */
			virtual int getMaxLatency()
			{
				return getLatency();
			}


			void setInputSource(SampleSource<SampleType>* newSource)
			{
				inputSource = newSource;
			}
			void setOutputSource(SampleSource<SampleType>* newSource)
			{
				outputSource = newSource;
			}


			BaseProcessingUnit<SampleType>* linkedProcessor = nullptr; // automatically update this processors parameters to match this one's
		protected:
			bool bypassed = false;

			SampleType sampleRate = 44100;
			SampleType smoothTime = 0.125;
			juce::SmoothedValue<SampleType> smoothedDryMix, smoothedWetMix;

			juce::dsp::AudioBlock<const SampleType> internalDryBlock;
			juce::dsp::AudioBlock<SampleType> internalWetBlock;
			juce::AudioBuffer<SampleType> inputCopy; // used to create internalDryBlock when the context provided uses a single io block (juce::ProcessContextReplacing)
			std::unique_ptr<juce::dsp::ProcessContextNonReplacing<SampleType>> internalContext;

			SampleSource<SampleType>* inputSource = nullptr, * outputSource = nullptr; // pointer to sample sources you want to tack samples from either the input or output of this processor


			/**
			 * Override this in derived classes to perform block processing. Use internal dry and wet blocks loaded in base class's process call.
			 * @param isBypassed True if processing should be bypassed
			 */
			virtual void processInternal(bool isBypassed) noexcept = 0;



			bool linkedProcessorCopiesMix = false; // set true if you want linked processor to auto copy this processor's mix parameter values (might not for things like 2nd order filters)

		private:


			/**
			 * Loads the internal dry and wet blocks from the provided processing context
			 * @param context The processing context to load from (either juce::dsp::ProcessContextReplacing or juce::dsp::ProcessContextNonReplacing)
			 */
			template <class ContextType>
			void initProcessContext(ContextType& context)
			{
				if (context.usesSeparateInputAndOutputBlocks())
				{
					internalDryBlock = context.getInputBlock();
				}
				else
				{
					auto& input = context.getInputBlock();
					inputCopy.setSize(input.getNumChannels(), input.getNumSamples());

					for (int i = 0; i < input.getNumChannels(); ++i)
					{
						inputCopy.copyFrom(i, 0, input.getChannelPointer(i), input.getNumSamples());
					}
					internalDryBlock = juce::dsp::AudioBlock<SampleType>(inputCopy);
				}

				internalWetBlock = context.getOutputBlock();

				internalContext.reset(new juce::dsp::ProcessContextNonReplacing<SampleType>(internalDryBlock, internalWetBlock));
			}


			JUCE_LEAK_DETECTOR(BaseProcessingUnit)
		};


		/**
		 * A processor that does nothing
		 */
		template <typename SampleType>
		class EmptyProcessor : public BaseProcessingUnit<SampleType>
		{
		public:
			void processInternal(bool isBypassed) noexcept override
			{

			}
		};


		/**
		 * Wraps a juce:dsp processing class into a BDSP processing context
		 * @tparam coreClass The juce::dsp processing class to wrap
		 */
		template <typename SampleType, class coreClass>
		class BaseProcessingUnitWrapper : public coreClass, public BaseProcessingUnit<SampleType>
		{
		public:
			template <class ... Types>
			BaseProcessingUnitWrapper(Types...args)
				:coreClass(args...)
			{

			}
			~BaseProcessingUnitWrapper() = default;

			void reset() override
			{
				coreClass::reset();
			}

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);

				coreClass::prepare(spec);
			}


			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return !!BaseProcessingUnit<SampleType>::bypassed * inputSample + !BaseProcessingUnit<SampleType>::bypassed * coreClass::processSample(channel, inputSample);
			}

			inline SampleType processSample(const SampleType inputSample) noexcept override
			{
				return !!BaseProcessingUnit<SampleType>::bypassed * inputSample + !BaseProcessingUnit<SampleType>::bypassed * coreClass::processSample(0, inputSample);
			}

			void processInternal(bool isBypassed) noexcept override
			{
				coreClass::process(BaseProcessingUnit<SampleType>::internalContext);
			}

		};





	}// namespace dsp
}// namnepace bdsp


