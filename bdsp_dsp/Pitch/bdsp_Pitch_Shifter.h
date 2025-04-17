#pragma once


namespace bdsp
{
	namespace dsp
	{
		/**
		 * Takes an audio block and wraps it in a way that the signal smith stretcher can use it
		 */
		template<typename SampleType>
		class SignalSmithBlockBridge
		{
		public:
			SignalSmithBlockBridge(juce::dsp::AudioBlock<SampleType>& block)
			{
				arr.addArray(block.getChannelPointer(0), block.getNumSamples());
			}
		
			SignalSmithBlockBridge(const juce::dsp::AudioBlock<const SampleType>& block)
			{
				arr.addArray(block.getChannelPointer(0), block.getNumSamples());
			}

			SignalSmithBlockBridge(juce::AudioBuffer<SampleType>& buffer)
			{
				arr.addArray(buffer.getReadPointer(0), buffer.getNumSamples());
			}

			SampleType* operator[](int ch)
			{
				return arr.getRawDataPointer();
			}

			SampleType* data()
			{
				return arr.data();
			}
		private:
			juce::Array<SampleType> arr;
		};

		/**
		 * Wraps Signal Smith's pitch shifting library in a BDSP context.
		 * https://github.com/Signalsmith-Audio/signalsmith-stretch
		 */
		template<typename SampleType>
		class PitchShifter : public BaseProcessingUnit<SampleType>
		{
		public:
			PitchShifter();

			~PitchShifter() = default;


			void initShiftAmount(int channel, SampleType semitones);
			void setShiftAmount(int channel, SampleType semitones);

			void prepare(const juce::dsp::ProcessSpec& spec) override;

			void reset() override;

			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();

				for (size_t c = 0; c < numChannels; ++c)
				{

					SignalSmithBlockBridge<SampleType> inputBridge(BaseProcessingUnit<SampleType>::internalDryBlock.getSingleChannelBlock(c));
					SignalSmithBlockBridge<SampleType> outputBridge(BaseProcessingUnit<SampleType>::internalWetBlock.getSingleChannelBlock(c));

					pitchAmt[c]->skip(numSamples);
					stretch[c]->setTransposeSemitones(pitchAmt[c]->getCurrentValue());
					stretch[c]->process(inputBridge, numSamples, outputBridge, numSamples);

					juce::FloatVectorOperations::copy(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(0), outputBridge.data(), BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples());
				}

				latencyComp.mixWithLatencyCompensation(*BaseProcessingUnit<SampleType>::internalContext.get(), BaseProcessingUnit<SampleType>::smoothedDryMix, BaseProcessingUnit<SampleType>::smoothedWetMix);
			}

			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;

			int getLatency() override
			{
				jassert(!stretch.isEmpty());
				return stretch.getFirst()->inputLatency() + stretch.getFirst()->outputLatency();
			}

		private:
			juce::OwnedArray<signalsmith::stretch::SignalsmithStretch<SampleType>> stretch;
			juce::OwnedArray<juce::SmoothedValue<SampleType>> pitchAmt;

			LatencyCompensator<SampleType> latencyComp;
		};
	} // namespace dsp
} // namespace bdsp
