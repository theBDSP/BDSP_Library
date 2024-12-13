#pragma once
#include "bdsp_DistortionBase.h"

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class SineWaveFoldDistortion : public DistortionBase<SampleType>
		{
		public:
			SineWaveFoldDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 4, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, oversamplingFactor, handleLatencyCompensationInternally)
			{
                DistortionBase<SampleType>::lookup->generateTables(LookupTables::Distortion, DistortionTypes::SineWaveFold);
			}
			~SineWaveFoldDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getSineWaveFoldDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};	
		
		template <typename SampleType>
		class TriWaveFoldDistortion : public DistortionBase<SampleType>
		{
		public:
			TriWaveFoldDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 4, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, oversamplingFactor, handleLatencyCompensationInternally)
			{
                DistortionBase<SampleType>::lookup->generateTables(LookupTables::Distortion, DistortionTypes::TriWaveFold);
			}
			~TriWaveFoldDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getTriWaveFoldDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


