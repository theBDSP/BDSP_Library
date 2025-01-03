#pragma once
namespace bdsp
{
	namespace HashFunctions
	{

		// Struct to provide hashing functions to use juce::String as a key in standard library maps and juce HashMaps
		struct String
		{
			std::size_t operator() (const juce::String& s) const noexcept
			{
				return std::hash<std::string>{}(s.toStdString());
			}

			int generateHash(const juce::String& key, int upperLimit) const
			{
				return juce::DefaultHashFunctions::generateHash(key, upperLimit);
			}
		};


		// Struct to provide hashing functions to use juce::Identifier as a key in standard library maps and juce HashMaps
		struct Identifier
		{
			std::size_t operator() (const juce::Identifier& id) const noexcept
			{
				return std::hash<std::string>{}(id.toString().toStdString());
			}

			int generateHash(const juce::Identifier& key, int upperLimit) const
			{
				return juce::DefaultHashFunctions::generateHash(key.toString(), upperLimit);
			}
		};




	}
}