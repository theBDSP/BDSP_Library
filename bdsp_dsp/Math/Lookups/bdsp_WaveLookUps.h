#pragma once

namespace bdsp
{
	namespace dsp
	{
		
		// holds useful lookup tables to reduce computational load during process block
		template<typename T>
		class WaveLookups
		{
		public:
			WaveLookups()
			{
				generateSinLFOTable();
				generateTriangleLFOTable();
				generateSquareLFOTable();
			}


			~WaveLookups() = default;

	
			inline T getLFOValue(T shape, T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				float num = 3.0f;
				auto SinAmt = juce::jlimit(0.0f, 1.0f, 1 - (num - 1) * shape);
				auto TriAmt = juce::jlimit(0.0f, 1.0f, 1 - abs(1 - (num - 1) * shape));
				auto SqrAmt = juce::jlimit(0.0f, 1.0f, 1 - abs(2 - (num - 1) * shape));
				//T out = SinAmt * SinShapes.processSample(skew, phase) + TriAmt * TriShapes.processSample(skew, phase) + SqrAmt * SqrShapes.processSample(skew, phase);
				T out = SinAmt * SinShapes.processSample(skew, phase) + TriAmt * TriShapes.processSample(skew, phase) + SqrAmt * (phase <= skew ? -1 : 1);

				return amp * (bipolar ? out : (out + 1) / 2.0);
			}

			T lookupSin(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T out = SinShapes.processSample(skew, phase);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}
			T lookupTri(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T out = TriShapes.processSample(skew, phase);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}
			T lookupSqr(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T out = SqrShapes.processSample(skew, phase);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}


			T fastLookupSin(T skew, T phase)
			{
				return SinShapes.processSampleUnchecked(skew, phase);
			}

			T fastLookupTri(T skew, T phase)
			{
				return TriShapes.processSampleUnchecked(skew, phase);
			}

			T fastLookupSqr(T skew, T phase)
			{
				return SqrShapes.processSampleUnchecked(skew, phase);
			}




		private:

			Lookup_2D<T> SinShapes, TriShapes, SqrShapes;

			double delta = 0.001; // difference between sample points

			void generateSinLFOTable()
			{
				if (!SinShapes.isInitialized())
				{

					std::function<T(T, T)> SinShapeFunc = [=](T skew, T phase)
					{
						if ((phase == 0 && skew == 0) || (phase == 1 && skew == 1))
						{
							return T(1);
						}
						else if (phase < skew)
						{
							return sin(PI * phase / skew - PI / 2);
						}
						else
						{
							return -sin(phase * 2 * PI / (2 * (1 - skew)) - PI / 2 - 2 * PI * skew / (2 - 2 * skew));
						}

					};

					SinShapes.initialise(SinShapeFunc, 0, 1, 0, 1, 500, 1 / delta);
				}
			}
			void generateTriangleLFOTable()
			{
				if (!TriShapes.isInitialized())
				{

					std::function<T(T, T)> TriShapeFunc = [=](T skew, T phase)
					{
						if ((phase == 0 && skew == 0) || (phase == 1 && skew == 1))
						{
							return T(1);
						}
						else if (phase < skew)
						{
							return -1 + 2 * phase / skew;
						}
						else
						{
							return 1 - 2 * (phase - skew) / (1 - skew);
						}

					};

					TriShapes.initialise(TriShapeFunc, 0, 1, 0, 1, 500, 1 / delta);
				}
			}
			void generateSquareLFOTable()
			{
				if (!SqrShapes.isInitialized())
				{

					std::function<T(T, T)> SQRShapeFunc = [=](T skew, T phase)
					{
						if (phase <= skew)
						{
							return -1;
						}
						else
						{
							return 1;
						}

					};

					SqrShapes.initialise(SQRShapeFunc, 0, 1, 0, 1, 500, 1 / delta);
				}
			}

			JUCE_LEAK_DETECTOR(WaveLookups)
		};
	}// namespace dsp

}//namespace bdsp
