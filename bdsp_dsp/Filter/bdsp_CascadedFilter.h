#pragma once
namespace bdsp
{
	namespace dsp
	{
		
		/**
		 * A class that wraps a specific filter class and creates a higher order version of it.
		 * @tparam order The number of identical filters to use in processing to create a higher order filter.
		 * Note this does not necessarily exactly equal the actual order of the resulting filter, the order of the resulting filter will be order * the order of the derived filter class.
		 * i.e. A BiQuad filter (2nd order) with an order of 2 will result in a final filter of order 4.
		 */

		template <typename SampleType, class FilterType, int order = 1>
		class CascadedFilter : public FilterType
		{
		public:

			template<class ... Types>
			CascadedFilter(Types...args)
				:FilterType(args...)
			{

			}

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				juce::dsp::ProcessSpec multiSpec{ spec.sampleRate,spec.maximumBlockSize, order * spec.numChannels }; // repeated filters processed in dummy channels, can't reuse same channel because that would affect the values of previous samples and interfere with calculations.
				FilterType::prepare(multiSpec);
				channels = spec.numChannels;
			}


			juce::dsp::Complex<SampleType> calculateResponseForFrequency(BaseFilterParameters<SampleType>* paramsToUse, SampleType frequency) override
			{
				return pow(FilterType::calculateResponseForFrequency(paramsToUse, frequency), SampleType(order)); // response of series process is multiplication of responses
			}

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				SampleType out = FilterType::processSample(channel, inputSample); // result of first filter

				for (int i = 1; i < order; ++i) // send through repeated filters
				{
					out = FilterType::processSample(i * channels + channel, out);
				}
				return out;
			}

		private:
			int channels;
		};
	} // namespace dsp
} // namespace bdsp
