#pragma once
#include "bdsp_BiQuad.h"

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class PeakingBiQuad : public BiQuadBase<SampleType>
		{
		public:
			PeakingBiQuad(DSP_Universals<SampleType>* lookupToUse)
				:BiQuadBase<SampleType>(lookupToUse)
			{
			}
			~PeakingBiQuad() = default;




			void calculateCoefficients( BaseFilterParameters<SampleType>* paramsToUse,  BaseFilterCoefficients<SampleType>* coeffsToFill) override
			{
				auto q = paramsToUse->q;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate;
				SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
				SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
				SampleType alpha = sinOmega / (2 * q);
				SampleType A = pow(10, paramsToUse->gain / 40);

				auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

				cast->a0 = 1 + alpha / A;
				cast->a1 = -2 * cosOmega;
				cast->a2 = 1 - alpha / A;





				cast->b0 = 1 + alpha * A;
				cast->b1 = -2 * cosOmega;
				cast->b2 = 1 - alpha * A;

			}

		};

		template <typename SampleType>
		class LowShelfBiQuad : public BiQuadBase<SampleType>
		{
		public:
			LowShelfBiQuad(DSP_Universals<SampleType>* lookupToUse)
				:BiQuadBase<SampleType>(lookupToUse)
			{
			}
			~LowShelfBiQuad() = default;




			void calculateCoefficients( BaseFilterParameters<SampleType>* paramsToUse,  BaseFilterCoefficients<SampleType>* coeffsToFill) override
			{
				auto q = paramsToUse->q;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate;
				SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
				SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
				SampleType alpha = sinOmega / (2 * q);
				SampleType A = pow(10, paramsToUse->gain / 40);
				SampleType sqrtA = 2 * sqrt(A) * alpha;

				auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

				SampleType minMult = (A - 1) * cosOmega;
				SampleType plusMult = (A + 1) * cosOmega;

				cast->a0 = A + 1 + minMult + sqrtA;
				cast->a1 = -2 * (A - 1 + plusMult);
				cast->a2 = A + 1 + minMult - sqrtA;





				cast->b0 = A * (A + 1 - minMult + sqrtA);
				cast->b1 = 2 * A * (A - 1 - plusMult);
				cast->b2 = A * (A + 1 - minMult - sqrtA);

			}

		};

		template <typename SampleType>
		class HighShelfBiQuad : public BiQuadBase<SampleType>
		{
		public:
			HighShelfBiQuad(DSP_Universals<SampleType>* lookupToUse)
				:BiQuadBase<SampleType>(lookupToUse)
			{
			}
			~HighShelfBiQuad() = default;




			void calculateCoefficients( BaseFilterParameters<SampleType>* paramsToUse,  BaseFilterCoefficients<SampleType>* coeffsToFill) override
			{
				auto q = paramsToUse->q;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate;
				SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
				SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
				SampleType alpha = sinOmega / (2 * q);
				SampleType A = pow(10, paramsToUse->gain / 40);
				SampleType sqrtA = 2 * sqrt(A) * alpha;

				auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);


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


		template <typename SampleType>
		class ParametricEQ : public ProcessorChain<SampleType>
		{
		public:

			ParametricEQ(DSP_Universals<SampleType>* lookupsToUse, int numBands, bool hasLowShelf = true, bool hasHighShelf = true)
			{

				if (hasLowShelf)
				{
					lowShelf = std::make_unique <LowShelfBiQuad<SampleType>>(lookupsToUse);
                    ProcessorChain<SampleType>::addProcessor(lowShelf.get());

				}
				for (int i = 0; i < numBands; ++i)
				{
					peakBands.add(new PeakingBiQuad<SampleType>(lookupsToUse));
                    ProcessorChain<SampleType>::addProcessor(peakBands.getLast());
				}
				if (hasHighShelf)
				{
					highShelf = std::make_unique <HighShelfBiQuad<SampleType>>(lookupsToUse);
                    ProcessorChain<SampleType>::addProcessor(highShelf.get());
				}

			}



			FilterBase<SampleType>* getBand(int i)
			{
				return dynamic_cast<FilterBase<SampleType>*>(ProcessorChain<SampleType>::getProcessor(i));
			}

			PeakingBiQuad<SampleType>* getPeakBand(int i)
			{
				return peakBands[i];
			}

			LowShelfBiQuad<SampleType>* getLowShelf()
			{
				return lowShelf.get();
			}

			HighShelfBiQuad<SampleType>* getHighShelf()
			{
				return highShelf.get();
			}



		private:
			juce::OwnedArray<PeakingBiQuad<SampleType>> peakBands;
			std::unique_ptr <LowShelfBiQuad<SampleType>> lowShelf;
			std::unique_ptr <HighShelfBiQuad<SampleType>> highShelf;


		};

	} // namespace dsp
} // namespace bdsp
