#pragma once 

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Base class for generators that create pitched signals.
		 * This class will calculate a phase value each sample based on the frequency set, in your derived class you need to override the getSample method to output a value based on the current phase value.
		 */
		template <typename SampleType>
		class BasicWaveGenerator : public GeneratorBase<SampleType>
		{
		public:


			BasicWaveGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:GeneratorBase<SampleType>()
			{
				lookups = lookupsToUse;
			}
			~BasicWaveGenerator() = default;

			void reset() override
			{
				GeneratorBase<SampleType>::reset();
			}

			void inline updateSmoothedVariables() override
			{
				GeneratorBase<SampleType>::updateSmoothedVariables();
				phaseDelta = frequency / BaseProcessingUnit<SampleType>::sampleRate;
				phase = modf(phase + phaseDelta, &phase);

			}

			void setFrequency(SampleType newValue)
			{
				frequency = newValue;
			}



		protected:

			SampleType frequency;
			SampleType phase = 0, phaseDelta = 0;

			DSP_Universals<SampleType>* lookups = nullptr;

		};
	} // namespace dsp
} // namespace bdsp
