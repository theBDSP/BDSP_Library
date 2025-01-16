#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace BiQuadFilters
		{
			/**
			 * High-Shelf biquad filter. Formula adapted from: https://www.w3.org/TR/audio-eq-cookbook/
			 */
			template <typename SampleType>
			class HighShelf : public BiQuadBase<SampleType>
			{
			public:
				HighShelf(DSP_Universals<SampleType>* lookupToUse)
					:BiQuadBase<SampleType>(lookupToUse)
				{
				}
				~HighShelf() = default;




				void calculateCoefficients(BaseFilterParameters<SampleType>* paramsToUse, BaseFilterCoefficients* coeffsToFill) override
				{
					auto& q = paramsToUse->q;
					SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate; // limited to [0,2 * PI] given a frequency below nyquist
					//================================================================================================================================================================================================
					// calculate intermediate variables
					SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
					SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
					SampleType alpha = sinOmega / (2 * q);
					SampleType A = pow(10, paramsToUse->gain / 40);
					SampleType sqrtA = 2 * sqrt(A) * alpha;
					//================================================================================================================================================================================================

					auto* cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);


					SampleType minMult = (A - 1) * cosOmega;
					SampleType plusMult = (A + 1) * cosOmega;

					cast->a0 = A + 1 - minMult + sqrtA;
					cast->a1 = 2 * (A - 1 - plusMult);
					cast->a2 = A + 1 - minMult - sqrtA;

					cast->b0 = A * (A + 1 + minMult + sqrtA);
					cast->b1 = -2 * A * (A - 1 + plusMult);
					cast->b2 = A * (A + 1 + minMult - sqrtA);
				}

			};
		} // namespace BiQuadFilters
	} //namespace dsp
} //namespace bdsp