#pragma once


namespace bdsp
{
	namespace dsp
	{
		template <typename T>
		class Lookup_2D
		{
		public:

			Lookup_2D() = default;

			Lookup_2D(const std::function<T(T, T)>& functionToApproximate,
				T minInputValueToUseX,
				T maxInputValueToUseX,
				T minInputValueToUseY,
				T maxInputValueToUseY,
				size_t numPointsX,
				size_t numPointsY)
			{
				initialise(functionToApproximate, minInputValueToUseX, maxInputValueToUseX, minInputValueToUseY, maxInputValueToUseY, numPointsX, numPointsY);
			}


			void initialise(const std::function<T(T, T)>& functionToApproximate,
				T minInputValueToUseX,
				T maxInputValueToUseX,
				T minInputValueToUseY,
				T maxInputValueToUseY,
				size_t numPointsX,
				size_t numPointsY)
			{

				dimX = numPointsX;
				dimY = numPointsY;


				minInputValueX = minInputValueToUseX;
				maxInputValueX = maxInputValueToUseX;

				minInputValueY = minInputValueToUseY;
				maxInputValueY = maxInputValueToUseY;


				scalerX = T(dimX - 1) / (maxInputValueX - minInputValueX);
				offsetX = -minInputValueX * scalerX;

				scalerY = T(dimY - 1) / (maxInputValueY - minInputValueY);
				offsetY = -minInputValueY * scalerY;



				data.resize(dimX * dimY);




				for (size_t x = 0; x < dimX; x++)
				{
					for (size_t y = 0; y < dimY; y++)
					{
						auto unScaledX = (x - offsetX) / scalerX;
						auto unScaledY = (y - offsetY) / scalerY;


						auto value = functionToApproximate(unScaledX, unScaledY);

						jassert(!std::isnan(value));
						jassert(!std::isinf(value));
						// Make sure functionToApproximate returns a sensible value for the entire specified range.
						// E.g., this won't work for zero:  [] (size_t i) { return 1.0f / i; }

						data.getReference(getIndex(x, y)) = value;

					}
				}
				initialized = true;
			}

			//https://en.wikipedia.org/wiki/Bilinear_interpolation
			T processSampleUnchecked(T x, T y) const noexcept
			{


				auto scaledX = scalerX * x + offsetX;
				auto xFloor = static_cast<size_t>(scaledX);
				if (xFloor == dimX - 1)
				{
					xFloor--;
				}

				auto scaledY = scalerY * y + offsetY;
				auto yFloor = static_cast<size_t>(scaledY);
				if (yFloor == dimY - 1)
				{
					yFloor--;
				}


				T xDiff = scaledX - T(xFloor);


				T yDiff = scaledY - T(yFloor);



				T R1 = xDiff * data.getUnchecked(getIndex(xFloor, yFloor)) + (1 - xDiff) * data.getUnchecked(getIndex(xFloor + 1, yFloor));
				T R2 = yDiff * data.getUnchecked(getIndex(xFloor, yFloor + 1)) + (1 - yDiff) * data.getUnchecked(getIndex(xFloor + 1, yFloor + 1));





				return juce::jmap(yDiff, R1, R2);

			}


			T processSample(T x, T y) const noexcept
			{
				auto xLimited = juce::jlimit(minInputValueX, maxInputValueX, x);


				auto yLimited = juce::jlimit(minInputValueY, maxInputValueY, y);


				return processSampleUnchecked(xLimited, yLimited);
			}


			size_t getStepsX()
			{
				return dimX;
			}
			size_t getStepsY()
			{
				return dimY;
			}

			bool isInitialized()
			{
				return initialized;
			}

		private:
			int getIndex(size_t x, size_t y) const
			{
				return x + y * dimX;
			}


			//==============================================================================
			juce::Array<T> data;

			size_t dimX, dimY; // num of points pre-calculated for each variable

			T minInputValueX, maxInputValueX;
			T minInputValueY, maxInputValueY;
			T scalerX, offsetX;
			T scalerY, offsetY;
			bool initialized = false;


			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lookup_2D)
		};
	}// namespace dsp

}//namespace bdsp