#pragma once
#include "bdsp_DistortionBase.h"

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class ArcTanDistortion : public DistortionBase<SampleType>
		{
		public:
			ArcTanDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, oversamplingFactor, handleLatencyCompensationInternally)
			{
				lookupToUse->distortionLookups->generateTables(DistortionTypes::ArcTan);
			}
			~ArcTanDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getArcTanDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


