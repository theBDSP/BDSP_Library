#pragma once

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class SampleRateReducer
		{
		public:
			SampleRateReducer() = default;
			virtual ~SampleRateReducer() = default;

			void prepare(const juce::dsp::ProcessSpec& spec)
			{
				smoothedReductionFactor.setCurrentAndTargetValue(1.0);
				reset();
			}

			void reset()
			{
				smp = StereoSample<SampleType>(0, 0);
				n = 0;
			}

			void updateSmoothedVariables()
			{
				smoothedReductionFactor.getNextValue();
			}

			void setSmoothingTime(SampleType sampleRate, SampleType timeInSeconds)
			{
				smoothedReductionFactor.reset(sampleRate, timeInSeconds);
			}

			void initReductionFactor(SampleType newFactor)
			{
				smoothedReductionFactor.setCurrentAndTargetValue(newFactor);
			}
			void setReductionFactor(SampleType newFactor)
			{
				smoothedReductionFactor.setTargetValue(newFactor);
			}


			template<class ContextType>
			void process(const ContextType& context) noexcept
			{
				const auto& inputBlock = context.getInputBlock();
				auto& outputBlock = context.getOutputBlock();
				juce::AudioBuffer<SampleType> buff;
				buff.setSize(2, inputBlock.getNumSamples());
				juce::dsp::AudioBlock<SampleType> nonConstInputBlock(buff);
				nonConstInputBlock.copyFrom(inputBlock);


				if (context.isBypassed)
				{
					return;
				}

				for (size_t i = 0; i < outputBlock.getNumSamples(); ++i)
				{
					updateVariables();

					int f = floor(smoothedReductionFactor.getCurrentValue()) - 1;
					if (n < f)
					{
						++n;
					}
					else
					{
						smp.left = processSingleSample(0, inputBlock.getSample(0, i));
						smp.right = processSingleSample(1, inputBlock.getSample(1, i));
						n = 0;
					}
					outputBlock.setSample(0, i, applyMix(inputBlock.getSample(0, i), smp.left));
					outputBlock.setSample(1, i, applyMix(inputBlock.getSample(1, i), smp.right));

				}

			}
			virtual SampleType applyMix(SampleType dry, SampleType wet) = 0;
			virtual SampleType processSingleSample(int channel, SampleType sample) = 0;
			virtual void updateVariables() = 0;
		protected:

			int n = 0;

			StereoSample<SampleType> smp;

			juce::SmoothedValue<SampleType> smoothedReductionFactor;
		};


		template <typename SampleType>
		class BitCrushDistortion : public BaseProcessingUnit<SampleType>, public SampleRateReducer<SampleType>
		{
		public:
			BitCrushDistortion(DSP_Universals<SampleType>* lookupToUse)
			{
				lookups = lookupToUse;
				lookupToUse->distortionLookups->generateTables(DistortionTypes::BitCrush);
			}
			~BitCrushDistortion() = default;

			//==============================================================================
			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);
				SampleRateReducer<SampleType>::prepare(spec);
			}

			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();
				SampleRateReducer<SampleType>::reset();

			}
			void processInternal(bool isBypassed) noexcept override
			{
				if (!isBypassed)
				{
					SampleRateReducer<SampleType>::process(*BaseProcessingUnit<SampleType>::internalContext.get());
				}
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				SampleRateReducer<SampleType>::setSmoothingTime(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
				smoothedBitDepth.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}
			void updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
				SampleRateReducer<SampleType>::updateSmoothedVariables();
				smoothedBitDepth.getNextValue();
			}

			void initBitDepth(SampleType initValue)
			{
				smoothedBitDepth.setCurrentAndTargetValue(initValue);
			}
			void setBitDepth(SampleType newValue)
			{
				smoothedBitDepth.setTargetValue(newValue);
			}
		protected:

			SampleType applyMix(SampleType dry, SampleType wet) override
			{
				return BaseProcessingUnit<SampleType>::applyDryWetMix(dry, wet);
			}
			SampleType processSingleSample(int channel, SampleType sample) override
			{
				return processSample(channel, sample);
			}
			void updateVariables() override
			{
				updateSmoothedVariables();
			}

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return  lookups->distortionLookups->getBitCrushDistortion(inputSample, smoothedBitDepth.getCurrentValue(), false);
			}

			DSP_Universals<SampleType>* lookups;
			juce::SmoothedValue<SampleType> smoothedBitDepth;

		};



	}// namespace dsp
}// namnepace bdsp


