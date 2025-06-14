#pragma once
#include "bdsp_MultiTapDelay.h"
namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType, typename DelayType, typename InterpolationType>
		MultiTapDelay<SampleType, DelayType, InterpolationType>::MultiTapDelay(int maxDelayInSamples, DSP_Universals<SampleType>* lookupToUse)
			:DelayLine<SampleType, DelayType, InterpolationType>(maxDelayInSamples, lookupToUse)
		{
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		SampleType MultiTapDelay<SampleType, DelayType, InterpolationType>::popSample(int channel)
		{
			//jassert(channel < DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels());
			SampleType out = 0;
			int stride = DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels();
			for (int c = channel; c < DelayLine<SampleType, DelayType, InterpolationType>::delayFrac.size(); c += stride)
			{
				tapOutputs[c] = DelayLine<SampleType, DelayType, InterpolationType>::popSample(c);
				out += tapOutputs[c];
				if (c >= DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels())
				{
					updateReadPointer(c);
				}
			}
			return out;
		}


		template<typename SampleType, typename DelayType, typename InterpolationType>
		void MultiTapDelay<SampleType, DelayType, InterpolationType>::setTap(int channel, int tapNumber, SampleType delay)
		{
			int pseudoChannel = DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels()* tapNumber + channel;
			if (DelayLine<SampleType, DelayType, InterpolationType>::delayFrac.size() <= pseudoChannel)
			{
				DelayLine<SampleType, DelayType, InterpolationType>::readPos.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::readPos[(size_t)pseudoChannel] = DelayLine<SampleType, DelayType, InterpolationType>::readPos[(size_t)channel];


				DelayLine<SampleType, DelayType, InterpolationType>::targetDelay.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::delay.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::delayFrac.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::delayInt.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::v.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::alpha.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::reversePos.resize(pseudoChannel + 1);
				DelayLine<SampleType, DelayType, InterpolationType>::reverseOffset.resize(pseudoChannel + 1);

				tapOutputs.resize(pseudoChannel + 1);
				tapOutputs[pseudoChannel] = 0;
			}

			setDelay(pseudoChannel, delay);
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		SampleType MultiTapDelay<SampleType, DelayType, InterpolationType>::getTap(int channel, int tapNumber)
		{
			int pseudoChannel = DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels()* tapNumber + channel;
			return getDelay(pseudoChannel);
		}

		template<typename SampleType, typename DelayType, typename InterpolationType>
		int MultiTapDelay<SampleType, DelayType, InterpolationType>::getChannel(int channel) const
		{
			return channel % DelayLine<SampleType, DelayType, InterpolationType>::bufferData.getNumChannels();
		}


		template class MultiTapDelay<float, DelayTypes::Basic, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<double, DelayTypes::Basic, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<float, DelayTypes::Basic, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<double, DelayTypes::Basic, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<float, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<double, DelayTypes::Basic, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<float, DelayTypes::Basic, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<double, DelayTypes::Basic, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<float, DelayTypes::Basic, DelayLineInterpolationTypes::Lanczos>;
		template class MultiTapDelay<double, DelayTypes::Basic, DelayLineInterpolationTypes::Lanczos>;




		template class MultiTapDelay<float, DelayTypes::FBCF, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<double, DelayTypes::FBCF, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<float, DelayTypes::FBCF, DelayLineInterpolationTypes::Lanczos>;
		template class MultiTapDelay<double, DelayTypes::FBCF, DelayLineInterpolationTypes::Lanczos>;

		template class MultiTapDelay<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<float, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lanczos>;
		template class MultiTapDelay<double, DelayTypes::LPFBCF, DelayLineInterpolationTypes::Lanczos>;

		template class MultiTapDelay<float, DelayTypes::PingPong, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<double, DelayTypes::PingPong, DelayLineInterpolationTypes::None>;
		template class MultiTapDelay<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Linear>;
		template class MultiTapDelay<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Lagrange3rd>;
		template class MultiTapDelay<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Thiran>;
		template class MultiTapDelay<float, DelayTypes::PingPong, DelayLineInterpolationTypes::Lanczos>;
		template class MultiTapDelay<double, DelayTypes::PingPong, DelayLineInterpolationTypes::Lanczos>;


	} //namespace dsp
} //namespace bdsp