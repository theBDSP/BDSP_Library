#pragma once



#ifndef BDSP_RATE_MIN
#define BDSP_RATE_MIN -5
#endif // !BDSP_RATE_MIN

#ifndef BDSP_RATE_MAX
#define BDSP_RATE_MAX 4
#endif // !BDSP_RATE_MAX

namespace bdsp
{

	struct FloatParameterAttribute
	{
		FloatParameterAttribute()
			:FloatParameterAttribute(juce::Identifier(), std::function<juce::String(float, int)>(), std::function<float(const juce::String&)>(), juce::NormalisableRange<float>())
		{

		}

		FloatParameterAttribute(const juce::Identifier& id, const std::function<juce::String(float, int)>& V2T, const std::function<float(const juce::String&)>& T2V, const juce::NormalisableRange<float>& nRange)
		{
			ID = id;
			valueToTextLambda = V2T;
			textToValueLambda = T2V;
			range = nRange;
		}

		juce::AudioParameterFloatAttributes toJuceAttributes() const
		{
			return juce::AudioParameterFloatAttributes().withValueFromStringFunction(textToValueLambda).withStringFromValueFunction(valueToTextLambda);
		}

		bool operator ==(const FloatParameterAttribute& other)
		{
			return ID == other.ID;
		}

		juce::Identifier ID;
		std::function<juce::String(float, int)> valueToTextLambda;
		std::function<float(const juce::String&)>textToValueLambda;
		juce::NormalisableRange<float> range;
	};

	class Parameter;
	struct SampleRateDependentFloatParameterAttribute
	{
		SampleRateDependentFloatParameterAttribute(const juce::Identifier& id, const std::function<std::function<juce::String(float, int)>(double)>& lambdaGen, const std::function<std::function<float(const juce::String&)>(double)>& lambdaTextGen, const std::function<juce::NormalisableRange<float>(double)>& rangeGen)
		{
			attribute.ID = id;
			lambdaGenerator = lambdaGen;
			lambdaTextGenerator = lambdaTextGen;
			rangeGenerator = rangeGen;
		}

		operator FloatParameterAttribute()
		{
			return attribute;
		}

		void setNewSampleRate(double newRate);
		FloatParameterAttribute attribute;
		std::function<std::function<juce::String(float, int)>(double)> lambdaGenerator;// input the new sampleRate into this func to get out the new lambda function
		std::function<std::function<float(const juce::String&)>(double)> lambdaTextGenerator;
		std::function<juce::NormalisableRange<float>(double)> rangeGenerator;
		juce::Array<Parameter*> effectedParameters;

	};

	struct BoolParameterAttribute
	{
		BoolParameterAttribute()
			:BoolParameterAttribute(juce::Identifier(), std::function<juce::String(bool, int)>(), std::function<bool(const juce::String&)>())
		{

		}

		BoolParameterAttribute(const juce::Identifier& id, const std::function<juce::String(bool, int)>& V2T, const std::function<bool(const juce::String&)>& T2V)
		{
			ID = id;
			valueToTextLambda = V2T;
			textToValueLambda = T2V;
		}

		bool operator ==(const BoolParameterAttribute& other)
		{
			return ID == other.ID;
		}


		juce::AudioParameterBoolAttributes toJuceAttributes() const
		{
			return juce::AudioParameterBoolAttributes().withValueFromStringFunction(textToValueLambda).withStringFromValueFunction(valueToTextLambda);
		}

		juce::Identifier ID;
		std::function<juce::String(bool, int)> valueToTextLambda;
		std::function<bool(const juce::String&)>textToValueLambda;
	};

	class GenericParameterAttributes // class to store generic parameter lambdas and ranges (%, frequency, filter q, etc.)
	{
	public:
		GenericParameterAttributes();

		const FloatParameterAttribute& getFloatAttribute(const juce::Identifier& ID);
		SampleRateDependentFloatParameterAttribute* getSampleRateDependentAttribute(const juce::Identifier& ID);

		bool isSampleRateDependent(const juce::Identifier& ID);

		void addFloatAttribute(const FloatParameterAttribute& attributeToAdd);
		void addFloatAttribute(const juce::Identifier& id, const std::function<juce::String(float, int)>& V2T, const std::function<float(const juce::String&)>& T2V, const juce::NormalisableRange<float>& nRange);

		void updateSampleRateDependentAttributes(double sampleRate);

		const BoolParameterAttribute& getBoolAttribute(const juce::Identifier& ID);

		void addBoolAttribute(const BoolParameterAttribute& attributeToAdd);
		void addBoolAttribute(const juce::Identifier& id, const std::function<juce::String(bool, int)>& V2T, const std::function<bool(const juce::String&)>& T2V);

	private:
		juce::OwnedArray<FloatParameterAttribute> floatAttributes;
		juce::OwnedArray<SampleRateDependentFloatParameterAttribute> sampleRateDependentAttributes;
		juce::OwnedArray<BoolParameterAttribute> boolAttributes;

	};

} // namespace bdsp