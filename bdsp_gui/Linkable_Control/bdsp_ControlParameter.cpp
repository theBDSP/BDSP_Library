#include "bdsp_ControlParameter.h"


namespace bdsp
{

	ControlParameter::ControlParameter(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, const juce::NormalisableRange<float>& normalisableRange, float defaultValue, const juce::AudioParameterFloatAttributes& floatAttributes, int versionHint)
		:Parameter(juce::ParameterID(parameterID, versionHint), parameterName, normalisableRange, defaultValue, floatAttributes)
	{
		init(genericAttributes, layout, controlIDArray, controlNameArray, parameterID, parameterName, versionHint);
	}

	ControlParameter::ControlParameter(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, float defaultValue, const juce::Identifier& attributesID, int versionHint)
		:ControlParameter(genericAttributes, layout, controlIDArray, controlNameArray, parameterID, parameterName, defaultValue, genericAttributes->getFloatAttribute(attributesID), versionHint)
	{
		auto att = genericAttributes->getSampleRateDependentAttribute(attributesID);
		if (att != nullptr)
		{
			att->effectedParameters.add(this);
		}
	}

	ControlParameter::ControlParameter(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, float defaultValue, const FloatParameterAttribute& attribute, int versionHint)
		:ControlParameter(genericAttributes, layout, controlIDArray, controlNameArray, parameterID, parameterName, attribute.range, defaultValue, juce::AudioParameterFloatAttributes().withStringFromValueFunction(attribute.valueToTextLambda).withValueFromStringFunction(attribute.textToValueLambda), versionHint)
	{

	}

	ControlParameter::ControlParameter(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, float defaultValue, SampleRateDependentFloatParameterAttribute* attribute, int versionHint)
		:ControlParameter(genericAttributes, layout, controlIDArray, controlNameArray, parameterID, parameterName, attribute->attribute.range, defaultValue, juce::AudioParameterFloatAttributes().withStringFromValueFunction(attribute->attribute.valueToTextLambda).withValueFromStringFunction(attribute->attribute.textToValueLambda), versionHint)
	{
		attribute->effectedParameters.add(this);
	}










	void ControlParameter::setAPVTS(juce::AudioProcessorValueTreeState* newAPVTS)
	{
		APVTS = newAPVTS;

		for (int i = 0; i < influenceParameters.size(); ++i)
		{
			influenceParameters.set(i, APVTS->getParameter(baseID + controlIDs.getReference(i)));
		}

	}
	void ControlParameter::setControlObjects(juce::OwnedArray<LinkableControl>* newControlObjects)
	{
		for (auto c : *newControlObjects)
		{
			controlObjects.add(c);
		}
	}



	float ControlParameter::getSlotInfluence(int idx)
	{
		return controlObjects.getUnchecked(idx)->value * influenceParameterValues.getUnchecked(idx);
	}

	float ControlParameter::getActualValueNormalized()
	{
		return normalizedValue;

	}

	void ControlParameter::loadInfluenceValues()
	{
		for (int i = 0; i < influenceParameters.size(); ++i)
		{
			auto* p = influenceParameters.getUnchecked(i);
			influenceParameterValues.set(i, p->getNormalisableRange().convertFrom0to1(p->getValue()));
		}
	}




	float ControlParameter::getActualValue()
	{
		return actualValue;
	}



	/*void ControlParameter::addControlLink(LinkableControl* controlToLink)
	{
		controlToLink->addListener(this);
	}

	void ControlParameter::removeControlLink(LinkableControl* controlToUnLink)
	{
		controlToUnLink->removeListener(this);
	}

*/




	juce::RangedAudioParameter* ControlParameter::getControlParameter(const juce::String& linkableID)
	{
		return APVTS->getParameter(linkableID);
	}

	juce::RangedAudioParameter* ControlParameter::getInfluenceParameter(const juce::String& linkableID)
	{
		int i = controlIDs.indexOf(linkableID);
		return influenceParameters[i];

	}

	juce::RangedAudioParameter* ControlParameter::getInfluenceParameter(const int idx)
	{
		return  influenceParameters[idx];
	}

	void ControlParameter::valueChanged()
	{
		calculateValue();
	}

	juce::String ControlParameter::getBaseID()
	{
		return baseID;
	}

	void ControlParameter::setModulationSnapParameter(juce::AudioParameterBool* param)
	{
		modulationSnapParameter = param;
		modulationSnapParameter->addListener(this);
	}

	juce::AudioParameterBool* ControlParameter::getModulationSnapParameter()
	{
		return modulationSnapParameter;
	}

	// mod snap parameter

	void ControlParameter::parameterValueChanged(int parameterIndex, float newValue)
	{
		modulationSnap = !!newValue;
	}

	void ControlParameter::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
	{

	}

	float ControlParameter::convertTo0to1(float v) const noexcept
	{
		const auto& range = getNormalisableRange();
		if (modulationSnap)
		{
			return range.convertTo0to1(range.snapToLegalValue(v));
		}
		else
		{
			return range.convertTo0to1(v);
		}
	}


	float ControlParameter::convertFrom0to1(float v) const noexcept
	{
		const auto& range = getNormalisableRange();
		if (modulationSnap)
		{
			return range.snapToLegalValue(range.convertFrom0to1(juce::jlimit(0.0f, 1.0f, v)));
		}
		else
		{
			return range.convertFrom0to1(juce::jlimit(0.0f, 1.0f, v));
		}

	}



	void ControlParameter::calculateValue()
	{
		float out = getValue();
		for (int i = 0; i < influenceParameters.size(); ++i)
		{
			out += getSlotInfluence(i);
		}

		//out = juce::jlimit(0.0f, 1.0f, out);


		actualValue = convertFrom0to1(out);//getNormalisableRange().snapToLegalValue(getNormalisableRange().convertFrom0to1(out));

		normalizedValue = convertTo0to1(actualValue);
	}

	void ControlParameter::init(GenericParameterAttributes* genericAttributes, juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::StringArray& controlIDArray, const juce::StringArray& controlNameArray, const juce::String& parameterID, const juce::String& parameterName, int versionHint)
	{

		influenceParameters.resize(BDSP_NUMBER_OF_LINKABLE_CONTROLS);
		influenceParameterValues.resize(BDSP_NUMBER_OF_LINKABLE_CONTROLS);
		controlIDs = controlIDArray;


		baseID = paramID.upToLastOccurrenceOf("ID", false, true);





		const auto& percentAtt = genericAttributes->getFloatAttribute("Percent");


		//=====================================================================================================================
		// amount of influence each control unit has on this parameter
		for (int i = 0; i < controlIDs.size(); ++i)
		{
			layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(baseID + controlIDs[i], versionHint), parameterName + " " + controlNameArray[i], juce::NormalisableRange<float>(-1, 1), 0, juce::AudioParameterFloatAttributes().withStringFromValueFunction(percentAtt.valueToTextLambda).withValueFromStringFunction(percentAtt.textToValueLambda)));
		}

	}

















}// namnepace bdsp
