#pragma once 


namespace bdsp
{

	/**
	 * A parameter class mostly the same as juce::AudioParameterFloat, but with public access to certain private members
	 */
	class Parameter : public juce::RangedAudioParameter
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


		virtual ~Parameter();

		/**
		 * Changes the conversion functions between value and string, and sets a new normalisable range.
		 */
		void updateLambdas(const std::function<juce::String(float, int)>& newValueToString, const std::function<float(const juce::String&)>& newStringToValue, juce::NormalisableRange<float> newRange);


		/**
		 * Returns the value of this parameter accounting for all extrernal factors (only modulation at this point). Override this method to account for external factors.
		 * @return
		 */
		virtual float getActualValue()
		{
			return getValue();
		}



		//================================================================================================================================================================================================
		// Inherited via RangedAudioParameter

		/**
		 * @return The normalized value of the parameter
		 */
		float getValue() const override;

		/**
		 * Sets the value of this parameter
		 * @param newValue The normalized value to set the parameter to
		 */
		void setValue(float newValue) override;

		/**
		 * @return The normalized default value
		 */
		float getDefaultValue() const override;

		const juce::NormalisableRange<float>& getNormalisableRange() const override;

		/**
		 * @return The text generated from the conversion function (or a backup function, if the conversion is not set) given a certain value and maximum length for the output
		 */
		juce::String getText(float v, int length) const override;

		/**
		 * @return The value generated from the conversion function (or a backup function, if the conversion is not set) given a certain string
		 */
		float getValueForText(const juce::String& text) const override;
		//================================================================================================================================================================================================

		/**
		 * Override this method if you are interested in receiving callbacks when the parameter value changes.
		 */
		virtual void valueChanged()
		{

		}

		/**
		 * Sets the value of this parameter
		 * @param newValue The non-normalized value to set the parameter to
		 */
		Parameter& operator=(float newValue);


		/**
		 * A listener class specifically to get notified when a parameter's string conversion lambdas or range changes.
		 * To get notified about changes to a parameter's value, instead derive from juce::RangedAudioParameter::Listener
		 */
		class LambdaListener
		{
		public:
            virtual ~LambdaListener()=default;

			virtual void parameterLambdasUpdated() = 0;
		};

		void addLambdaListener(LambdaListener* listenerToAdd)
		{
			lambdaListeners.add(listenerToAdd);
		}

		void removeLambdaListener(LambdaListener* listenerToRemove)
		{
			lambdaListeners.remove(listenerToRemove);
		}


		juce::NormalisableRange<float> range;
		std::function<juce::String(float, int)> valueToStringFunction;
		std::function<float(const juce::String&)> stringToValueFunction;
	private:
		juce::ListenerList<LambdaListener> lambdaListeners;

		std::atomic<float> value;
		float defaultValue = 0.0f;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)


	};
} // namespace bdsp
