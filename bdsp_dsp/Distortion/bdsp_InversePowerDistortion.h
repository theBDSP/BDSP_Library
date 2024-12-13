#pragma once


namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class InversePowerDistortion : public DistortionBase<SampleType>
		{
		public:
			InversePowerDistortion(DSP_Universals<SampleType>* lookupToUse, int oversamplingFactor = 2, bool handleLatencyCompensationInternally = true)
				:DistortionBase<SampleType>(lookupToUse, DistortionTypes::InversePower, oversamplingFactor, handleLatencyCompensationInternally)
			{
			}
			~InversePowerDistortion() = default;

			//==============================================================================




		protected:


			// dont call this directly or oversamlping will be skipped
			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();
				return  DistortionBase<SampleType>::lookup->getInversePowerDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled);
			}

		};



	}// namespace dsp
}// namnepace bdsp


