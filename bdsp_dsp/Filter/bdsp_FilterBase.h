#pragma once

constexpr double BDSP_FILTER_DEFAULT_Q = 0.7071067811865475; // 1 / sqrt(2)

namespace bdsp
{
	namespace dsp
	{
		/**
		 * A set of values used to calculate filter coefficients.
		 * Derive a new struct from this one to add more parameters for a specific type of filter.
		 */
		template <typename SampleType>
		struct BaseFilterParameters
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

		/**
		 * Derive a new struct from this one to add more coefficients for a specific type of filter.
		 */
		struct BaseFilterCoefficients
		{
	
		};

		/**
		 * Bolierplate class for everything needed to implement a filter processor (except the actual filtering).
		 * To create a new filter class, derive from this class and define calculateResponseForFrequency and calculateCoefficients.
		 * Don't forget to create instances of the proper type for all parameter and coefficient pointers in your constructor.
		 */
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


			/* Calculates a complex response from a set of parameters for a given frequency */
			virtual	juce::dsp::Complex<SampleType> calculateResponseForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) = 0;

			/* Calculates a complex response from a set of parameters for a given normalized frequency */
			juce::dsp::Complex<SampleType> calculateResponseForNormalizedFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return calculateResponseForFrequency(paramsToUse, normalizedFrequency * BaseProcessingUnit<SampleType>::sampleRate / 2);
			}

			/* Returns the maginatude response from a set of parameters for a given frequency */
			SampleType getMagnitudeForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency)
			{
				return std::abs(calculateResponseForFrequency(paramsToUse, frequency));
			}

			/* Returns the maginatude response from a set of parameters for a given normalized frequency */
			SampleType getMagnitudeForNormalizedFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return std::abs(calculateResponseForNormalizedFrequency(paramsToUse, normalizedFrequency));
			}

			/* Returns the phase response from a set of parameters for a given frequency */
			SampleType getPhaseForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency)
			{
				return std::arg(calculateResponseForFrequency(paramsToUse, frequency));
			}

			/* Returns the phase response from a set of parameters for a given normalized frequency */
			SampleType getPhaseForNormalizedFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType normalizedFrequency)
			{
				return std::arg(calculateResponseForNormalizedFrequency(paramsToUse, normalizedFrequency));
			}

			/* Calculates a set of coefficients for a given set of parameters and fills the results into the provided set of coefficients */
			virtual void calculateCoefficients(BaseFilterParameters<SampleType>* paramsToUse, BaseFilterCoefficients* coeffsToFill) = 0;

			void initFrequency(SampleType initFreq)
			{
				smoothedFrequency.setCurrentAndTargetValue(initFreq);
			}

			void initQFactor(SampleType initQ)
			{
				smoothedQ.setCurrentAndTargetValue(initQ);
			}
			void initType(SampleType initType)
			{
				smoothedType.setCurrentAndTargetValue(initType);
			}


			void setFrequency(SampleType newFreq)
			{
				smoothedFrequency.setTargetValue(newFreq);
			}

			void setQFactor(SampleType newQ)
			{
				smoothedQ.setTargetValue(newQ);
			}

			void setType(SampleType newType)
			{
				smoothedType.setTargetValue(newType);
			}


			void initGain(SampleType newValue)
			{
				smoothedGain.setCurrentAndTargetValue(newValue);

			}
			void setGain(SampleType newValue)
			{
				smoothedGain.setTargetValue(newValue);
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

			/* Additionally loads the smoothed values into the parameters*/
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

					for (int j = 0; j < numChannels; ++j)
					{
						auto smp = this->processSample(j, BaseProcessingUnit<SampleType>::internalDryBlock.getSample(j, i));
						BaseProcessingUnit<SampleType>::internalWetBlock.setSample(j, i, smp);
					}

				}

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
				snapToZero();
#endif			
			}

			/**
			 * Ensure that the state variables are rounded to zero if the state variables are denormals.
			 */
			virtual void snapToZero()
			{
			}


		protected:
			juce::SmoothedValue<SampleType> smoothedFrequency;
			juce::SmoothedValue<SampleType> smoothedQ;
			juce::SmoothedValue<SampleType> smoothedType;
			juce::SmoothedValue<SampleType> smoothedGain;

			DSP_Universals<SampleType>* lookup = nullptr;


		public:

			//================================================================================================================================================================================================
			// Create in derived class constructor
			std::unique_ptr<BaseFilterParameters<SampleType>> parameters;
			std::unique_ptr<BaseFilterCoefficients> coefficients; 

			std::unique_ptr<BaseFilterParameters<SampleType>> visualizerParameters; // parameters used by visualizers to generate a response graph
			std::unique_ptr<BaseFilterCoefficients> visualizerCoefficients; // coefficients used by visualizers to calculate a response graph
			//================================================================================================================================================================================================

		};

		/*template <typename SampleType, class FilterType>
		class StereoFilter : public FilterType
		{
		public:

			template<class ... Types>
			StereoFilter(Types...args)
				:FilterType(args...),
				right(args...)
			{
				FilterBase<SampleType>* f = FilterType::linkedFilter;

				while (f->linkedFilter)
				{
					f = f->linkedFilter;
				}

				f->setLinkedFilter(&right, true);
			}



			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{

				return channel == 0 ? FilterType::processSample(0, inputSample) : right.processSample(0, inputSample);
			}

			void processInternal(bool isBypassed) noexcept override
			{


				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}



				for (size_t i = 0; i < BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples(); ++i)
				{
					FilterType::updateSmoothedVariables();
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, processSample(0, BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i)));
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, processSample(1, BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)));
				}
				applyDryWetMix();

			}
		protected:
			FilterType right;
		};*/


	} // namespace dsp
} // namespace bdsp
