#pragma once

namespace bdsp
{
	namespace dsp
	{

		template <typename SampleType>
		struct BiQuadCoefficients : public BaseFilterCoefficients<SampleType>
		{
			SampleType a0 = 1, a1 = 0, a2 = 0, b0 = 0, b1 = 0, b2 = 0;
		};

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

				FilterBase<SampleType>::setSmoothingTime(FilterBase<SampleType>::smoothTime);
			}
			~BiQuadBase() = default;


			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				FilterBase<SampleType>::prepare(spec);

				x1.resize(spec.numChannels);
				x2.resize(spec.numChannels);
				y1.resize(spec.numChannels);
				y2.resize(spec.numChannels);

				reset();

			}



			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{

				//FilterBase<SampleType>::updateSmoothedVariables();

				SampleType out = (coeffs->b0 * inputSample + coeffs->b1 * x1[channel] + coeffs->b2 * x2[channel] - coeffs->a1 * y1[channel] - coeffs->a2 * y2[channel]) / coeffs->a0;

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




			juce::dsp::Complex<SampleType> calculateResponseForFrequency( BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) override
			{
				BiQuadCoefficients<SampleType> c;
				this->calculateCoefficients(paramsToUse, &c);
				constexpr juce::dsp::Complex<SampleType> j(0, 1);

				//jassert(frequency >= 0 && frequency <= BaseProcessingUnit<SampleType>::sampleRate * 0.5);

				juce::dsp::Complex<SampleType> numerator = 0.0, denominator = 0.0, factor = 1.0;
				//juce::dsp::Complex<SampleType> jw = std::exp(-j * juce::MathConstants<SampleType>::twoPi * frequency / SampleType(BaseProcessingUnit<SampleType>::sampleRate));
				juce::dsp::Complex<SampleType> jw = std::polar(SampleType(1), -juce::MathConstants<SampleType>::twoPi * frequency / SampleType(BaseProcessingUnit<SampleType>::sampleRate));


				numerator += c.b0 * factor;
				denominator += c.a0 * factor;
				factor *= jw;

				numerator += c.b1 * factor;
				denominator += c.a1 * factor;
				factor *= jw;

				numerator += c.b2 * factor;
				denominator += c.a2 * factor;






				return numerator / denominator;
			}



		protected:


			//SampleType x_1 = 0, x_2 = 0, y_1 = 0, y_2 = 0;
			juce::Array<SampleType> x1, x2, y1, y2;


			BiQuadCoefficients<SampleType>* coeffs;


		};


		// coefficient formulae from:
		// https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
		template <typename SampleType>
		class SVF_BiQuadFilter : public BiQuadBase<SampleType>
		{
		public:

			SVF_BiQuadFilter(DSP_Universals<SampleType>* lookupToUse)
				:BiQuadBase<SampleType>(lookupToUse)
			{

			}
			~SVF_BiQuadFilter() = default;



			void calculateCoefficients( BaseFilterParameters<SampleType>* paramsToUse,  BaseFilterCoefficients<SampleType>* coeffsToFill) override
			{
				auto q = paramsToUse->q;
				auto type = paramsToUse->type;
				SampleType omega = 2 * PI * paramsToUse->frequency / BaseProcessingUnit<SampleType>::sampleRate;
				SampleType cosOmega = FilterBase<SampleType>::lookup->trigLookups->cos(omega);
				SampleType sinOmega = FilterBase<SampleType>::lookup->trigLookups->sin(omega);
				SampleType alpha = sinOmega / (2 * q);

				auto cast = static_cast<BiQuadCoefficients<SampleType>*>(coeffsToFill);

				cast->a0 = 1 + alpha;
				cast->a1 = -2 * cosOmega;
				cast->a2 = 1 - alpha;

				SampleType LPAmt = juce::jlimit(SampleType(0), SampleType(1), -2 * type + 1);
				SampleType BPAmt = juce::jlimit(SampleType(0), SampleType(1), 1 - abs(2 * type - 1));
				SampleType HPAmt = juce::jlimit(SampleType(0), SampleType(1), 2 * type - 1);

				SampleType minusCos = (1 - cosOmega);
				SampleType plusCos = (1 + cosOmega);
				SampleType minusCosOver2 = minusCos / 2;
				SampleType plusCosOver2 = plusCos / 2;

				auto limitQ = juce::jmax(q, SampleType(1));
				LPAmt /= limitQ;
				HPAmt /= limitQ;


				cast->b0 = LPAmt * minusCosOver2 + BPAmt * alpha + HPAmt * plusCosOver2;
				cast->b1 = LPAmt * minusCos - HPAmt * plusCos;
				cast->b2 = LPAmt * minusCosOver2 - BPAmt * alpha + HPAmt * plusCosOver2;



			}

		};



		/*	template <typename SampleType>
			using SVF_BiQuadFilterStereo = StereoFilter<SampleType, SVF_BiQuadFilter<SampleType>>;*/

		template <typename SampleType>
		class SVF_SecondOrderBiquadFilter : public SVF_BiQuadFilter<SampleType>
		{
		public:

			SVF_SecondOrderBiquadFilter(DSP_Universals<SampleType>* lookupToUse)
				:SVF_BiQuadFilter<SampleType>(lookupToUse),
				first(lookupToUse)
			{
				FilterBase<SampleType>::setLinkedFilter(&first);
			}

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SVF_BiQuadFilter<SampleType>::processSample(channel, first.processSample(channel, inputSample));
			}

			juce::dsp::Complex<SampleType> calculateResponseForFrequency( BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) override
			{
				return SVF_BiQuadFilter<SampleType>::calculateResponseForFrequency(paramsToUse, frequency) * first.calculateResponseForFrequency(paramsToUse, frequency);
			}


			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				SVF_BiQuadFilter<SampleType>::prepare(spec);
				first.prepare(spec);
			}

			void reset() override
			{
				SVF_BiQuadFilter<SampleType>::reset();
				first.reset();
			}


			//void calculateCoefficients(typename BaseFilterParameters<SampleType>* paramsToUse, typename BaseFilterCoefficients<SampleType>* coeffsToFill) override
			//{
			//	SVF_BiQuadFilter<SampleType>::calculateCoe


			//}


		protected:

			SVF_BiQuadFilter<SampleType> first;
		};

		/*	template <typename SampleType>
			using SVF_SecondOrderBiquadFilterStereo = StereoFilter<SampleType, SVF_SecondOrderBiquadFilter<SampleType>>;*/

	}// namespace dsp
}// namnepace bdsp


