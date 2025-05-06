#pragma once

#define BDSP_PHASER_MAX_POLES 16

namespace bdsp
{
	namespace dsp
	{
		/**
		 * Basic phaser unit implemented with a pair of cascaded BiQuad Allpass filters
		 */
		template <typename SampleType>
		class Phaser : public BaseProcessingUnit<SampleType>
		{
		public:
			Phaser(DSP_Universals<SampleType>* lookupToUse, const juce::NormalisableRange<float>* freqRange)
				:frequencyRange(freqRange)
			{
				lookup = lookupToUse;

				lookup->waveLookups.operator->();

				allpassL = std::make_unique<CascadedFilter<SampleType, BiQuadFilters::SecondOrderAllpassFilter<SampleType>, BDSP_PHASER_MAX_POLES / 2>>(lookup);
				allpassR = std::make_unique<CascadedFilter<SampleType, BiQuadFilters::SecondOrderAllpassFilter<SampleType>, BDSP_PHASER_MAX_POLES / 2>>(lookup);
			}

			~Phaser() = default;

			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				juce::dsp::ProcessSpec monoSpec{ spec.sampleRate, spec.maximumBlockSize, 1 };

				allpassL->prepare(monoSpec);
				allpassR->prepare(monoSpec);

				BaseProcessingUnit<SampleType>::prepare(spec);

				reset();
			}


			void reset() override
			{
				allpassL->reset();
				allpassR->reset();

				BaseProcessingUnit<SampleType>::reset();
				modPhase = 0;

				fbL = 0;
				fbR = 0;
			}


			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept override
			{
				SampleType l = allpassL->processSample(0, inputSample.left + fbL);
				SampleType r = allpassR->processSample(0, inputSample.right + fbR);

				fbL = l * feedback;
				fbR = r * feedback;

				return BaseProcessingUnit<SampleType>::applyDryWetMix(inputSample, StereoSample<SampleType>(l, r));
			}

			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed || BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() < 2 || BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels() < 2)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}

				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);


				for (size_t i = 0; i < numSamples; ++i)
				{
					updateSmoothedVariables();
					auto smp = processSampleStereo(StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i)));

					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, smp.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, smp.right);
				}
			}



			void updateSmoothedVariables() override
			{
				float tmp;
				auto modInc = phaseChangeRate / (BaseProcessingUnit<SampleType>::sampleRate);

				modPhase = modf(modPhase + modInc, &tmp);

				SampleType modValL = getModPosition(true) + 1;
				SampleType modValR = getModPosition(false) + 1;

				SampleType freqL = frequencyRange->convertFrom0to1(modValL - static_cast<int>(modValL));
				SampleType freqR = frequencyRange->convertFrom0to1(modValR - static_cast<int>(modValR));

				//================================================================================================================================================================================================

				allpassL->setFrequency(freqL);
				allpassR->setFrequency(freqR);

				allpassL->updateSmoothedVariables();
				allpassR->updateSmoothedVariables();

				allpassL->calculateCoefficients(allpassL->parameters.get(), allpassL->coefficients.get());
				allpassR->calculateCoefficients(allpassR->parameters.get(), allpassR->coefficients.get());

				//================================================================================================================================================================================================

				BaseProcessingUnit<SampleType>::updateSmoothedVariables();
			}

			SampleType getModPosition(bool left)
			{
				float tmp;
				auto center = (minPhase + maxPhase) / 2;
				auto depth = maxPhase - minPhase;

				auto pos = left ? modPhase : modf(modPhase + stereoWidth / 2, &tmp);
				SampleType modVal = lookup->waveLookups->lookupSin(0.5, pos, true, depth);
				return center + modVal;

			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				allpassL->setSmoothingTime(timeInSeconds);
				allpassR->setSmoothingTime(timeInSeconds);
			}



			void setPhaseChangeRate(SampleType newBase)
			{
				phaseChangeRate = newBase;
			}

			void setCenterAndDepth(SampleType newCenter, SampleType newDepth)
			{
				setMinPhase(newCenter - newDepth / 2);
				setMaxPhase(newCenter + newDepth / 2);
			}

			void setMinPhase(SampleType newValue)
			{
				minPhase = newValue;
			}

			void setMaxPhase(SampleType newValue)
			{
				maxPhase = newValue;
			}

			void setStereoWidth(SampleType newBase)
			{
				stereoWidth = newBase;
			}

			void setFeedback(SampleType newBase)
			{
				feedback = newBase;
			}

			SampleType getFeedback()
			{
				return feedback;
			}

			void setNumStages(int newNumStages)
			{
				allpassL->setOrder(newNumStages);
				allpassR->setOrder(newNumStages);
			}

			SampleType calculateResponseForFrequency(int channel, SampleType freq)
			{
				auto* f = channel == 0 ? allpassL.get() : allpassR.get();
				juce::dsp::Complex<SampleType> filterResponse = f->calculateResponseForFrequency(f->parameters.get(), freq);
				juce::dsp::Complex<SampleType> feedbackResponse = juce::dsp::Complex<SampleType>(1, 0) - feedback * std::polar(SampleType(1), -2 * PI * freq / BaseProcessingUnit<SampleType>::sampleRate);
				return std::arg(filterResponse / feedbackResponse);
			}

			SampleType calculateResponseForNormalizedFrequency(int channel, SampleType normalizedFreq)
			{
				return calculateResponseForFrequency(channel, normalizedFreq * BaseProcessingUnit<SampleType>::sampleRate);
			}



		protected:

			std::unique_ptr <CascadedFilter<SampleType, BiQuadFilters::SecondOrderAllpassFilter<SampleType>, BDSP_PHASER_MAX_POLES / 2>>allpassL, allpassR;

			SampleType phaseChangeRate;
			SampleType minPhase, maxPhase;
			SampleType stereoWidth;
			SampleType feedback;

			int numStages = 4;

			SampleType modPhase;

			DSP_Universals<SampleType>* lookup;




			SampleType fbL = 0, fbR = 0;


			const juce::NormalisableRange<float>* frequencyRange;

		};




	}// namespace dsp
}// namnepace bdsp


