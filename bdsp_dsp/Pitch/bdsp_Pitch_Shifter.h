#pragma once


namespace bdsp
{
	namespace dsp
	{

		template<typename SampleType>
		class SignalSmithBlockBridge // takes audio block and wraps it in a way that signal smith stretcher can use it
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


		template<typename SampleType>
		class PitchShifter : public BaseProcessingUnit<SampleType>
		{
		public:
			PitchShifter();

			~PitchShifter() = default;


			void initShiftAmount(SampleType semitones);
			void setShiftAmount(SampleType semitones);

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

				//juce::AudioBuffer<SampleType> inputCopy(numChannels, numSamples);
				//for (int i = 0; i < numChannels; ++i)
				//{
				//	inputCopy.copyFrom(i, 0,  BaseProcessingUnit<SampleType>::internalDryBlock.getChannelPointer(i), numSamples);
				//}

				SignalSmithBlockBridge<SampleType> inputBridge(BaseProcessingUnit<SampleType>::internalDryBlock);
				SignalSmithBlockBridge<SampleType> outputBridge(BaseProcessingUnit<SampleType>::internalWetBlock);

				pitchAmt.skip(numSamples);
				stretch.setTransposeSemitones(pitchAmt.getCurrentValue());
				stretch.process(inputBridge, numSamples, outputBridge, numSamples);

				//juce::dsp::AudioBlock<SampleType> input(inputCopy);
				//juce::dsp::ProcessContextNonReplacing<SampleType> latencyContext(input, BaseProcessingUnit<SampleType>::internalWetBlock);
				latencyComp.mixWithLatencyCompensation(*BaseProcessingUnit<SampleType>::internalContext.get(), BaseProcessingUnit<SampleType>::smoothedDryMix, BaseProcessingUnit<SampleType>::smoothedWetMix);
			}



			inline StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override;

			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;

			int getLatency() override
			{
				return stretch.inputLatency() + stretch.outputLatency();
			}

		private:
			signalsmith::stretch::SignalsmithStretch<SampleType> stretch;
			juce::SmoothedValue<SampleType> pitchAmt;

			LatencyCompensator<SampleType> latencyComp;




		};

		template<typename SampleType>
		class StereoPitchShifter : public BaseProcessingUnit<SampleType>
		{
		public:
			StereoPitchShifter();
			~StereoPitchShifter() = default;

			void initShiftAmount(SampleType semitonesL, SampleType semitonesR);
			void setShiftAmount(SampleType semitonesL, SampleType semitonesR);

			void prepare(const juce::dsp::ProcessSpec& spec) override;

			void reset() override;

			void processInternal(bool isBypassed) noexcept override
			{
			

				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();





				SignalSmithBlockBridge<SampleType> inputBridgeL(BaseProcessingUnit<SampleType>::internalDryBlock.getSingleChannelBlock(0));
				SignalSmithBlockBridge<SampleType> inputBridgeR(BaseProcessingUnit<SampleType>::internalDryBlock.getSingleChannelBlock(1));

				SignalSmithBlockBridge<SampleType> outputBridgeL(BaseProcessingUnit<SampleType>::internalWetBlock.getSingleChannelBlock(0));
				SignalSmithBlockBridge<SampleType> outputBridgeR(BaseProcessingUnit<SampleType>::internalWetBlock.getSingleChannelBlock(1));

				pitchAmtL.skip(numSamples);
				pitchAmtR.skip(numSamples);
				stretchL.setTransposeSemitones(pitchAmtL.getCurrentValue());
				stretchR.setTransposeSemitones(pitchAmtR.getCurrentValue());
				stretchL.process(inputBridgeL, numSamples, outputBridgeL, numSamples);
				stretchR.process(inputBridgeR, numSamples, outputBridgeR, numSamples);

				juce::FloatVectorOperations::copy(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(0), outputBridgeL.data(), BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples());
				juce::FloatVectorOperations::copy(BaseProcessingUnit<SampleType>::internalWetBlock.getChannelPointer(1), outputBridgeR.data(), BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples());




				latencyComp.mixWithLatencyCompensation(*BaseProcessingUnit<SampleType>::internalContext.get(), BaseProcessingUnit<SampleType>::smoothedDryMix, BaseProcessingUnit<SampleType>::smoothedWetMix);


			}





			inline StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override;

			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;

			int getLatency() override
			{
				return stretchL.inputLatency() + stretchL.outputLatency();
			}

		private:
			signalsmith::stretch::SignalsmithStretch<SampleType> stretchL, stretchR;
			juce::SmoothedValue<SampleType> pitchAmtL, pitchAmtR;

			LatencyCompensator<SampleType> latencyComp;

		};




	} // namespace dsp
} // namespace bdsp
