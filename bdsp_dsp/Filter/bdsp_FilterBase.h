#pragma once

// 1/sqrt(2)
#define BDSP_FILTER_DEFAULT_Q  0.7071067811865475
namespace bdsp
{
	namespace dsp
	{
		template <typename SampleType>
		struct BaseFilterParameters // set of values used to calculate plots for filter. inherit this into a new struct to add more parameters
		{
			BaseFilterParameters()
			{
				frequency = 20;
				q = BDSP_FILTER_DEFAULT_Q;
				type = 0;
				gain = 1;
			}

			SampleType frequency;
			SampleType q;
			SampleType type;
			SampleType gain;
		};

		template <typename SampleType>
		struct BaseFilterCoefficients // inherit this into a new struct for each child class
		{

		};
		template <typename SampleType>
		class FilterBase : public BaseProcessingUnit<SampleType>
		{
		public:

			FilterBase() = default;


			virtual ~FilterBase() override
			{

				parameters.reset();
				coefficients.reset();


				visualizerParameters.reset();
				visualizerCoefficients.reset();
			}



			juce::dsp::Complex<SampleType> calculateResponseForNormalizedFrequency( BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return calculateResponseForFrequency(paramsToUse, normalizedFrequency * BaseProcessingUnit<SampleType>::sampleRate / 2);
			}

			virtual	juce::dsp::Complex<SampleType> calculateResponseForFrequency( BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) = 0;
			SampleType getMagnitudeForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency)
			{
				return std::abs(calculateResponseForFrequency(paramsToUse, frequency));
			}

			SampleType getMagnitudeForNormalizedFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return std::abs(calculateResponseForNormalizedFrequency(paramsToUse, normalizedFrequency));
			}

			SampleType getPhaseForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency)
			{
				return std::arg(calculateResponseForFrequency(paramsToUse, frequency));
			}	
			
			SampleType getPhaseForNormalizedFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return std::arg(calculateResponseForNormalizedFrequency(paramsToUse, normalizedFrequency));
			}

            virtual void calculateCoefficients(BaseFilterParameters<SampleType>* paramsToUse, BaseFilterCoefficients<SampleType>* coeffsToFill) = 0;

			void initFrequency(SampleType initFreq)
			{
				smoothedFrequency.setCurrentAndTargetValue(initFreq);
				if (linkedFilter != nullptr)
				{
					linkedFilter->initFrequency(initFreq);
				}
			}

			void initQFactor(SampleType initQ)
			{
				smoothedQ.setCurrentAndTargetValue(initQ);

				if (linkedFilter != nullptr)
				{
					linkedFilter->initQFactor(initQ);
				}
			}
			void initType(SampleType initType)
			{
				smoothedType.setCurrentAndTargetValue(initType);

				if (linkedFilter != nullptr)
				{
					linkedFilter->initType(initType);
				}
			}


			void setFrequency(SampleType newFreq)
			{
				smoothedFrequency.setTargetValue(newFreq);

				if (linkedFilter != nullptr)
				{
					linkedFilter->setFrequency(newFreq);
				}
			}

			void setQFactor(SampleType newQ)
			{
				smoothedQ.setTargetValue(newQ);

				if (linkedFilter != nullptr)
				{
					linkedFilter->setQFactor(newQ);
				}
			}

			void setType(SampleType newType)
			{
				smoothedType.setTargetValue(newType);

				if (linkedFilter != nullptr)
				{
					linkedFilter->setType(newType);
				}
			}


			void initGain(SampleType newValue)
			{
				smoothedGain.setCurrentAndTargetValue(newValue);

				if (linkedFilter != nullptr)
				{
					linkedFilter->initGain(newValue);
				}
			}
			void setGain(SampleType newValue)
			{
				smoothedGain.setTargetValue(newValue);

				if (linkedFilter != nullptr)
				{
					linkedFilter->setGain(newValue);
				}
			}

			SampleType getFrequency()
			{
				return smoothedFrequency.getCurrentValue();
			}
			SampleType getQ()
			{
				return smoothedQ.getCurrentValue();
			}
			SampleType getType()
			{
				return smoothedType.getCurrentValue();
			}

			SampleType getGain()
			{
				return smoothedGain.getCurrentValue();
			}


			inline void updateSmoothedVariables() override
			{
				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
				parameters->frequency = smoothedFrequency.getNextValue();
				parameters->q = smoothedQ.getNextValue();
				parameters->type = smoothedType.getNextValue();
				parameters->gain = smoothedGain.getNextValue();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
				smoothedFrequency.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
				smoothedQ.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
				smoothedType.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
				smoothedGain.reset(BaseProcessingUnit<SampleType>::sampleRate, BaseProcessingUnit<SampleType>::smoothTime);
			}

			void processInternal(bool isBypassed) noexcept override
			{



				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);

				for (size_t i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					calculateCoefficients(parameters.get(), coefficients.get());

					if (linkedFilter != nullptr)
					{
						linkedFilter->calculateCoefficients(linkedFilter->parameters.get(), linkedFilter->coefficients.get());
					}
					for (int j = 0; j < numChannels; ++j)
					{
						auto smp = this->processSample(j, BaseProcessingUnit<SampleType>::internalDryBlock.getSample(j, i)); // calculate each sample
						BaseProcessingUnit<SampleType>::internalWetBlock.setSample(j, i, smp);
					}

				}

				snapToZero();
				//applyDryWetMix();

			}


			virtual void snapToZero()
			{
			}
		protected:
			juce::SmoothedValue<SampleType> smoothedFrequency, smoothedQ;
			juce::SmoothedValue<SampleType> smoothedType;
			juce::SmoothedValue<SampleType> smoothedGain;

			DSP_Universals<SampleType>* lookup = nullptr;


		public:

			void setLinkedFilter(FilterBase* f)
			{
                BaseProcessingUnit<SampleType>::linkedProcessor = f;
				linkedFilter = f;
                BaseProcessingUnit<SampleType>::linkedProcessorCopiesMix = true;
			}

			FilterBase* linkedFilter = nullptr; // filter that should share same parameter values
			std::unique_ptr<BaseFilterParameters<SampleType>> parameters; //  create seperate struct instance in child class and assign its pointer to this
			std::unique_ptr<BaseFilterCoefficients<SampleType>> coefficients; //  create seperate struct instance in child class and assign its pointer to this

			std::unique_ptr<BaseFilterParameters<SampleType>> visualizerParameters; //  create seperate struct instance in child class and assign its pointer to this
			std::unique_ptr<BaseFilterCoefficients<SampleType>> visualizerCoefficients; //  create seperate struct instance in child class and assign its pointer to this


		};
	} // namespace dsp
} // namespace bdsp
