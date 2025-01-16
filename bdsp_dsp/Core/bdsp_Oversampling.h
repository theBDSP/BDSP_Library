#pragma once
namespace bdsp
{
	namespace dsp
	{

		/**
		 * Performs up and down sampling to reduce aliasing in non linear dsp components. Also provides latency compensation for latency added in the oversampling process
		 */
		template <typename SampleType>
		class Oversampling : public juce::dsp::Oversampling<SampleType>
		{
		public:

			/**
			 * Creates an oversampling processor with a certain oversampling factor and filter type
			 * @param numChannels The number of channels this should be able to process
			 * @param factor The number of oversampling stages (the upsampled samplerate will be 2 ^ factor * the original samplerate)
			 * @param type The type of filter design employed for filtering during oversampling
			 * @param shoudlHandleLatencyCompensation If true, the internal latency compensator will handle latency compensation when mixing
			 */
			Oversampling(size_t numChannels, size_t factor, typename juce::dsp::Oversampling<SampleType>::FilterType type, bool shoudlHandleLatencyCompensation)
				:juce::dsp::Oversampling<SampleType>(numChannels, factor, type, false)
			{
				latencyComp.handlesLatencyCompensation = shoudlHandleLatencyCompensation;

				//================================================================================================================================================================================================
				// create the correct number of oversampling stages
				juce::dsp::Oversampling<SampleType>::clearOversamplingStages();

				juce::dsp::Oversampling<SampleType>::addOversamplingStage(type, 0.001f, -100.0f, 0.001f, -100.0f);

				for (int i = 1; i < factor; ++i)
				{
					juce::dsp::Oversampling<SampleType>::addOversamplingStage(type, 0.005f, -100.0f, 0.005f, -100.0f);
				}
				//================================================================================================================================================================================================

				juce::dsp::Oversampling<SampleType>::setUsingIntegerLatency(true);
			}

			void prepare(const juce::dsp::ProcessSpec& spec)
			{
				juce::dsp::Oversampling<SampleType>::initProcessing(spec.maximumBlockSize);
				latencyComp.setLatency(juce::dsp::Oversampling<SampleType>::getLatencyInSamples());
				latencyComp.prepare(spec);
			}

			int getLatency()
			{
				return latencyComp.getLatency();
			}

			template<typename ProcessContext>
			void mixWithLatencyCompensation(const ProcessContext& context, juce::SmoothedValue<SampleType>& dryGain, juce::SmoothedValue<SampleType>& wetGain, bool bypassed = false)
			{
				latencyComp.mixWithLatencyCompensation(context, dryGain, wetGain, bypassed);
			}

		private:
			LatencyCompensator<SampleType> latencyComp;

		};
	} //namespace dsp
} //namespace bdsp