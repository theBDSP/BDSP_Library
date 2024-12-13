#pragma once 


namespace bdsp
{
	namespace dsp
	{

		template <typename SampleType>
		class GeneratorBase : public BaseProcessingUnit<SampleType>
		{
		public:


			GeneratorBase()
			{
				gain.initMix(1);
				gain.initGain(1);
			}
			~GeneratorBase() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);
				gain.prepare(spec);
			}

			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();
				gain.reset();
			}

			void processInternal(bool bypassed) noexcept override
			{
				if (!bypassed)
				{
					for (int i = 0; i < BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples(); ++i)
					{
						updateSmoothedVariables();
						for (int j = 0; j < BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels(); ++j)
						{
							BaseProcessingUnit<SampleType>::internalWetBlock.setSample(j, i, gain.processSample(j, (getSample(j, i))));
						}
					}
                    BaseProcessingUnit<SampleType>::applyDryWetMix();
				}
			}

			void setGain(SampleType newValue)
			{
				gain.setGain(newValue);
			}

			void initGain(SampleType initValue)
			{
				gain.initGain(initValue);
			}

			void inline updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
				gain.updateSmoothedVariables();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
				gain.setSmoothingTime(timeInSeconds);
			}

			virtual SampleType getSample(int channel, int smp) = 0;


			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SampleType();
			}

		protected:

			Gain<SampleType> gain;
		};

	} // namespace dsp
} // namespace bdsp
