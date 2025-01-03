#pragma once

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Simple BitCrusher Effect.
		 * Performs both sample rate reduction and bit depth reduction.
		 */
		template <typename SampleType>
		class BitCrushDistortion : public ProcessorChain<SampleType>
		{
		public:
			BitCrushDistortion(DSP_Universals<SampleType>* lookupToUse)
				:crush(lookupToUse, DistortionTypes::BitCrush<SampleType>::Name, 0)
			{
				this->addProcessor(&crush);
				this->addProcessor(&rate);
			}
			~BitCrushDistortion() = default;

			//==============================================================================



			void initBitDepth(SampleType initValue)
			{
				crush.initAmount(initValue);
			}
			void setBitDepth(SampleType newValue)
			{
				crush.setAmount(newValue);
			}

			void setReductionFactor(SampleType newFactor)
			{
				rate.setReductionFactor(newFactor);
			}
		private:

			DistortionBase<SampleType> crush;
			SampleRateReducer<SampleType> rate;

		};



	}// namespace dsp
}// namnepace bdsp


