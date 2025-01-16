#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace BiQuadFilters
		{
			/**
			 * Peaking biquad filter. Formula adapted from: https://www.w3.org/TR/audio-eq-cookbook/
			 */
			template <typename SampleType>
			class Peaking : public BiQuadBase<SampleType>
			{
			public:
				Peaking(DSP_Universals<SampleType>* lookupToUse)
					:BiQuadBase<SampleType>(lookupToUse)
				{
				}
				~Peaking() = default;




				void calculateCoefficients(BaseFilterParameters<SampleType>* paramsToUse, BaseFilterCoefficients* coeffsToFill) override
				{
					auto q = paramsToUse->q;
					SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate; // limited to [0,2 * PI] given a frequency below nyquist

					//================================================================================================================================================================================================
					// calculate intermediate variables
					SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
					SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
					SampleType alpha = sinOmega / (2 * q);
					SampleType A = pow(10, paramsToUse->gain / 40);
					//================================================================================================================================================================================================

					auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

					cast->a0 = 1 + alpha / A;
					cast->a1 = -2 * cosOmega;
					cast->a2 = 1 - alpha / A;





					cast->b0 = 1 + alpha * A;
					cast->b1 = -2 * cosOmega;
					cast->b2 = 1 - alpha * A;

				}

			};
		} //namespace BiquadFilters
	} //namespace dsp
} //namespace bdsp