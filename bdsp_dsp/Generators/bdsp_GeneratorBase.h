#pragma once 
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Base class for all processors that generate signal without the need for an input signal.
		 * The generated signal is treated like a wet signal and can be mixed with the input signal using if desired.
		 * To create a new generator all you need to do is derive from this class and override the getSample method.
		 */
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
							BaseProcessingUnit<SampleType>::internalWetBlock.setSample(j, i, gain.processSample(j, (getSample(j)))); // calcuate sample for each channel and apply output gain
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

			/**
			 * Derived classes override this to define how the signal is generated
			 */
			virtual SampleType getSample(int channel) = 0;

		protected:

			Gain<SampleType> gain;
		};

	} // namespace dsp
} // namespace bdsp
