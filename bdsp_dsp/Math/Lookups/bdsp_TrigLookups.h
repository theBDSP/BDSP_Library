#pragma once

namespace bdsp
{
	namespace dsp
	{

		// holds useful lookup tables to reduce computational load during process block
		template<typename T>
		class TrigLookups
		{
		public:
			TrigLookups()
			{
				generateLanczosTable();
			}


			~TrigLookups() = default;


			inline T limtToPlusMinusPI(T x)
			{
				return fmod(x, PI);
			}

			T sin(T x, bool limit = false)
			{
				if (limit)
				{
					x = limtToPlusMinusPI(x);
				}
				return juce::dsp::FastMathApproximations::sin(x);
			}


			T cos(T x, bool limit = false)
			{
				if (limit)
				{
					x = limtToPlusMinusPI(x);
				}
				return juce::dsp::FastMathApproximations::cos(x);
			}

			inline T lanczos(T x)
			{
				return LanczosLookUp.processSample(x);
			}

			inline int getLanczosA()
			{
				return a;
			}

		private:
			juce::dsp::LookupTableTransform<T> LanczosLookUp;
			bool lanczosInitialized = false;
			int a = 3;
			T delta = 0.001;

			void generateLanczosTable()
			{
				if (!lanczosInitialized)
				{


					const std::function<T(T)> LanczosFunc = [=](T x)
					{
						T out = 0;
						if (x == 0)
						{
							out = 1;
						}
						else if (-a <= x && x < a)
						{
							out = a * std::sin(PI * x) * std::sin(PI * x / a) / (PI * PI * x * x);
						}

						return out;
					};

					LanczosLookUp.initialise(LanczosFunc, -a, a + 1, (2 * a + 1) / delta);
					lanczosInitialized = true;
				}
			}


			JUCE_LEAK_DETECTOR(TrigLookups)
		};
	}// namespace dsp

}//namespace bdsp
