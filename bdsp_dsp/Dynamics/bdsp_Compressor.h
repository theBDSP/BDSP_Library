#pragma once
#include "bdsp_DynamicsBase.h"


#define BDSP_COMPRESSOR_NEGATIVE_INFINITY_DB -200
#define BDSP_COMPRESSOR_KNEE_MAX 0.25
namespace bdsp
{
	namespace dsp
	{

		template <typename SampleType>
		class Compressor : public DynamicsBase<SampleType>
		{
		public:


			void setThresholdDB(SampleType newThresholdDB)
			{
				threshold = juce::Decibels::decibelsToGain(newThresholdDB, static_cast<SampleType> (BDSP_COMPRESSOR_NEGATIVE_INFINITY_DB));
			}


			void setThreshold(SampleType newThreshold)
			{
				threshold = newThreshold;
			}

			void setKnee(SampleType newKneePercentage)
			{
				knee = juce::jmap(newKneePercentage, static_cast<SampleType>(0), static_cast<SampleType>(BDSP_COMPRESSOR_KNEE_MAX));
			}
			void setRatio(SampleType newRatio)
			{
				ratio = newRatio;
			}


			StereoSample<SampleType> applyDynamics(const StereoSample<SampleType>& inputSample, const SampleType& env) override
			{
				gainReduction = calculateGainReduction(env);

				return inputSample * gainReduction;
			}

			SampleType calculateTransferCurve(SampleType x)
			{
				if (x < lowerBoundary)
				{
					return x;
				}
				else if (x < upperBoundary)
				{
					return funcQuad * x * x + funcLinear * x - funcConstant;
				}
				else
				{
					return threshold + invRatio * (x - threshold);
				}
			}

			SampleType calculateGainReduction(SampleType env)
			{
				if (env < lowerBoundary)
				{
					return SampleType(1);
				}
				else if (env > upperBoundary)
				{
					return invRatio;
				}
				else
				{
					return 2 * funcQuad * env + funcLinear;
				}
			}

			void processInternal(bool isBypassed) noexcept override
			{
				calculateTransferCurveConstants();
				DynamicsBase<SampleType>::processInternal(isBypassed);
			}

			SampleType getGainReduction()
			{
				return gainReduction;
			}

		protected:


			void calculateTransferCurveConstants()
			{
				lowerBoundary = threshold - knee;
				upperBoundary = threshold + knee;
				invRatio = 1 / ratio;

				if (knee > 0) // 0 knee doesn't use these variables
				{
					kneeSlope = (invRatio - 1) / (2 * knee);

					funcLinear = 1 - kneeSlope * (threshold - knee);
					funcQuad = kneeSlope / 2;

					funcConstant = 0;

					funcConstant = calculateTransferCurve(lowerBoundary) - lowerBoundary;
				}
			}



			SampleType threshold = 1, knee = BDSP_COMPRESSOR_KNEE_MAX, ratio = 1;
			SampleType invRatio = 1, kneeSlope;
			SampleType funcConstant, funcLinear, funcQuad;
			SampleType lowerBoundary, upperBoundary;

			SampleType gainReduction;
		};

	} // namespace dsp
} // namespace bdsp