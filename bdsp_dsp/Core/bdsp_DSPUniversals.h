#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * Pool of resources that get used by various different dsp class, like lookup tables, FFTs, etc.
		 * A single instance gets created by the main Audio Processor and a pointer is passed to dsp classes from there.
		 * You should never create another instance elsewhere.
		 */
		template <typename T>
		class DSP_Universals
		{
		public:

		
			/**
			 * Retrieves an already existing or creates a FFT object with a certain order 
			 * @param order The order of the FFT object to return (num of points the object will operate on is 2 ^ order)
			 */
			juce::dsp::FFT* getFFT(int order)
			{

				if (!FFTs.contains(order))
				{
					FFTs.add(order, order);
				}
				return FFTs[order];
			}


			LazyLoad<TrigLookups<T>> trigLookups;
			LazyLoad<DistortionLookups<T>> distortionLookups;
			LazyLoad<PanningLookups<T>> panningLookups;
			LazyLoad<WaveLookups<T>> waveLookups;
			LazyLoad<ChorusLookups<T>> chorusLookups;
		private:
			OwnedMap<int, juce::dsp::FFT> FFTs;
		};
	} // namespace dsp
} // namespace bdsp