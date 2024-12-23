#include "bdsp_DelayLine.h"

namespace bdsp
{
	namespace dsp
	{

		template<typename SampleType>
		DelayLineBase<SampleType>::DelayLineBase()
			:DelayLineBase(0)
		{
		}

		template<typename SampleType>
		DelayLineBase<SampleType>::DelayLineBase(int maxDelayInSamples)
		{
			jassert(maxDelayInSamples >= 0);

			setMaxDelay(maxDelayInSamples);
		}


		template<typename SampleType>
		void DelayLineBase<SampleType>::setMaxDelay(int numSamples)
		{
			totalSize = juce::nextPowerOfTwo(juce::jmax(4, numSamples + 1));
			bufferData.setSize(bufferData.getNumChannels(), totalSize);
			reset();
		}



		template<typename SampleType>
		void DelayLineBase<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			jassert(spec.numChannels > 0);

			BaseProcessingUnit<SampleType>::sampleRate = spec.sampleRate;

			bufferData.setSize((int)spec.numChannels, totalSize, false, false, true);

			writePos.resize(spec.numChannels);
			readPos.resize(spec.numChannels);



            BaseProcessingUnit<SampleType>::smoothTime = 0.15 * BaseProcessingUnit<SampleType>::sampleRate;
			reset();
		}

		template<typename SampleType>
		void DelayLineBase<SampleType>::reset()
		{
			for (auto vec : { &writePos, &readPos })
				std::fill(vec->begin(), vec->end(), 0);

			bufferData.clear();
		}

		template<typename SampleType>
		SampleType DelayLineBase<SampleType>::popSample(int channel)
		{
			//interpolatedSample = interpolateSample(channel);

			int index = (readPos[(size_t)channel] + delayInt) & (totalSize - 1);

			return bufferData.getSample(channel, index);
		}

		template<typename SampleType>
		SampleType DelayLineBase<SampleType>::popSampleUpdateRead(int channel)
		{
			SampleType out = popSample(channel);
			updateReadPointer(channel);
			return out;
		}

		template<typename SampleType>
		void DelayLineBase<SampleType>::pushSample(int channel, SampleType smp)
		{
			jassert(isfinite(smp));
			bufferData.setSample(channel, writePos[(size_t)channel], smp);
			writePos[(size_t)channel] = (writePos[(size_t)channel] + totalSize - 1) & (totalSize - 1);
		}

		template<typename SampleType>
		void DelayLineBase<SampleType>::updateReadPointer(int channel)
		{
			readPos[(size_t)channel] = (readPos[(size_t)channel] + totalSize - 1) & (totalSize - 1);
		}

	
		template<typename SampleType>
		void DelayLineBase<SampleType>::setDelay(SampleType newValue)
		{
			delayInt = static_cast<int>(newValue);
		}

		template<typename SampleType>
		int DelayLineBase<SampleType>::getDelayInt()
		{
			return delayInt;
		}

	
		template class DelayLineBase<float>;
		template class DelayLineBase<double>;
		//================================================================================================================================================================================================


		template <typename SampleType, typename DelayType, typename InterpolationType>
		DelayLine<SampleType, DelayType, InterpolationType>::DelayLine()
			:DelayLine(0, nullptr)
		{
		}

		//==============================================================================


		template <typename SampleType, typename DelayType, typename InterpolationType>
		DelayLine<SampleType, DelayType, InterpolationType>::DelayLine(int maxDelayInSamples, DSP_Universals<SampleType>* lookupToUse)
			:DelayLineBase<SampleType>(maxDelayInSamples),
			outputPanner(lookupToUse)
		{
			lookup = lookupToUse;
			if (lookup != nullptr)
			{
				generateLookupTables();
			}



			initFeedback(0);
            BaseProcessingUnit<SampleType>::initMix(1);
			outputPanner.initPan(0);
			initPongMix(0);

		}

		//==============================================================================
		template <typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::updateDelay()
		{

			// Negative delay time or delay time greater than the max delay time wouldn't make sense
			jassert(juce::isPositiveAndNotGreaterThan(targetDelay, DelayLineBase<SampleType>::totalSize - 1));


			auto inc = (targetDelay - delay) / (BaseProcessingUnit<SampleType>::smoothTime); // calculates how much to increment the delay time this sample
			delay += inc;

			DelayLineBase<SampleType>::delayInt = static_cast<int> (std::floor(delay));
			delayFrac = delay - (SampleType)DelayLineBase<SampleType>::delayInt;


			updateInternalVariables(); // calculate other necessary variables depending on the smoothing type being used
		}



		template<typename SampleType, typename DelayType, typename InterpolationType>
		SampleType DelayLine<SampleType, DelayType, InterpolationType>::getDelay() const
		{
			return delay;
		}


		template<typename SampleType, typename DelayType, typename InterpolationType>
		SampleType DelayLine<SampleType, DelayType, InterpolationType>::getTargetDelay() const
		{
			return targetDelay;
		}



		//==============================================================================
		template <typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			v.resize(spec.numChannels);

			DelayLineBase<SampleType>::prepare(spec);

			reset();
		}

		template <typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::reset()
		{
			DelayLineBase<SampleType>::reset();

			std::fill(v.begin(), v.end(), static_cast<SampleType> (0));
		}




		template <typename SampleType, typename DelayType, typename InterpolationType>
		SampleType DelayLine<SampleType, DelayType, InterpolationType>::popSampleUpdateRead(int channel)
		{
			interpolatedSample = interpolateSample(channel);

			DelayLineBase<SampleType>::updateReadPointer(channel);

			return interpolatedSample;
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		StereoSample<SampleType> DelayLine<SampleType, DelayType, InterpolationType>::pingPongPop()
		{
			auto poppedSample = StereoSample<SampleType>(popSample(0), popSample(1));


			//Fewer multiplication of: norm * (1-pongMix) + pong * pongMix
			// norm + pongMix * (pong-norm)

			return StereoSample<SampleType>(poppedSample.left + smoothedPongMix.getCurrentValue() * (poppedSample.right - poppedSample.left), 
				poppedSample.right + smoothedPongMix.getCurrentValue() * (poppedSample.left - poppedSample.right));
		}


		template<typename SampleType, typename DelayType, typename InterpolationType>
		StereoSample<SampleType> DelayLine<SampleType, DelayType, InterpolationType>::pingPongPopUpdateRead()
		{
			auto out = pingPongPop();
			updateReadPointer(0);
			updateReadPointer(1);
			return out;
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		SampleType DelayLine<SampleType, DelayType, InterpolationType>::popSample(int channel)
		{
			return interpolateSample(channel);
		}


	
		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::setFeedback(SampleType newValue)
		{
			smoothedFeedback.setTargetValue(newValue);
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::setPongMix(SampleType newValue)
		{
			smoothedPongMix.setTargetValue(newValue);
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::setDelay(SampleType newValue)
		{
			targetDelay = newValue;
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::snapDelay(SampleType newValue)
		{
			targetDelay = newValue;
			delay = newValue;
		}




		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::initFeedback(SampleType newValue)
		{
			smoothedFeedback.setCurrentAndTargetValue(newValue);
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::initPongMix(SampleType newValue)
		{
			smoothedPongMix.setCurrentAndTargetValue(newValue);
		}



		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::updateSmoothedVariables()
		{
			DelayLineBase<SampleType>::updateSmoothedVariables();
			smoothedFeedback.getNextValue();
			smoothedPongMix.getNextValue();

			outputPanner.updateSmoothedVariables();

			updateDelay();
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		void DelayLine<SampleType, DelayType, InterpolationType>::setSmoothingTime(SampleType timeInSeconds)
		{
			DelayLineBase<SampleType>::setSmoothingTime(timeInSeconds);
			smoothedFeedback.reset(DelayLineBase<SampleType>::sampleRate, timeInSeconds);
			smoothedPongMix.reset(DelayLineBase<SampleType>::sampleRate, timeInSeconds);

			outputPanner.setSmoothingTime(timeInSeconds);
		}


		template class DelayLine<float, DelayTypes::Basic, DelayLineInterpolationTypes::None>;
		template class DelayLine<double, DelayTypes::Basic, DelayLineInterpolationTypes::None>;
		template class DelayLine<float, DelayTypes::Basic, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<double, DelayTypes::Basic, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<float, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<double, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<float, DelayTypes::Basic, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<double, DelayTypes::Basic, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<float, DelayTypes::Basic, DelayLineInterpolationTypes::Lanczos>;
		template class DelayLine<double, DelayTypes::Basic, DelayLineInterpolationTypes::Lanczos>;




		template class DelayLine<float, DelayTypes::FBCF, DelayLineInterpolationTypes::None>;
		template class DelayLine<double, DelayTypes::FBCF, DelayLineInterpolationTypes::None>;
		template class DelayLine<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Lanczos>;
		template class DelayLine<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Lanczos>;

		template class DelayLine<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::None>;
		template class DelayLine<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::None>;
		template class DelayLine<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lanczos>;
		template class DelayLine<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lanczos>;

		template class DelayLine<float, DelayTypes::PingPong, DelayLineInterpolationTypes::None>;
		template class DelayLine<double, DelayTypes::PingPong, DelayLineInterpolationTypes::None>;
		template class DelayLine<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Linear>;
		template class DelayLine<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Lagrange3rd>;
		template class DelayLine<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Thiran>;
		template class DelayLine<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Lanczos>;
		template class DelayLine<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Lanczos>;





	}// namespace dsp
}// namespace bdsp
