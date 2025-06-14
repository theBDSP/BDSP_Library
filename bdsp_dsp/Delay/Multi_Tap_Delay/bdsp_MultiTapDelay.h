#pragma once
namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType, typename DelayType = DelayTypes::Basic, typename InterpolationType = DelayLineInterpolationTypes::Lagrange3rd>
		class MultiTapDelay : public DelayLine<SampleType, DelayType, InterpolationType>
		{
		public:
			MultiTapDelay(int maxDelayInSamples, DSP_Universals<SampleType>* lookupToUse = nullptr);

			SampleType popSample(int channel) override;

			void setTap(int channel, int tapNumber, SampleType delay);
			SampleType getTap(int channel, int tapNumber);
			int getChannel(int channel) const override;

		protected:
			std::vector<SampleType> tapOutputs; // last sample from each tap for use in derived process sample calls

		};



	} //namespace dsp
} //namespace bdsp