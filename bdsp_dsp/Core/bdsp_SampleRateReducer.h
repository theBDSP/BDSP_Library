#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * Basic Sample Rate Reduction.
		 * Only capable of integer reduction factors.
		 */
		template <typename SampleType>
		class SampleRateReducer : public BaseProcessingUnit<SampleType>
		{
		public:
			SampleRateReducer() = default;
			virtual ~SampleRateReducer() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);
				reset();
			}

			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();
				smp = StereoSample<SampleType>(0, 0);
				n = 0;
			}


			void setReductionFactor(SampleType newFactor)
			{
				reductionFactor = newFactor;
			}


			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed)
				{
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				for (size_t i = 0; i < numSamples; ++i)
				{
					BaseProcessingUnit<SampleType>::updateSmoothedVariables();


					if (n < reductionFactor - 1) // discard every (reductionFactor - 1) samples
					{
						++n;
					}
					else
					{
						smp.left = BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i);
						smp.right = BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i);
						n = 0;
					}

					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, smp.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, smp.right);

				}
				BaseProcessingUnit<SampleType>::applyDryWetMix();
			}


			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SampleType();
			}

		protected:

			int n = 0;

			StereoSample<SampleType> smp;

			SampleType reductionFactor = 1;
		};

	} //dsp namespace
} //bdsp namespace
