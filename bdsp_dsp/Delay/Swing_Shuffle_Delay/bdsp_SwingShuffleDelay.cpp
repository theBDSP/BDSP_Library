#pragma once
#include "bdsp_SwingShuffleDelay.h"
namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType, typename InterpolationType>
		SwingShuffleDelay<SampleType, InterpolationType>::SwingShuffleDelay(int maxDelayInSamples, SampleType maxSwingShuffleAmt, DSP_Universals<SampleType>* lookupToUse)
			:delay1(maxDelayInSamples* maxSwingShuffleAmt, lookupToUse),
			delay2(maxDelayInSamples* maxSwingShuffleAmt, lookupToUse)
		{
			maxSwingShuffleAmount = maxSwingShuffleAmt;
		}
		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setMaxDelay(int numSamples)
		{
			delay1.setMaxDelay(numSamples * maxSwingShuffleAmount);
			delay2.setMaxDelay(numSamples * maxSwingShuffleAmount);
		}
		template<typename SampleType, typename InterpolationType>
		SampleType SwingShuffleDelay<SampleType, InterpolationType>::getDelay(int channel) const
		{
			return baseDelay[(size_t)channel];
		}

		template<typename SampleType, typename InterpolationType>
		SampleType SwingShuffleDelay<SampleType, InterpolationType>::getTargetDelay(int channel) const
		{
			return getDelay(channel);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::updateSmoothedVariables()
		{
			BaseProcessingUnit<SampleType>::updateSmoothedVariables();
			delay1.updateSmoothedVariables();
			delay2.updateSmoothedVariables();
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			BaseProcessingUnit<SampleType>::prepare(spec);
			delay1.prepare(spec);
			delay2.prepare(spec);

			baseDelay.resize(spec.numChannels);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::reset()
		{
			BaseProcessingUnit<SampleType>::reset();
			delay1.reset();
			delay2.reset();

			prevOutL = 0;
			prevOutR = 0;
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setSwingShuffleAmount(SampleType newValue)
		{
			swingShuffleAmount = newValue;
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setSmoothingTime(SampleType timeInSeconds)
		{
			BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
			delay1.setSmoothingTime(timeInSeconds);
			delay2.setSmoothingTime(timeInSeconds);

		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setDelaySmoothingTime(SampleType timeInSeconds)
		{
			delay1.setDelaySmoothingTime(timeInSeconds);
			delay2.setDelaySmoothingTime(timeInSeconds);

		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setFeedback(SampleType newValue)
		{
			delay1.setFeedback(newValue);
			delay2.setFeedback(newValue);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setPongMix(SampleType newValue)
		{
			delay1.setPongMix(newValue);
			delay2.setPongMix(newValue);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setDelay(int channel, SampleType newValue)
		{
			baseDelay[(size_t)channel] = newValue;
			delay1.setDelay(channel, swingShuffleAmount * newValue);
			delay2.setDelay(channel, (1 - swingShuffleAmount) * newValue);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::snapDelay(int channel, SampleType newValue)
		{
			baseDelay[(size_t)channel] = newValue;
			delay1.snapDelay(channel, swingShuffleAmount * newValue);
			delay2.snapDelay(channel, (1 - swingShuffleAmount) * newValue);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::initFeedback(SampleType newValue)
		{
			delay1.initFeedback(newValue);
			delay2.initFeedback(newValue);
		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::initPongMix(SampleType newValue)
		{
			delay1.initPongMix(newValue);
			delay2.initPongMix(newValue);

		}

		template<typename SampleType, typename InterpolationType>
		void SwingShuffleDelay<SampleType, InterpolationType>::setReversed(bool shouldBeReversed)
		{
			delay1.setReversed(shouldBeReversed);
		}

		template<typename SampleType, typename InterpolationType>
		SampleType SwingShuffleDelay<SampleType, InterpolationType>::getFeedback()
		{
			return delay1.getFeedback();
		}

		template<typename SampleType, typename InterpolationType>
		SampleType SwingShuffleDelay<SampleType, InterpolationType>::getPongMix()
		{
			return delay1.getPongMix();
		}

		template<typename SampleType, typename InterpolationType>
		bool SwingShuffleDelay<SampleType, InterpolationType>::getReversed()
		{
			return delay1.getReversed();
		}



		template class SwingShuffleDelay<float, DelayLineInterpolationTypes::None>;
		template class SwingShuffleDelay<double, DelayLineInterpolationTypes::None>;
		template class SwingShuffleDelay<float, DelayLineInterpolationTypes::Linear>;
		template class SwingShuffleDelay<double, DelayLineInterpolationTypes::Linear>;
		template class SwingShuffleDelay<float, DelayLineInterpolationTypes::Lagrange3rd>;
		template class SwingShuffleDelay<double, DelayLineInterpolationTypes::Lagrange3rd>;
		template class SwingShuffleDelay<float, DelayLineInterpolationTypes::Thiran>;
		template class SwingShuffleDelay<double, DelayLineInterpolationTypes::Thiran>;
		template class SwingShuffleDelay<float, DelayLineInterpolationTypes::Lanczos>;
		template class SwingShuffleDelay<double, DelayLineInterpolationTypes::Lanczos>;

	} //namespace dsp
} //namespace bdsp