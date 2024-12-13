#pragma once
namespace bdsp
{

	/**
	 * Checks if a flag is set in a certain combination of flags
	 * This only works for flag values that are all powers of 2
	 * i.e. @code
	 * enum flag: int{flag1 = 1, flag2 = 2, flag3 = 4};
	 * @endcode
	 *
	 * @tparam flagType The underlying type of the flag (the enum-base for enums)
	 * @param flagToTest The combination of flags being tested
	 * @param flagToTestFor The flag to test for
	 * @return True if the flag is set in flagToTest
	 */
	template <typename flagType = unsigned int>
	inline bool testNonConsecutiveFlag(flagType flagToTest, flagType flagToTestFor)
	{
		return flagToTest & flagToTestFor;
	}


	/**
	 * Checks if a flag is set in a certain combination of flags
	 * This only works for flag values that are consecutive
	 * i.e. @code
	 * enum flag: int{flag1, flag2, flag3};
	 * @endcode
	 *
	 * @tparam flagType The underlying type of the flag (the enum-base for enums)
	 * @param flagToTest The combination of flags being tested
	 * @param flagToTestFor The flag to test for
	 * @return True if the flag is set in flagToTest
	 */
	template <typename flagType = unsigned int>
	inline bool testConsecutiveFlag(flagType flagToTest, flagType flagToTestFor)
	{
		return (flagType(1) << flagToTest) & (flagType(1) << flagToTestFor);
	}



	/**
	 * Returns an array with every flag set in a certain combination of flags
	 * This only works for flag values that are all powers of 2
	 * i.e. @code
	 * enum flag: int{flag1 = 1, flag2 = 2, flag3 = 4};
	 * @endcode
	 *
	 * @tparam flagType The underlying type of the flag (the enum-base for enums)
	 * @param flags The combination of flags being tested
	 */
	template <typename flagType = unsigned int>
	inline juce::Array<flagType> getAllSetNonConsecutiveFlags(flagType flags)
	{
		juce::Array<flagType> out;

		for (flagType i = 0; i < flags; ++i)
		{
			if (testNonConsecutiveFlag(flags, i))
			{
				out.add(i);
			}
		}
		return out;
	}


	/**
	 * Returns an array with every flag set in a certain combination of flags
	 * This only works for flag values that are consecutive
	 * i.e. @code
	 * enum flag: int{flag1, flag2, flag3};
	 * @endcode
	 *
	 * @tparam flagType The underlying type of the flag (the enum-base for enums)
	 * @param flags The combination of flags being tested
	 */
	template <typename flagType = unsigned int>
	inline juce::Array<flagType> getAllSetConsecutiveFlags(flagType flags)
	{
		juce::Array<flagType> out;

		for (flagType i = 0; i < flags; ++i)
		{
			if (testConsecutiveFlag(flags, i))
			{
				out.add(i);
			}
		}
		return out;
	}


} // namespace bdsp