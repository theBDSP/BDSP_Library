#pragma once


namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType>
		class MultiSignalProcessorBase : public BaseProcessingUnit<SampleType>
		{

		public:
			MultiSignalProcessorBase(int numExtraSignals)
			{
				numSignals = numExtraSignals;
			}


			template<class ContextType>
			void process(const ContextType& context, juce::Array<juce::dsp::AudioBlock<const SampleType>>& extraInputs)
			{
				for (int i = 0; i < extraInputs.size() && i < numSignals; ++i)
				{
					inputBlocks.set(i, &extraInputs[i]);
				}

				BaseProcessingUnit<SampleType>::process(context);
			}

		protected:
			juce::Array<juce::dsp::AudioBlock<const SampleType>*> inputBlocks; // extra inputs used in processing
			int numSignals = 1;

		};
	} // namespace dsp
} // namespace bdsp
