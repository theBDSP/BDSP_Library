#pragma once

namespace bdsp
{
	namespace dsp
	{

		/* Coefficients used to implement a standard BiQuad filter */
		template <typename SampleType>
		struct BiQuadCoefficients : public BaseFilterCoefficients
		{
			SampleType a0 = 1, a1 = 0, a2 = 0, b0 = 0, b1 = 0, b2 = 0;
		};


		/**
		 * Base class for all BiQuad filters. This does everything except calculate the coefficients needed given a set of parameters.
		 * Derived classes only need to define calculateCoefficients to create a new BiQuad filter.
		 */
		template <typename SampleType>
		class BiQuadBase : public FilterBase<SampleType>
		{
		public:

			BiQuadBase(DSP_Universals<SampleType>* lookupToUse)
			{

				FilterBase<SampleType>::lookup = lookupToUse;

				FilterBase<SampleType>::parameters = std::make_unique<BaseFilterParameters<SampleType>>();
				FilterBase<SampleType>::visualizerParameters = std::make_unique<BaseFilterParameters<SampleType>>();

				FilterBase<SampleType>::smoothedFrequency.setCurrentAndTargetValue(FilterBase<SampleType>::parameters->frequency);
				FilterBase<SampleType>::smoothedQ.setCurrentAndTargetValue(FilterBase<SampleType>::parameters->q);
				FilterBase<SampleType>::smoothedType.setCurrentAndTargetValue(FilterBase<SampleType>::parameters->type);

				FilterBase<SampleType>::coefficients = std::make_unique<BiQuadCoefficients<SampleType>>();
				FilterBase<SampleType>::visualizerCoefficients = std::make_unique<BiQuadCoefficients<SampleType>>();

				coeffs = static_cast<BiQuadCoefficients<SampleType>*>(FilterBase<SampleType>::coefficients.get());
				visCoeffs = static_cast<BiQuadCoefficients<SampleType>*>(FilterBase<SampleType>::visualizerCoefficients.get());

				FilterBase<SampleType>::setSmoothingTime(FilterBase<SampleType>::smoothTime);
			}
			virtual ~BiQuadBase() = default;


			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				FilterBase<SampleType>::prepare(spec);

				x1.resize(spec.numChannels);
				x2.resize(spec.numChannels);
				y1.resize(spec.numChannels);
				y2.resize(spec.numChannels);

				reset();

			}


			/* Calculates a single sample using direct form 1 */
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType out = (coeffs->b0 * inputSample + coeffs->b1 * x1[channel] + coeffs->b2 * x2[channel] - coeffs->a1 * y1[channel] - coeffs->a2 * y2[channel]) / coeffs->a0;

				// set new values for the state variables
				x2.set(channel, x1[channel]);
				x1.set(channel, inputSample);

				y2.set(channel, y1[channel]);
				y1.set(channel, out);

				return BaseProcessingUnit<SampleType>::applyDryWetMix(inputSample, out);
			}



			void reset() override
			{
				BaseProcessingUnit<SampleType>::reset();

				x1.fill(0);
				x2.fill(0);
				y1.fill(0);
				y2.fill(0);
			}




			juce::dsp::Complex<SampleType> calculateResponseForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) override
			{
				this->calculateCoefficients(paramsToUse, visCoeffs); // load the correct coefficients for the given parameters

				juce::dsp::Complex<SampleType> numerator = 0.0, denominator = 0.0, factor = 1.0;

				juce::dsp::Complex<SampleType> jw = std::polar(SampleType(1), -juce::MathConstants<SampleType>::twoPi * frequency / SampleType(BaseProcessingUnit<SampleType>::sampleRate)); //  pre-calculate e^(jw) for use in DTFT

				//================================================================================================================================================================================================
				//z^0
				numerator += visCoeffs->b0 * factor;
				denominator += visCoeffs->a0 * factor;
				//================================================================================================================================================================================================
				//z^-1
				factor *= jw;
				numerator += visCoeffs->b1 * factor;
				denominator += visCoeffs->a1 * factor;
				//================================================================================================================================================================================================
				//z^-2
				factor *= jw;
				numerator += visCoeffs->b2 * factor;
				denominator += visCoeffs->a2 * factor;
				//================================================================================================================================================================================================

				return numerator / denominator;
			}


			void snapToZero() override
			{
				for (auto& v : x1)
				{
					juce::dsp::util::snapToZero(v);
				}
				for (auto& v : x2)
				{
					juce::dsp::util::snapToZero(v);
				}
				for (auto& v : y1)
				{
					juce::dsp::util::snapToZero(v);
				}
				for (auto& v : y2)
				{
					juce::dsp::util::snapToZero(v);
				}

			}

		protected:
			juce::Array<SampleType> x1, x2, y1, y2;

			BiQuadCoefficients<SampleType>* coeffs, * visCoeffs;
		};


	

	}// namespace dsp
}// namnepace bdsp


