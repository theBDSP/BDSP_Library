#include "bdsp_Parameter.h"

namespace bdsp
{
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultValue, const juce::AudioParameterFloatAttributes& attributes)
		:juce::AudioParameterFloat(parameterID, parameterName, normalisableRange, defaultValue, attributes),
		valueToStringFunction(attributes.getStringFromValueFunction()),
		stringToValueFunction(attributes.getValueFromStringFunction())
	{
	}
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultValue, const FloatParameterAttribute& attributes)
		:juce::AudioParameterFloat(parameterID, parameterName, normalisableRange, defaultValue, attributes.toJuceAttributes()),
		valueToStringFunction(attributes.valueToTextLambda),
		stringToValueFunction(attributes.textToValueLambda)
	{
	}
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultValue, SampleRateDependentFloatParameterAttribute* attributes)
		:juce::AudioParameterFloat(parameterID, parameterName, normalisableRange, defaultValue, attributes->attribute.toJuceAttributes()),
		valueToStringFunction(attributes->attribute.valueToTextLambda),
		stringToValueFunction(attributes->attribute.textToValueLambda)
	{
		attributes->effectedParameters.add(this);
	}
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, float minValue, float maxValue, float defaultValue)
		:juce::AudioParameterFloat(parameterID, parameterName, minValue, maxValue, defaultValue)
	{
	}
	void Parameter::updateLambdas(const std::function<juce::String(float, int)>& newLambda, const std::function<float(const juce::String&)>& newLambdaText, juce::NormalisableRange<float> newRange)
	{
		valueToStringFunction = newLambda;
		stringToValueFunction = newLambdaText;
		auto prop = range.convertTo0to1(get());
		range = newRange;
		juce::AudioParameterFloat::operator=(range.convertFrom0to1(prop));
	}

} // namespace bdsp