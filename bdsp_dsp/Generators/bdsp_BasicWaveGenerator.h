#pragma once 
#include "bdsp_GeneratorBase.h"

namespace bdsp
{
	namespace dsp
	{

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

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				GeneratorBase<SampleType>::prepare(spec);
			}

			void reset() override
			{
				GeneratorBase<SampleType>::reset();
			}



			void inline updateSmoothedVariables() override
			{
				GeneratorBase<SampleType>::updateSmoothedVariables();
				frequency.getNextValue();
				phaseDelta = frequency.getCurrentValue() / BaseProcessingUnit<SampleType>::sampleRate;
				phase = modf(phase + phaseDelta, &phase);

				skew.getNextValue();

			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				GeneratorBase<SampleType>::setSmoothingTime(timeInSeconds);

				frequency.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				skew.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}

			SampleType getSample(int channel, int smp) override
			{
				return ((lookups->waveLookups.operator->())->*(lookupFunc))(skew.getCurrentValue(), phase);
			}


			void initFrequency(SampleType newValue)
			{
				frequency.setCurrentAndTargetValue(newValue);
			}

			void setFrequency(SampleType newValue)
			{
				frequency.setTargetValue(newValue);
			}

			void initSkew(SampleType newValue)
			{
				skew.setCurrentAndTargetValue(newValue);
			}

			void setSkew(SampleType newValue)
			{
				skew.setTargetValue(newValue);
			}

		protected:

			juce::SmoothedValue<SampleType> frequency, skew;
			SampleType phase = 0, phaseDelta = 0;

			DSP_Universals<SampleType>* lookups = nullptr;

			SampleType(WaveLookups<SampleType>::* lookupFunc) (SampleType, SampleType);

		};

		template <typename SampleType>
		class BasicShapesGenerator : public BasicWaveGenerator<SampleType>
		{
		public:

			BasicShapesGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{

			}

			void setShape(SampleType newShape)
			{
				shape = newShape;
			}


			SampleType getSample(int channel, int smp) override
			{
				return BasicWaveGenerator<SampleType>::lookups->waveLookups->getLFOValue(shape, BasicWaveGenerator<SampleType>::skew.getCurrentValue(), BasicWaveGenerator<SampleType>::phase);
			}

		private:
			SampleType shape = 0;
		};


		template <typename SampleType>
		class SinGenerator : public BasicWaveGenerator <SampleType>
		{
		public:

			SinGenerator(DSP_Universals<SampleType>* lookupsToUse)
				:BasicWaveGenerator<SampleType>(lookupsToUse)
			{
                BasicWaveGenerator<SampleType>::lookupFunc = &WaveLookups<SampleType>::fastLookupSin;
			}
		};

	} // namespace dsp
} // namespace bdsp
