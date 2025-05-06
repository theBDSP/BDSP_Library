#pragma once



namespace bdsp
{
	class ControlParameter : public Parameter, public juce::AudioParameterBool::Listener
	{
	public:

		ControlParameter(GenericParameterAttributes* genericAttributes,
			juce::AudioProcessorValueTreeState::ParameterLayout& layout,
			const juce::StringArray& controlIDArray,
			const juce::StringArray& controlNameArray,
			const juce::String& parameterID,
			const juce::String& parameterName,
			const juce::NormalisableRange<float>& normalisableRange,
			float defaultValue,
			const juce::AudioParameterFloatAttributes& floatAttributes,
			int versionHint = 1);


		ControlParameter(GenericParameterAttributes* genericAttributes,
			juce::AudioProcessorValueTreeState::ParameterLayout& layout,
			const juce::StringArray& controlIDArray,
			const juce::StringArray& controlNameArray,
			const juce::String& parameterID,
			const juce::String& parameterName,
			float defaultValue,
			const juce::Identifier& attributesID,
			int versionHint = 1);

		ControlParameter(GenericParameterAttributes* genericAttributes,
			juce::AudioProcessorValueTreeState::ParameterLayout& layout,
			const juce::StringArray& controlIDArray,
			const juce::StringArray& controlNameArray,
			const juce::String& parameterID,
			const juce::String& parameterName,
			float defaultValue,
			const FloatParameterAttribute& attribute,
			int versionHint = 1);


		ControlParameter(GenericParameterAttributes* genericAttributes,
			juce::AudioProcessorValueTreeState::ParameterLayout& layout,
			const juce::StringArray& controlIDArray,
			const juce::StringArray& controlNameArray,
			const juce::String& parameterID,
			const juce::String& parameterName,
			float defaultValue,
			SampleRateDependentFloatParameterAttribute* attribute,
			int versionHint = 1);

		~ControlParameter() = default;

		void setAPVTS(juce::AudioProcessorValueTreeState* newAPVTS);

		void setControlObjects(juce::OwnedArray<LinkableControl>* newControlObjects);

		float getSlotInfluence(int idx);
		float getActualValue() override;
		float getActualValueNormalized();


		void loadInfluenceValues();



		//void addControlLink(LinkableControl* controlToLink);
		//void removeControlLink(LinkableControl* controlToUnLink);





		juce::RangedAudioParameter* getControlParameter(const juce::String& linkableID);
		juce::RangedAudioParameter* getInfluenceParameter(const juce::String& linkableID);
		juce::RangedAudioParameter* getInfluenceParameter(const int idx);

		void valueChanged() override;

		juce::String getBaseID();


		juce::StringArray controlIDs;


		void setModulationSnapParameter(juce::AudioParameterBool* param);
		juce::AudioParameterBool* getModulationSnapParameter();

		// mod snap parameter
		void parameterValueChanged(int parameterIndex, float newValue) override;

		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting)override;
	private:


		juce::String baseID;


		juce::AudioProcessorValueTreeState* APVTS = nullptr;
		juce::Array<LinkableControl*> controlObjects;

		float normalizedValue; // normalized b/w 0 and 1
		float actualValue;

		juce::Array<juce::RangedAudioParameter*> influenceParameters;
		juce::Array<float> influenceParameterValues;


		float convertTo0to1(float v) const noexcept;

		float convertFrom0to1(float v) const noexcept;



		void calculateValue();

		void swapValues(Parameter& other)
		{
			auto cast = dynamic_cast<ControlParameter*>(&other);

			swapParameterValues(*this, other);
			if (cast != nullptr)
			{
				for (int i = 0; i < influenceParameters.size(); ++i)
				{
					swapParameterValues(influenceParameters[i], cast->influenceParameters[i]);
				}

			}

		}


		bool modulationSnap = true;

		juce::AudioParameterBool* modulationSnapParameter = nullptr;



		void init(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, int versionHint);
	};



}// namnepace bdsp
