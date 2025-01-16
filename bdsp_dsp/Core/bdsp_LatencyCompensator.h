#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * Delays an audio signal to compensate for latency elsewhere in the signal chain.
		 * Only handles integer latency (no sub-sample latency compensation).
		 */
		template <typename SampleType>
		class LatencyCompensator
		{
		public:
			LatencyCompensator()
			{
			}
			~LatencyCompensator() = default;



			/**
			 * @param latencyInSamples The total latency this needs to compensate for
			 */
			void setLatency(const unsigned int& latencyInSamples)
			{
				totalSize = juce::nextPowerOfTwo(latencyInSamples + 1);
				delay = latencyInSamples;
				reset();
			}

			int getLatency()
			{
				return delay;
			}


			void prepare(const juce::dsp::ProcessSpec& spec)
			{
				jassert(spec.numChannels > 0);


				delayBuffer.setSize((int)spec.numChannels, totalSize, false, false, true);


				reset();
			}

			void reset()
			{
				writePos = 0;
				readPos = 0;

				delayBuffer.clear();

			}


			/**
			 * Mixes a dry and wet signal while delaying the dry signal to compensate for latency in the wet signal
			 * @param context The processing context to perform the mixing on, either juce::dsp::ProcessContextReplacing or juce::dsp::ProcessContextNonReplacing
			 * @param dryGain The smoothed value controlling the dry gain
			 * @param wetGain The smoothed value controlling the wet gain
			 * @param bypassed If true, the mixing will be bypassed, but the input signal will still be delayed - defaults to false
			 */
			template<typename ProcessContext>
			void mixWithLatencyCompensation(const ProcessContext& context, juce::SmoothedValue<SampleType>& dryGain, juce::SmoothedValue<SampleType>& wetGain, bool bypassed = false)
			{
				const auto& inputBlock = context.getInputBlock();
				auto& outputBlock = context.getOutputBlock();

				//================================================================================================================================================================================================
				// create a non-const copy of the input block to write the delayed signal to
				juce::AudioBuffer<SampleType> buff;
				buff.setSize(inputBlock.getNumChannels(), inputBlock.getNumSamples());
				juce::dsp::AudioBlock<SampleType> nonConstInputBlock(buff);
				nonConstInputBlock.copyFrom(inputBlock);
				//================================================================================================================================================================================================

				if (handlesLatencyCompensation)
				{
					for (size_t i = 0; i < inputBlock.getNumSamples(); ++i)
					{
						processSample(i, inputBlock, nonConstInputBlock);
					}
				}


				if (bypassed)
				{
					outputBlock.copyFrom(nonConstInputBlock);
					return;
				}
				//================================================================================================================================================================================================
				// apply mixing
				nonConstInputBlock.multiplyBy(dryGain);
				outputBlock.multiplyBy(wetGain);

				outputBlock.replaceWithSumOf(nonConstInputBlock, outputBlock);
				//================================================================================================================================================================================================
			}


			bool handlesLatencyCompensation = true; // whether to delay input signal to compensate for latency when mixing
		private:


			/**
			 * Processes a single sample for each channel and updates the read and write pointers
			 * @tparam readBlockType The type of audio block provided for the read block - must be some form of juce::dsp::AudioBlock (can be float,double,const,non-const, etc.)
			 * @tparam writeBlockType The type of audio block provided for the write block - must be some form of juce::dsp::AudioBlock (can be float,double, must be non-const)
			 * @param smpNum The index of the sample to read from and write to the provided blocks
			 * @param blockToReadFrom The Audio Block to read the samples from
			 * @param blockToWriteTo The Audio Block to write the samples to
			 */
			template<class readBlockType, class writeBlockType>
			void processSample(const int& smpNum, const readBlockType& blockToReadFrom, writeBlockType& blockToWriteTo)
			{
				jassert(blockToReadFrom.getNumChannels() == blockToWriteTo.getNumChannels());

				unsigned int idx = (readPos + delay) & (totalSize - 1); // get delayed read index
				for (size_t i = 0; i < blockToReadFrom.getNumChannels(); ++i) // loop through each channel
				{
					blockToWriteTo.setSample(i, smpNum, delayBuffer.getSample(i, idx)); // write the current delayed samples to the write block
					delayBuffer.setSample(i, writePos, blockToReadFrom.getSample(i, smpNum)); // write the current sample from the read block to the delay buffer
				}

				// increment the read and write pointers
				readPos = (readPos + totalSize - 1) & (totalSize - 1);
				writePos = (writePos + totalSize - 1) & (totalSize - 1);

			}


			juce::AudioBuffer<SampleType> delayBuffer;
			unsigned int writePos, readPos;

			unsigned int delay = 1;
			unsigned int totalSize = 0;

			JUCE_LEAK_DETECTOR(LatencyCompensator)
		};

	} //namespace dsp
} //namespace bdsp