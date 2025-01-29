#pragma once 
namespace bdsp
{

	/**
	 * Creates a string representation of the simplified form of a fraction.
	 * Uses the Euclidian algorithm to find a Greatest Common Divisor, which is used to simplify the fraction
	 * https://en.wikipedia.org/wiki/Euclidean_algorithm
	 * @param num The numerator
	 * @param den The denominator
	 * @param seperator The string to place inbetween the numerator and the denominator - defaults to "/"
	 * @param removeDenominatorIfOne If true, only prints the numerator when the fraction simplifies to have a denominator of 1 - defaults to false
	 */
	inline juce::String reduceFraction(const int& num, const int& den, const juce::String& seperator = "/", bool removeDenominatorIfOne = false)
	{
		//================================================================================================================================================================================================
		// find GCD using the Euclidian algorithm, the GCD will be stored in the variable a
		int a = num;
		int b = den;
		while (b != 0)
		{
			int tempB = b;
			b = a % b;
			a = tempB;
		}

		//================================================================================================================================================================================================


		if (removeDenominatorIfOne && den / a == 1)
		{
			return  juce::String(num / a);
		}
		else
		{
			return  juce::String(num / a) + seperator + juce::String(den / a);
		}
	}


	/**
	 * Creates a string representation of a count of things, with correct pluralization.
	 * @param num The number of items
	 * @param singularText The singular form of the item
	 * @param pluralText The plural form of the item - defaults to empty string (adds "s" to singularText)
	 */
	inline juce::String displayCount(const int& num, const juce::String& singularText, const juce::String& pluralText = juce::String())
	{
		if (num == 1)
		{
			return juce::String(num) + " " + singularText;
		}
		else
		{
			auto& plural = pluralText.isEmpty() ? singularText + "s" : pluralText;
			return juce::String(num) + " " + plural;
		}
	}



	/**
	 * Compares string representations of version numbers
	 * @return -1 if v1 < v2, 1 v1>v2, 0 if v1==v2
	 */
	inline int versionStringCompare(const juce::String& v1, const juce::String& v2)
	{
		juce::String currInt;

		auto v1Strings(juce::StringArray::fromTokens(v1, ".", ""));
		auto v2Strings(juce::StringArray::fromTokens(v2, ".", ""));




		int size = juce::jmin(v1Strings.size(), v2Strings.size());
		int out = 0; // defualt to equal
		for (int i = 0; i < size; ++i)
		{
			int v1Int = v1Strings[i].getIntValue();
			int v2Int = v2Strings[i].getIntValue();
			if (v1Int == v2Int)
			{
				continue;
			}
			else
			{
				out = (v1Int < v2Int) ? -1 : 1;
				break;
			}
		}
		return out;

	}


	/**
	 * Creates a unique string that represents a certain string and contains no reserved XML characters.
	 * @param s The string to convert
	 */
	inline juce::String stringToXMLData(const juce::String& s)
	{

		juce::String out;
        for (const auto c : s) // for each character in the string append its integer value preceded by an underscore to the output
		{
			out += '_' + juce::String(int(c));
		}
		return out;
	}

	/**
	 * Reverses a string created by stringToXMLData to get back the original string
	 * @param data The string create by stringToXMLData
	 */
	inline juce::String XMLDataToString(const juce::String& data)
	{
		auto dataStrings(juce::StringArray::fromTokens(data, "_", ""));

		juce::String out;

		for (auto& s : dataStrings) // for each token revert it back to the character it started as
		{
			out += juce::juce_wchar(s.getIntValue());
		}

		return out;
	}

	/**
	 * Converts a single ascii character to a juce::String 
	 * @param asciiChar The character to convert
	 */
	inline juce::String asciiCharToJuceString(unsigned int asciiChar)
	{
		juce::String out;
		out += juce::juce_wchar(asciiChar);
		return out;
	}

} // namespace bdsp
