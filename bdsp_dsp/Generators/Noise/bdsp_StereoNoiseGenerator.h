#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Wraps noise generator class and provides methods to add stereo width to the noise signal.
			 * @tparam NoiseType The base noise class to add stereo width functionality to
			 */
			template <typename SampleType, class NoiseType>
			class StereoNoiseGenerator : public NoiseType
			{
			public:


				template <typename...Types>
				StereoNoiseGenerator(Types...args)
					:NoiseType(args...)
				{

				}

				virtual ~StereoNoiseGenerator() = default;

				void initStereoWidth(SampleType newValue)
				{
					smoothedStereoWidth.setCurrentAndTargetValue(newValue);
				}
				void setStereoWidth(SampleType newValue)
				{
					smoothedStereoWidth.setTargetValue(newValue);
				}

				void inline updateSmoothedVariables() override
				{
					NoiseType::updateSmoothedVariables();
					smoothedStereoWidth.getNextValue();
				}

				void setSmoothingTime(SampleType timeInSeconds) override
				{
					NoiseType::setSmoothingTime(timeInSeconds);
					smoothedStereoWidth.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
				}


				SampleType getSample(int channel) override
				{

					if (channel == 0)
					{
						prevValue = NoiseType::getSample(channel);
						return prevValue;
					}
					else
					{
						SampleType mix = smoothedStereoWidth.getCurrentValue();
						return mix * NoiseType::getSample(channel) + (1 - mix) * prevValue;
					}
				}


			protected:
				SampleType prevValue = 0;
				juce::SmoothedValue<SampleType> smoothedStereoWidth;
			};



		} // namespace Noise
	} // namespace dsp
} // namespace bdsp
