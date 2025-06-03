#pragma once

namespace bdsp
{
	namespace dsp
	{


		/**
		 * Captures samples from a stream of audio data and fills a buffer with the captured samples.
		 * Other classes with access to a sample source can then read from its buffer, even outside of the audio thread.
		 */
		template<typename SampleType>
		class SampleSource
		{
		public:

			/**
			 * Creates a sample source with a certain name
			 * @param sourceName
			 */
			SampleSource(const juce::String& sourceName)
				:name(sourceName)
			{

			}
			~SampleSource()
			{

			}

			void prepare(const juce::dsp::ProcessSpec& spec)
			{
				buffer.setSize(spec.numChannels, spec.maximumBlockSize);
				bufferPTR = 0;
                
                if(onSizeSet.operator bool())
                {
                    onSizeSet();
                }
			}




			/**
			 * Tracks a single sample and adds it to the internal buffer
			 * @param channel The channel of the buffer to ad the sample to
			 * @param smp The value of the sample to add
			 */
			void trackSample(const int& channel, const SampleType& smp)
			{
				buffer.setSample(channel, bufferPTR, smp);
				bufferPTR = (bufferPTR + 1) % buffer.getNumSamples();

				if (onBufferFilled.operator bool() && bufferPTR == 0)
				{
					onBufferFilled();
				}

			}


			/**
			 * Tracks an entire audio block and copies it into the internal buffer
			 * @tparam blockType The type of audio block provided - must be some form of juce::dsp::AudioBlock (can be float,double,const,non-const, etc.)
			 * @param input The audio block to track
			 */
			template <typename blockType>
			void trackBlock(blockType& input)
			{
				input.copyTo(buffer);
				if (onBufferFilled.operator bool())
				{
					onBufferFilled();
				}
			}

			/**
			 * Tracks another audio buffer and copies it into the internal buffer
			 * @param input The audio buffer to track
			 */
			void trackBuffer(const juce::AudioBuffer<SampleType>& input)
			{
				buffer = input;
				if (onBufferFilled.operator bool())
				{
					onBufferFilled();
				}
			}



			juce::String getName() const
			{
				return name;
			}


			const juce::AudioBuffer<SampleType>& getBuffer() const
			{
				return buffer;
			}

			std::function<void()> onBufferFilled, onSizeSet;
		protected:

			juce::AudioBuffer<SampleType> buffer;
			unsigned int bufferPTR = 0;
			juce::String name;

		private:
			JUCE_LEAK_DETECTOR(SampleSource)
				JUCE_DECLARE_WEAK_REFERENCEABLE(SampleSource)
		};

		template class SampleSource<float>;
		template class SampleSource<double>;


		/**
		 * Wrapper for a weak refernce to a sample source
		 * Used to ensure a valid buffer can always be read from even if this doesn't point to a valid sample source
		 */
		template<typename SampleType>
		struct SampleSourceWeakReference
		{

			/** Copies another pointer to this one. */
			SampleSourceWeakReference& operator= (SampleSource<SampleType>* newObject)
			{
				ref = newObject;
				return *this;
			}

			/**
			 * @return The samples source managed by the internal weak pointer
			 */
			SampleSource<SampleType>* get()
			{
				return ref.get();
			}


			/**
			 * Safe way to get the buffer from the referenced sample source.
			 * If the internal weak reference points to a valid sample source it simply returns that source's buffer. If it doesn't point to a valid sample source it returns an empty buffer of a specified size.
			 * @param expectedNumChannels The number of channels to make the empty buffer if necessary
			 * @param expectedNumSamples The number of samples to make the empty buffer if necessary
			 * @return 
			 */

			const juce::AudioBuffer<SampleType>& getBuffer(const int& expectedNumChannels, const int& expectedNumSamples)
			{
				if (get() == nullptr)
				{
					emptyBuffer.setSize(expectedNumChannels, expectedNumSamples, true, false, true);
					return emptyBuffer;
				}
				else
				{
					return ref.get()->getBuffer();
				}
			}

		private:
			juce::AudioBuffer<SampleType> emptyBuffer;
			juce::WeakReference<SampleSource<SampleType>> ref;

		};
		template <typename SampleType>
		using SampleSourceList = juce::OwnedArray <SampleSource<SampleType>>;

	} // namespace dsp

} // namespace bdsp
