#pragma once 


namespace bdsp
{

	class Parameter : public juce::AudioParameterFloat
	{
	public:


		Parameter(const juce::ParameterID& parameterID,
			const juce::String& parameterName,
			juce::NormalisableRange<float> normalisableRange,
			float defaultValue,
			const juce::AudioParameterFloatAttributes& attributes = {});

		Parameter(const juce::ParameterID& parameterID,
			const juce::String& parameterName,
			juce::NormalisableRange<float> normalisableRange,
			float defaultValue,
			const FloatParameterAttribute& attributes = FloatParameterAttribute());


		Parameter(const juce::ParameterID& parameterID,
			const juce::String& parameterName,
			juce::NormalisableRange<float> normalisableRange,
			float defaultValue,
			SampleRateDependentFloatParameterAttribute* attributes);



		/** Creates a AudioParameterFloat with an ID, name, and range.
			On creation, its value is set to the default value.
			For control over skew factors, you can use the other
			constructor and provide a NormalisableRange.
		*/
		Parameter(const juce::ParameterID& parameterID,
			const juce::String& parameterName,
			float minValue,
			float maxValue,
			float defaultValue);


		void updateLambdas(const std::function<juce::String(float, int)>& newLambda, const std::function<float(const juce::String&)>& newLambdaText, juce::NormalisableRange<float> newRange);



		virtual float getActualValue()
		{
			return get();
		}

		float getDefault() // is not private like the juce APF funciton
		{
			return dynamic_cast<AudioProcessorParameter*>(this)->getDefaultValue();
		}

		juce::String getText(float v, int length) const override
		{
			return valueToStringFunction(convertFrom0to1(v), length);
		}
		float getValueForText(const juce::String& text) const override
		{
			return convertTo0to1(stringToValueFunction(text));
		}
	private:
		// References to the lambdas owned by the audio parameter float base class which are private
		std::function<juce::String(float, int)> valueToStringFunction;
		std::function<float(const juce::String&)> stringToValueFunction;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
	};
} // namespace bdsp