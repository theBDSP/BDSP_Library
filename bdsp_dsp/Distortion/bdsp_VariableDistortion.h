#pragma once

namespace bdsp
{
	namespace dsp
	{

		/**
		 * Basic Waveshaping Distortion with selectable distortion type.
		 */
		template <typename SampleType>
		class VariableDistortion : public DistortionBase<SampleType>, public juce::AudioParameterChoice::Listener
		{
		public:

			/**
			 * Creates an empty distortion processor.
			 * This processor will do nothing until you cal setParameter.
			 */
			VariableDistortion(DSP_Universals<SampleType>* lookupToUse)
				:DistortionBase<SampleType>(lookupToUse, nullptr, 2, true)
			{

			}


			/**
			 * Links this processor to a parameter that controls which tpye of distortion to use.
			 */
			void setParameter(juce::AudioParameterChoice* param)
			{
				choiceParam = param;

				choiceParam->addListener(this);
				possibleTypes.clear();
				
				// Fill the array of possible types with those specified by the parameter provided
				for (const auto& s : choiceParam->choices)
				{
					possibleTypes.add(DistortionBase<SampleType>::lookup->distortionLookups->nameToDistortionType(s));
				}

				parameterValueChanged(choiceParam->getParameterIndex(), choiceParam->getIndex()); // init the current type with the value of the provided parameter
			}

		protected:


			juce::Array<DistortionTypeBase<SampleType>*> possibleTypes;

			juce::AudioParameterChoice* choiceParam = nullptr;

			// Inherited via Listener
			void parameterValueChanged(int parameterIndex, float newValue) override
			{
                DistortionBase<SampleType>::type = getTypeFromParameter();
			}
			void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
			{
			}


		private:
			DistortionTypeBase<SampleType>* getTypeFromParameter()
			{
				int idx = choiceParam->getIndex();
				return possibleTypes.getUnchecked(idx);
			}
		};

	}// namespace dsp
}// namnepace bdsp


