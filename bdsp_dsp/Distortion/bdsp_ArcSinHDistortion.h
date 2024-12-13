#pragma once
#include "bdsp_DistortionBase.h"

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class ArcSinHDistortion : public DistortionBase<SampleType>
		{
		public:
			ArcSinHDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, DistortionTypes::ArcSinH, oversamplingFactor, handleLatencyCompensationInternally)
			{
			}
			~ArcSinHDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
                SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getArcSinHDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


