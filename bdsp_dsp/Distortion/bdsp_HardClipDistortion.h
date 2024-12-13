#pragma once
#include "bdsp_DistortionBase.h"

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class HardClipDistortion : public DistortionBase<SampleType>
		{
		public:
			HardClipDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 4, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, DistortionTypes::HardClip, oversamplingFactor, handleLatencyCompensationInternally)
			{
			}
			~HardClipDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getHardClipDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


