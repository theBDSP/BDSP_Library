#pragma once


namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class PositiveClipDistortion : public DistortionBase<SampleType>
		{
		public:
			PositiveClipDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, oversamplingFactor, handleLatencyCompensationInternally)
			{
                DistortionBase<SampleType>::lookup->generateTables(LookupTables::Distortion, DistortionTypes::PositiveClip);
			}
			~PositiveClipDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getPositiveClipDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


