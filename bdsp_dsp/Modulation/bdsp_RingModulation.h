#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Processor that performs basic ring modulation.
		 * Can use input signal, sidechain input, or internal tone generator as the modulation signal.
		 */
		template <typename SampleType>
		class RingModulation : public BaseProcessingUnit<SampleType>
		{
		public:
			enum class RingModSource { None, Tone, Sidechain, Self };

			/**
			 * Creates an empty ring modulator.
			 * Initialized the tone generator to be unipolar.
			 */
			RingModulation(DSP_Universals<SampleType>* lookupsToUse)
				:tone(lookupsToUse)
			{
				tone.setBipolar(false);
			}

			/**
			 * Prepares for processing and initialized the modulation source to None.
			 * You need to set a source after this for the processor to do anything.
			 */
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				toneBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
				emptyBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
				for (int i = 0; i < spec.numChannels; ++i)
				{
					juce::FloatVectorOperations::fill(emptyBuffer.getWritePointer(i), SampleType(1), (size_t)spec.maximumBlockSize); // empty in the context of a ring modulation signal is a buffer of 1s
				}

				tone.prepare(spec);
				BaseProcessingUnit<SampleType>::prepare(spec);
				setModSource(RingModSource::None);
			}
			void reset() override
			{
				tone.reset();
				BaseProcessingUnit<SampleType>::reset();
			}

			void setModSource(RingModSource newSource)
			{
				source = newSource;
				switch (source)
				{
				case RingModSource::None:
					modulatorBlock = juce::dsp::AudioBlock<SampleType>(emptyBuffer);
					break;
				case RingModSource::Tone:
					modulatorBlock = juce::dsp::AudioBlock<SampleType>(toneBuffer);
					break;
				case RingModSource::Sidechain:
					if (sidechainBuffer == nullptr)
					{
						modulatorBlock = juce::dsp::AudioBlock<SampleType>(emptyBuffer);
					}
					else
					{
						modulatorBlock = juce::dsp::AudioBlock<SampleType>(*sidechainBuffer);
					}
					break;
				case RingModSource::Self:
					if (BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples() < 1) // this can be called before the internal processing blocks have been initialized
					{
						modulatorBlock = juce::dsp::AudioBlock<SampleType>(emptyBuffer);
					}
					else
					{
						modulatorBlock = BaseProcessingUnit<SampleType>::internalWetBlock;
					}
					break;

				}
			}

			/**
			 * Sets the buffer to use as the modulation source when set to Sidechain.
			 */
			void setSidechain(juce::AudioBuffer<SampleType>* newSidechainBuffer)
			{
				if (newSidechainBuffer->getNumChannels() < 1 || newSidechainBuffer->getNumSamples() < 1)
				{
					sidechainBuffer = &emptyBuffer;
				}
				else
				{
					sidechainBuffer = newSidechainBuffer;
				}

				if (source==RingModSource::Sidechain && newSidechainBuffer != sidechainBuffer)
				{
					setModSource(source); // if the buffer has changed we need to recreate the modulator block
				}

			}



			void processInternal(bool isBypassed) noexcept
			{
				if (!isBypassed)
				{
					if (source == RingModSource::Tone)
					{
						// Generate the tone in the modulator block
						juce::dsp::ProcessContextReplacing<SampleType> context(modulatorBlock);
						tone.process(context);
					}

					BaseProcessingUnit<SampleType>::internalWetBlock.replaceWithProductOf(BaseProcessingUnit<SampleType>::internalDryBlock, modulatorBlock); // apply ring modulation

					BaseProcessingUnit<SampleType>::applyDryWetMix();
				}
			}


			BasicShapesGenerator<SampleType>* getToneGenerator()
			{
				return &tone;
			}


		private:
			BasicShapesGenerator<SampleType> tone;
			juce::AudioBuffer<SampleType>* sidechainBuffer = nullptr;
			juce::AudioBuffer<SampleType> emptyBuffer;
			juce::AudioBuffer<SampleType> toneBuffer;

			juce::dsp::AudioBlock<SampleType> modulatorBlock;

			RingModSource source;
		};
	} // namespace dsp
} // namespace bdspnamespace dsp
