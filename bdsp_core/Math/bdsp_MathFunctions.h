#pragma once
namespace bdsp
{
	/**
	 * Returns number farthest from 0 even if it is negative
	 */
	template<typename T>
	inline T absMax(T first, T second)
	{
		return abs(first) > abs(second) ? first : second;
	}

	/**
	 * Calculates a length of time (in milliseconds) from a syncFraction
	 * @param bpm The tempo in beats per minute
	 * @param msValue The value of the time slider
	 * @param fractionValue The value of the tempo fraction slider
	 * @param timeDivision The value of the time divison combo box
	 * @param isRate True if you want to calculate the period of a rate syncFraction
	 * @return Length of time in milliseconds, period of a single cycle in milliseconds if isRate is true
	 */
	inline float calculateTimeInMs(double bpm, float msValue, float fractionValue, int timeDivision, bool isRate)
	{
		if (timeDivision == 0) // Not Synced
		{
			if (isRate)
			{
				return 1000.0 / msValue; // msValue is in Hz (1/Hz = Period)
			}
			else
			{
				return msValue;
			}
		}

		else
		{
			float mult = 1;

			switch (timeDivision)
			{
			case 1: //Straight
				mult = 1.0f;
				break;

			case 2: //Triplet
				mult = 2 / 3.0f;
				break;

			case 3: //Quintuplet
				mult = 4 / 5.0f;
				break;

			case 4: //Septuplet
				mult = 4 / 7.0f;
				break;
			}

			auto wholeNote = 240000.0 / bpm; // ms value of whole note (4 beats * 60s/min * 1000ms/sec / bpm)

			return wholeNote * fractionValue * mult;

		}
	}

	/**
	 * Efficient way to get a power of 2
	 * Be careful not to pass an exponent greater than the number of bits available for the type you're using
	 */
	template <typename intType = unsigned int>
	inline intType quickPow2(intType expononent)
	{
		return intType(1) << expononent;
	}


	/**
	 * Get the signedness of a number
	 * @return -1 if negative, 1 if positive, 0 if zero
	 */
	template<typename T>
	int signum(T num)
	{
		return (T(0) < num) - (num < T(0));
	}

	/**
	 * Checks if a number is between two endpoints, including the two endpoints.
	 * The endpoints do not need to be in order
	 */
	template<typename T>
	bool isBetweenInclusive(T num, T endpoint1, T endpoint2)
	{
		return (endpoint1 <= num && num <= endpoint2) || (endpoint2 <= num && num <= endpoint1);
	}

	/**
	 * Checks if a number is between two endpoints, not including the two endpoints
	 * The endpoints do not need to be in order
	 */
	template<typename T>
	bool isBetweenExclusive(T num, T endpoint1, T endpoint2)
	{
		return (endpoint1 < num && num < endpoint2) || (endpoint2 < num && num < endpoint1);
	}



	/**
	 * Finds the maximum value in a C-style array
	 * @tparam T The type stored in the array - the > operator must be defined
	 * @param arr The array to check
	 * @param size The size of the array to check
	 */
	template <typename T>
	T arrayMax(T* arr, const int& size)
	{
		std::remove_const<T>::type max = arr[0];
		for (int i = 1; i < size; ++i)
		{
			if (arr[i] > max)
			{
				max = arr[i];
			}
		}
		return max;
	}


	/**
	 * Finds the maximum value in a juce array
	 * @tparam T The type stored in the array - the > operator must be defined
	 * @param arr The array to check
	 */
	template <typename T>
	T arrayMax(const juce::Array<T>& arr)
	{
		std::remove_const<T>::type max = arr[0];
		for (int i = 1; i < arr.size(); ++i)
		{
			if (arr[i] > max)
			{
				max = arr[i];
			}
		}
		return max;
	}


	/**
	 * Finds the minimum value in a C-style array
	 * @tparam T The type stored in the array - the < operator must be defined
	 * @param arr The array to check
	 * @param size The size of the array to check
	 */
	template <typename T>
	T arrayMin(T* arr, const int& size)
	{
		std::remove_const<T>::type min = arr[0];
		for (int i = 1; i < size; ++i)
		{
			if (arr[i] < min)
			{
				min = arr[i];
			}
		}
		return min;
	}


	/**
	 * Finds the minimum value in a juce array
	 * @tparam T The type stored in the array - the < operator must be defined
	 * @param arr The array to check
	 */
	template <typename T>
	T arrayMin(juce::Array<T> arr)
	{
		std::remove_const<T>::type min = arr[0];
		for (int i = 1; i < arr.size(); ++i)
		{
			if (arr[i] < min)
			{
				min = arr[i];
			}
		}
		return min;
	}


	/**
	 * Calculates the sum of all elements in a C-Style array
	 * @tparam T The type stored in the array - the + operator must be defined
	 * @param arr The array to check
	 * @param size The size of the array to check
	 */
	template <typename T>
	T arraySum(T* arr, int size)
	{
		std::remove_const<T>::type sum = arr[0];
		for (int i = 1; i < size; ++i)
		{
			sum += arr[i];
		}
		return sum;
	}

	/**
	 * Calculates the sum of all elements in a juce array
	 * @tparam T The type stored in the array - the + operator must be defined
	 * @param arr The array to check
	 * @param size The size of the array to check
	 */
	template <typename T>
	T arraySum(juce::Array<T> arr)
	{
		std::remove_const<T>::type sum = arr[0];
		for (int i = 1; i < arr.size(); ++i)
		{
			sum += arr[i];
		}
		return sum;
	}

	/**
	 * Checks if each element in a given array is unique within the array 
	 * @tparam T The type stored in the array - the == operator must be defined
	 * @param arr The array to check
	 */
	template <typename T>
	bool isArrayDistinct(juce::Array<T> arr)
	{
		arr.sort();
		for (int i = 1; i < arr.size(); ++i)
		{
			if (arr[i - 1] == arr[i])
			{
				return false;
			}
		}
		return true;
	}

} // namespace bdsp