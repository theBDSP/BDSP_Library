#pragma once

namespace bdsp
{
	namespace dsp
	{
		template <typename SampleType>
		class ParametricEQ : public ProcessorChain<SampleType>
		{
		public:

			ParametricEQ(DSP_Universals<SampleType>* lookupsToUse, int numBands, bool hasLowShelf = true, bool hasHighShelf = true)
			{

				if (hasLowShelf)
				{
					lowShelf = std::make_unique <BiQuadFilters::LowShelf<SampleType>>(lookupsToUse);
					ProcessorChain<SampleType>::addProcessor(lowShelf.get());

				}
				for (int i = 0; i < numBands; ++i)
				{
					peakBands.add(new BiQuadFilters::Peaking<SampleType>(lookupsToUse));
					ProcessorChain<SampleType>::addProcessor(peakBands.getLast());
				}
				if (hasHighShelf)
				{
					highShelf = std::make_unique <BiQuadFilters::HighShelf<SampleType>>(lookupsToUse);
					ProcessorChain<SampleType>::addProcessor(highShelf.get());
				}

			}



			FilterBase<SampleType>* getBand(int i)
			{
				return dynamic_cast<FilterBase<SampleType>*>(ProcessorChain<SampleType>::getProcessor(i));
			}

			BiQuadFilters::Peaking<SampleType>* getPeakBand(int i)
			{
				return peakBands[i];
			}

			BiQuadFilters::LowShelf<SampleType>* getLowShelf()
			{
				return lowShelf.get();
			}

			BiQuadFilters::HighShelf<SampleType>* getHighShelf()
			{
				return highShelf.get();
			}



		private:
			juce::OwnedArray<BiQuadFilters::Peaking<SampleType>> peakBands;
			std::unique_ptr <BiQuadFilters::LowShelf<SampleType>> lowShelf;
			std::unique_ptr <BiQuadFilters::HighShelf<SampleType>> highShelf;


		};

	} // namespace dsp
} // namespace bdsp
