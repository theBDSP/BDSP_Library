#pragma once


namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class TanHPinchDistortion : public DistortionBase<SampleType>
		{
		public:
			TanHPinchDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, oversamplingFactor, handleLatencyCompensationInternally)
			{
                DistortionBase<SampleType>::lookup->generateTables(LookupTables::Distortion, DistortionTypes::TanHPinch);
			}
			~TanHPinchDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getTanHPinchDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


