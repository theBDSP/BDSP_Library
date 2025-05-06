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

				T p = getSkewedPhase(skew, phase);
				T out = SinAmt * SinShapes.processSample(p) + TriAmt * TriShapes.processSample(p) + SqrAmt * SqrShapes.processSample(p);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}

			T lookupSin(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T p = getSkewedPhase(skew, phase);
				T out = SinShapes.processSample(p);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}
			T lookupTri(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T p = getSkewedPhase(skew, phase);
				T out = TriShapes.processSample(p);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}
			T lookupSqr(T skew, T phase, bool bipolar = true, T amp = T(1))
			{
				T p = getSkewedPhase(skew, phase);
				T out = SqrShapes.processSample(p);
				return amp * (bipolar ? out : (out + 1) / 2.0);
			}


			T fastLookupSin(T skew, T phase)
			{
				T p = getSkewedPhase(skew, phase);
				return SinShapes.processSampleUnchecked(p);
			}

			T fastLookupTri(T skew, T phase)
			{
				T p = getSkewedPhase(skew, phase);
				return TriShapes.processSampleUnchecked(p);
			}

			T fastLookupSqr(T skew, T phase)
			{
				T p = getSkewedPhase(skew, phase);
				return SqrShapes.processSampleUnchecked(p);
			}




		private:

			juce::dsp::LookupTableTransform<T> SinShapes, TriShapes, SqrShapes;

			double delta = 0.001; // difference between sample points

			void generateSinLFOTable()
			{
				std::function<T(T)> SinShapeFunc = [=](T phase)
				{
					return sin(2 * PI * phase - PI / 2);
				};

				SinShapes.initialise(SinShapeFunc, 0, 1, 1 / delta);
			}

			void generateTriangleLFOTable()
			{
				std::function<T(T)> TriShapeFunc = [=](T phase)
				{
					return 1 - 2 * abs(2 * (1 - phase) - 1);
				};

				TriShapes.initialise(TriShapeFunc, 0, 1, 1 / delta);

			}
			void generateSquareLFOTable()
			{

				std::function<T(T)> SQRShapeFunc = [=](T phase)
				{
					if (phase <= 0.5)
					{
						return -1;
					}
					else
					{
						return 1;
					}

				};

				SqrShapes.initialise(SQRShapeFunc, 0, 1, 1 / delta);
			}

			T getSkewedPhase(T skew, T phase)
			{
				if (phase < skew)
				{
					return phase / (2 * skew + FLT_MIN);
				}
				else
				{
					return (phase - skew) / (2 * (1 - skew + FLT_MIN)) + 0.5;
				}
			}

			JUCE_LEAK_DETECTOR(WaveLookups)
		};
	}// namespace dsp

}//namespace bdsp
