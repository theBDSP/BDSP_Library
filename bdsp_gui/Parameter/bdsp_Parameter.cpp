#include "bdsp_Parameter.h"

namespace bdsp
{
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultVal, const juce::AudioParameterFloatAttributes& attributes)
		:juce::RangedAudioParameter(parameterID, parameterName, attributes.getAudioProcessorParameterWithIDAttributes()),
		range(normalisableRange),
		value(defaultVal),
		defaultValue(defaultVal),
		valueToStringFunction(attributes.getStringFromValueFunction()),
		stringToValueFunction(attributes.getValueFromStringFunction())
	{
	}
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultVal, const FloatParameterAttribute& attributes)
		:Parameter(parameterID, parameterName, normalisableRange, defaultVal, attributes.toJuceAttributes())
	{
	}
	Parameter::Parameter(const juce::ParameterID& parameterID, const juce::String& parameterName, juce::NormalisableRange<float> normalisableRange, float defaultVal, SampleRateDependentFloatParameterAttribute* attributes)
		:Parameter(parameterID, parameterName, normalisableRange, defaultVal, attributes->attribute.toJuceAttributes())
	{
		attributes->effectedParameters.add(this);
	}

	Parameter::~Parameter()
	{
#if __cpp_lib_atomic_is_always_lock_free
		static_assert (std::atomic<float>::is_always_lock_free,
			"Parameter requires a lock-free std::atomic<float>");
#endif
	}

	void Parameter::updateLambdas(const std::function<juce::String(float, int)>& newValueToString, const std::function<float(const juce::String&)>& newStringToValue, juce::NormalisableRange<float> newRange)
	{
		valueToStringFunction = newValueToString;
		stringToValueFunction = newStringToValue;

		auto prop = range.convertTo0to1(getValue()); // store current normalized value before changing the range
		range = newRange;
		this->operator=(range.convertFrom0to1(prop));

		lambdaListeners.call([&](LambdaListener& l) {l.parameterLambdasUpdated(); });

	}


	const juce::NormalisableRange<float>& Parameter::getNormalisableRange() const
	{
		return range;
	}



	juce::String Parameter::getText(float v, int length) const
	{
		if (valueToStringFunction.operator bool())
		{
			return valueToStringFunction(convertFrom0to1(v), length);
		}
		else
		{
			return juce::String(v, length);
		}
	}



	 float Parameter::getValueForText(const juce::String& text) const
	{
		if (stringToValueFunction.operator bool())
		{
			return convertTo0to1(stringToValueFunction(text));
		}
		else
		{
			return text.getFloatValue();
		}
	}

	float Parameter::getValue() const
	{
		return convertTo0to1(value);
	}

	void Parameter::setValue(float newValue)
	{
		value = convertFrom0to1(newValue);
		valueChanged();
	}

	float Parameter::getDefaultValue() const
	{
		return convertTo0to1(defaultValue);
	}

	Parameter& Parameter::operator= (float newValue)
	{
		if (!juce::approximatelyEqual((float)value, newValue))
		{
			setValueNotifyingHost(convertTo0to1(newValue));
			valueChanged();
		}
		return *this;
	}

} // namespace bdsp