#pragma once


namespace bdsp
{
	namespace dsp
	{
	
		// holds useful lookup tables to reduce computational load during process block
		template<typename T>
		class PanningLookups
		{
		public:
			PanningLookups()
			{
				generatePanTables();
			}

			virtual ~PanningLookups() = default;


			//================================================================================================================================================================================================

			inline T getPanLL(T x)
			{
				return PanLLLookup.processSampleUnchecked(x);
			}

			inline T getPanLR(T x)
			{
				return PanLRLookup.processSampleUnchecked(x);
			}

			inline T getPanRL(T x)
			{
				return PanRLLookup.processSampleUnchecked(x);
			}

			inline T getPanRR(T x)
			{
				return PanRRLookup.processSampleUnchecked(x);
			}

			
		private:

			bool panTablesInitialized = false;

			juce::dsp::LookupTableTransform<T> PanLLLookup;
			juce::dsp::LookupTableTransform<T> PanLRLookup;
			juce::dsp::LookupTableTransform<T> PanRLLookup;
			juce::dsp::LookupTableTransform<T> PanRRLookup;

			double delta = 0.001; // difference between sample points


			void generatePanTables()
			{
				if (!panTablesInitialized)
				{
					auto scalar = 4.592;

					const std::function<T(T)> LLFunc = [=](T x)
					{
						return x < 0 ? ((scalar - 1) + std::cos(PI * x / 2)) / scalar : std::cos(PI * x / 2);
					};
					PanLLLookup.initialise(LLFunc, -1, 1, 2 / delta);

					const std::function<T(T)> LRFunc = [=](T x)
					{
						return x > 0 ? std::sin(PI * x / scalar) : 0;

					};
					PanLRLookup.initialise(LRFunc, -1, 1, 2 / delta);


					const std::function<T(T)> RLFunc = [=](T x)
					{
						return x < 0 ? -std::sin(PI * x / scalar) : 0;
					};
					PanRLLookup.initialise(RLFunc, -1, 1, 2 / delta);

					const std::function<T(T)> RRFunc = [=](T x)
					{
						return x > 0 ? ((scalar - 1) + std::cos(PI * x / 2)) / scalar : std::cos(PI * x / 2);
					};
					PanRRLookup.initialise(RRFunc, -1, 1, 2 / delta);

					panTablesInitialized = true;
				}
			}


			JUCE_LEAK_DETECTOR(PanningLookups)
		};
	}// namespace dsp

}//namespace bdsp
