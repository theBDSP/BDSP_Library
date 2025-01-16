#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Approximates brown noise by integrating a base noise signal. This is technically an approximation because we are doing the integration with a leaky integrator.
			 * @tparam BaseSignal The Base signal to integrate to create brown noise - defaults to white noise
			 */
			template <typename SampleType, class BaseSignal = WhiteNoiseGenerator<SampleType>>
			class BrownNoiseGenerator : public BaseSignal
			{
			public:

				void prepare(const juce::dsp::ProcessSpec& spec) override
				{
					BaseSignal::prepare(spec);
					prevSamples.resize(spec.numChannels);
					prevSamples.fill(0);
				}

				SampleType getSample(int channel) override
				{
					SampleType out = a * prevSamples[channel] + BaseSignal::getSample(channel);
					prevSamples.set(channel, out);
					return out;
				}


			protected:
				juce::Array<SampleType> prevSamples;
				SampleType a = 0.99;
			};
		} //namespace Noise
	} //namespace dsp
} //namespace bdsp