#pragma once
namespace bdsp
{
	namespace dsp
	{
		template <typename SampleType>
		class SecondOrderAllpassFilter : public BiQuadBase<SampleType>
		{
		public:


			SecondOrderAllpassFilter(DSP_Universals<SampleType>* lookupsToUse)
				:BiQuadBase<SampleType>(lookupsToUse)
			{

			}
			~SecondOrderAllpassFilter() = default;

			void calculateCoefficients( BaseFilterParameters<SampleType>* paramsToUse,  BaseFilterCoefficients<SampleType>* coeffsToFill) override
			{
				auto q = paramsToUse->q;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate;
				SampleType cosOmega = cos(omega);
				SampleType sinOmega = sin(omega);
				SampleType alpha = sinOmega / (2 * q);

				auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

				cast->a0 = 1 + alpha;
				cast->a1 = -2 * cosOmega;
				cast->a2 = 1 - alpha;

				cast->b0 = cast->a2;
				cast->b1 = cast->a1;
				cast->b2 = cast->a0;



			}

		};


	} // namespace dsp
} // namespace bdsp
