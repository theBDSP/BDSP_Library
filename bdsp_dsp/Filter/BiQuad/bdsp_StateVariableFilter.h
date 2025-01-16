#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace BiQuadFilters
		{

		/**
		 * State-Variable Biquad. Can continuosly sweep between a low-pass, band-pass,and high-pass filter.
		 * coefficient formulae from: https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
		 */
		template <typename SampleType>
		class StateVariableFilter : public BiQuadBase<SampleType>
		{
		public:

			StateVariableFilter(DSP_Universals<SampleType>* lookupToUse)
				:BiQuadBase<SampleType>(lookupToUse)
			{

			}
			~StateVariableFilter() = default;



			void calculateCoefficients(BaseFilterParameters<SampleType>* paramsToUse, BaseFilterCoefficients* coeffsToFill) override
			{
				auto& q = paramsToUse->q;
				auto& type = paramsToUse->type;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate; // limited to [0,2 * PI] given a frequency below nyquist

				//================================================================================================================================================================================================
				// calculate intermediate variables
				SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
				SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
				SampleType alpha = sinOmega / (2 * q);

				SampleType minusCos = (1 - cosOmega);
				SampleType plusCos = (1 + cosOmega);
				SampleType minusCosOver2 = minusCos / 2;
				SampleType plusCosOver2 = plusCos / 2;
				//================================================================================================================================================================================================

				auto* cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

				//================================================================================================================================================================================================
				// These coefficients are the same across all 3 filter types
				cast->a0 = 1 + alpha;
				cast->a1 = -2 * cosOmega;
				cast->a2 = 1 - alpha;
				//================================================================================================================================================================================================

				SampleType LPAmt = juce::jlimit(SampleType(0), SampleType(1), -2 * type + 1);
				SampleType BPAmt = juce::jlimit(SampleType(0), SampleType(1), 1 - abs(2 * type - 1));
				SampleType HPAmt = juce::jlimit(SampleType(0), SampleType(1), 2 * type - 1);

				// scale LP and HP amounts to maintain a consistent max amplitude response with the BP filter
				auto limitQ = juce::jmax(q, SampleType(1));
				LPAmt /= limitQ;
				HPAmt /= limitQ;


				cast->b0 = LPAmt * minusCosOver2 + BPAmt * alpha + HPAmt * plusCosOver2;
				cast->b1 = LPAmt * minusCos - HPAmt * plusCos;
				cast->b2 = LPAmt * minusCosOver2 - BPAmt * alpha + HPAmt * plusCosOver2;



			}

		};
		} // namespace BiQuadFilters
	} //namespace dsp
} //namespace bdsp