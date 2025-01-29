#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * A Lookup table that stores the pre-calculated values that depend on 2 seperate variables.
		 * Intermediate values are approximated using bilinear interpolation.
		 * https://en.wikipedia.org/wiki/Bilinear_interpolation
		 */
		template <typename T>
		class Lookup_2D
		{
		public:

			/**
			 * Creates and empty lookup table. This will be useless until initialized with a function.
			 */
			Lookup_2D() = default;


			/**
			 * Creates a lookup table that pre-calculates a given function of 2 variables
			 * @param functionToApproximate The function to pre calculate
			 * @param minInputValueToUseX The minimum input value expected for the first variable of the function
			 * @param maxInputValueToUseX The maximum input value expected for the first variable of the function
			 * @param minInputValueToUseY The minimum input value expected for the second variable of the function
			 * @param maxInputValueToUseY The maximum input value expected for the second variable of the function
			 * @param numPointsX The number of values to pre-calculate per value of the second variable
			 * @param numPointsY The number of values to pre-calculate per value of the first variable
			 */
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


			/**
			 * Pre-calculates a given function of 2 variables
			 * @param functionToApproximate The function to pre calculate
			 * @param minInputValueToUseX The minimum input value expected for the first variable of the function
			 * @param maxInputValueToUseX The maximum input value expected for the first variable of the function
			 * @param minInputValueToUseY The minimum input value expected for the second variable of the function
			 * @param maxInputValueToUseY The maximum input value expected for the second variable of the function
			 * @param numPointsX The number of values to pre-calculate per value of the second variable
			 * @param numPointsY The number of values to pre-calculate per value of the first variable
			 */
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



				//================================================================================================================================================================================================
				// pre-calculate each point
				for (size_t x = 0; x < dimX; x++)
				{
					for (size_t y = 0; y < dimY; y++)
					{
						auto value = functionToApproximate(convertXIndexToInput(x), convertYIndexToInput(y));

						/* Make sure functionToApproximate returns a sensible value for the entire specified range */
						jassert(!std::isnan(value));
						jassert(!std::isinf(value));

						data.getReference(getIndex(x, y)) = value;

					}
				}
				//================================================================================================================================================================================================
				initialized = true;
			}

			/**
			* Approximates a value from 2 given variable values.
			* This will not check that these variables are in the expected range.
			* If you are unsure if your inputs are in range call processSample instead.
			*/
			T processSampleUnchecked(T x, T y) const noexcept
			{

				T xIdx = convertXInputToIndex(x);
				size_t x1 = floor(xIdx);
				size_t x2 = ceil(xIdx);


				T yIdx = convertYInputToIndex(y);
				size_t y1 = floor(yIdx);
				size_t y2 = ceil(yIdx);


				if (x1 == x2 && y1 == y2) // trivial case where both inputs exactly match a pre-calculated value
				{
					return getValue(x1, y1);
				}
				else if (x1 == x2) // case where only the x-input matches a pre-calculated x-input
				{
					return juce::jmap(yIdx - y1, getValue(x1, y1), getValue(x1, y2));
				}
				else if (y1 == y2) // case where only the y-input matches a pre-calculated y-input
				{
					return juce::jmap(xIdx - x1, getValue(x1, y1), getValue(x2, y1));
				}
				else // desired value lies between a set of 4 pre-calculated values
				{
					T endpoint1 = juce::jmap(xIdx - x1, getValue(x1, y1), getValue(x2, y1));
					T endpoint2 = juce::jmap(xIdx - x1, getValue(x1, y2), getValue(x2, y2));

					return juce::jmap(yIdx - y1, endpoint1, endpoint2);
				}
			}


			/**
			 * Approximates a value from 2 given variable values.
			 * This will limit these values to the expected range.
			 */
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
			size_t getIndex(size_t x, size_t y) const
			{
				return x + y * dimX;
			}

			T getValue(size_t x, size_t y) const
			{
				return data[getIndex(x, y)];
			}

			T convertXInputToIndex(T xInput) const
			{
				return scalerX * xInput + offsetX;
			}
			T convertYInputToIndex(T yInput) const
			{
				return scalerY * yInput + offsetY;
			}

			T convertXIndexToInput(size_t xIndex) const
			{
				return (xIndex - offsetX) / scalerX;
			}
			T convertYIndexToInput(size_t yIndex) const
			{
				return (yIndex - offsetY) / scalerY;
			}

			//==============================================================================
			juce::Array<T> data;

			size_t dimX, dimY; // num of points pre-calculated for each variable

			T minInputValueX, maxInputValueX;
			T minInputValueY, maxInputValueY;

			T scalerX, scalerY; // Multiplies the input to convert to an index in the data array
			T offsetX, offsetY; // Multiplies the input to convert to an index in the data array

			bool initialized = false;


			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lookup_2D)
		};
	}// namespace dsp

}//namespace bdsp
