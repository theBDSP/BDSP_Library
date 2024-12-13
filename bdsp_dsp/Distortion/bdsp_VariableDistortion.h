#pragma once

namespace bdsp
{
	namespace dsp
	{


		template <typename SampleType>
		class VariableDistortion : public DistortionBase<SampleType>, public juce::AudioParameterChoice::Listener
		{
		public:
			VariableDistortion(DSP_Universals<SampleType>* lookupToUse)
				:DistortionBase<SampleType>(lookupToUse, DistortionTypes(1), 2, false)
			{

			}



			void setParameter(juce::AudioParameterChoice* param)
			{
				choiceParam = param;

				choiceParam->addListener(this);
				possibleTypes.clear();
				for (auto s : choiceParam->choices)
				{
					possibleTypes.add(DistortionBase<SampleType>::lookup->distortionLookups->stringToDistortionType(s));
                    DistortionBase<SampleType>::lookup->distortionLookups->generateSingleTable(possibleTypes.getLast());
				}
				parameterValueChanged(choiceParam->getParameterIndex(), choiceParam->getIndex());
			}

			DistortionTypes getTypeFromParameter()
			{
				int idx = choiceParam->getIndex();
				return possibleTypes.getUnchecked(idx);
			}
		protected:


			juce::Array<DistortionTypes> possibleTypes;

			juce::AudioParameterChoice* choiceParam = nullptr;

			// Inherited via Listener
			void parameterValueChanged(int parameterIndex, float newValue) override
			{
                DistortionBase<SampleType>::func = DistortionBase<SampleType>::lookup->distortionLookups->getDistortionFunc(getTypeFromParameter());
			}
			void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
			{
			}



		};

		////seperate transfer functions for + and - values
		//template <typename SampleType>
		//class BiPolarDistortion : public VariableDistortion<SampleType>
		//{
		//public:
		//	BiPolarDistortion(DSP_Universals<SampleType>* lookupToUse, DistortionTypes types)
		//		:VariableDistortion<SampleType>(lookupToUse, types)
		//	{
		//		jassert(!VariableDistortion<SampleType>::possibleTypes.contains(DistortionTypes::BitCrush));
		//	}






		//	void setParameters(juce::AudioParameterChoice* param, juce::AudioParameterChoice* negParam)
		//	{
		//		VariableDistortion<SampleType>::setParameter(param);

		//		negChoiceParam = negParam;

		//		negChoiceParam->addListener(this);

		//		negType = getNegTypeFromParameter();
		//	}

		//	DistortionTypes getNegTypeFromParameter()
		//	{
		//		int idx = negChoiceParam->getIndex();
		//		return VariableDistortion<SampleType>::possibleTypes[idx];
		//	}
		//private:

		//	inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
		//	{
		//		SampleType scaledInput = inputSample * DistortionBase<SampleType>::smoothedPre.getCurrentValue();

		//		return DistortionBase<SampleType>::lookup->getDistortion(scaledInput, DistortionBase<SampleType>::smoothedAmt.getCurrentValue(), DistortionBase<SampleType>::isScaled, inputSample <= 0 ? negType : VariableDistortion<SampleType>::type);



		//	}
		//	DistortionTypes negType;

		//	juce::AudioParameterChoice* negChoiceParam = nullptr;

		//	// Inherited via Listener
		//	void parameterValueChanged(int parameterIndex, float newValue) override
		//	{
		//		VariableDistortion<SampleType>::type = VariableDistortion<SampleType>::getTypeFromParameter();
		//		negType = getNegTypeFromParameter();
		//	}
		//	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
		//	{
		//	}


		//};



	}// namespace dsp
}// namnepace bdsp


