#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Approximates purple noise by differentiating a base noise signal. This is technically an approximation because we are working with a discrete signal.
			 * @tparam BaseSignal The Base signal to differentiate to create pruple noise - defaults to white noise
			 */
			template <typename SampleType, class BaseSignal = WhiteNoiseGenerator<SampleType>>
			class PurpleNoiseGenerator : public BaseSignal
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
					SampleType currentSample = BaseSignal::getSample(channel);
					SampleType out = (currentSample - prevSamples[channel]) / 2;
					prevSamples.set(channel, currentSample);
					return out;
				}


			protected:
				juce::Array<SampleType> prevSamples;
			};

		} //namepsace Noise
	} //namespace dsp
} //namespace bdsp