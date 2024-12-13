#pragma once

namespace bdsp
{
	namespace dsp
	{
		template <typename SampleType>
		class RingModulation : public MultiSignalProcessorBase<SampleType>
		{
		public:

			RingModulation(DSP_Universals<SampleType>* lookupsToUse)
				:MultiSignalProcessorBase<SampleType>(1),
				tone(lookupsToUse)
			{

			}

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				emptySidechain.setSize(spec.numChannels, spec.maximumBlockSize);
				for (int i = 0; i < spec.numChannels; ++i)
				{
					juce::FloatVectorOperations::fill(emptySidechain.getWritePointer(i), SampleType(1), (size_t)spec.maximumBlockSize);
				}

				tone.prepare(spec);
				MultiSignalProcessorBase<SampleType>::prepare(spec);
			}
			void reset() override
			{
				tone.reset();
				MultiSignalProcessorBase<SampleType>::reset();
			}

			enum class RingModSource { None, Tone, Sidechain, Self };
			void setSource(RingModSource newSource)
			{
				source = newSource;
			}

			void setSidechain(juce::AudioBuffer<SampleType>* sidechainBuffer)
			{
				if (sidechainBuffer->getNumChannels() < 1 || sidechainBuffer->getNumSamples() < 1)
				{
					sidechain = &emptySidechain;
				}
				else
				{
					sidechain = sidechainBuffer;
				}
			}



			void processInternal(bool isBypassed) noexcept
			{
                if (!isBypassed)
				{

					if (sourceSource != nullptr)
					{
						sourceSource->trackBlock(BaseProcessingUnit<SampleType>::internalDryBlock);
					}

					switch (source)
					{
					case RingModSource::None:
						break;

					case RingModSource::Tone:
					{

						juce::dsp::ProcessContextReplacing<SampleType> waveContext(BaseProcessingUnit<SampleType>::internalWetBlock);
						tone.process(waveContext);

						if (toneSource != nullptr)
						{
							toneSource->trackBlock(BaseProcessingUnit<SampleType>::internalWetBlock);
						}
						BaseProcessingUnit<SampleType>::internalWetBlock.replaceWithProductOf(BaseProcessingUnit<SampleType>::internalDryBlock, BaseProcessingUnit<SampleType>::internalWetBlock);
						break;
					}

					case RingModSource::Sidechain:

						for (int i = 0; i < BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() && sidechain != nullptr; ++i)
						{
							juce::FloatVectorOperations::copy(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(i), BaseProcessingUnit<SampleType>::internalDryBlock.getChannelPointer(i), BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples());
							juce::FloatVectorOperations::multiply(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(i), sidechain->getReadPointer(i), sidechain->getNumSamples());
						}
						break;

					case RingModSource::Self:
						BaseProcessingUnit<SampleType>::internalWetBlock.replaceWithProductOf(BaseProcessingUnit<SampleType>::internalDryBlock, BaseProcessingUnit<SampleType>::internalDryBlock);
						break;

					default:
						break;
					}

                    BaseProcessingUnit<SampleType>::applyDryWetMix();
				}
			}

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SampleType();
			}

			BasicShapesGenerator<SampleType>* getToneGenerator()
			{
				return &tone;
			}


		private:
			BasicShapesGenerator<SampleType> tone;
			juce::AudioBuffer<SampleType>* sidechain = nullptr;
			juce::AudioBuffer<SampleType> emptySidechain;

			RingModSource source = RingModSource::None;
			SampleSource<SampleType>* toneSource = nullptr, * sidechainSource = nullptr, * sourceSource = nullptr;

		};
	} // namespace dsp
} // namespace bdspnamespace dsp
