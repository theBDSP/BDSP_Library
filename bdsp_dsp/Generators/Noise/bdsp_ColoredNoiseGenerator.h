#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Generates various colors of noise by mixing between brown, white, and purple noise
			 */
			template <typename SampleType>
			class ColoredNoise : public WhiteNoiseGenerator<SampleType>
			{
			public:

				ColoredNoise() = default;

				virtual ~ColoredNoise() = default;

				void prepare(const juce::dsp::ProcessSpec& spec) override
				{
					WhiteNoiseGenerator<SampleType>::prepare(spec);

					brown.prepare(spec);
					purple.prepare(spec);
				}

				void reset() override
				{
					WhiteNoiseGenerator<SampleType>::reset();

					brown.reset();
					purple.reset();
				}


				void inline updateSmoothedVariables() override
				{
					WhiteNoiseGenerator<SampleType>::updateSmoothedVariables();
					smoothedColor.getNextValue();
					brown.updateSmoothedVariables();
					purple.updateSmoothedVariables();
				}

				void setSmoothingTime(SampleType timeInSeconds) override
				{
					WhiteNoiseGenerator<SampleType>::setSmoothingTime(timeInSeconds);
					smoothedColor.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);

					brown.setSmoothingTime(timeInSeconds);
					purple.setSmoothingTime(timeInSeconds);
				}

				/**
				 * -1 = brown, 0 = white, 1 = purple
				 */
				void initColor(SampleType color)
				{
					smoothedColor.setCurrentAndTargetValue(color);
				}
		
				/**
				 * -1 = brown, 0 = white, 1 = purple
				 */
				void setColor(SampleType color)
				{
					smoothedColor.setTargetValue(color);
				}

				SampleType getSample(int channel) override
				{
					if (smoothedColor.getCurrentValue() < 0)
					{
						return abs(smoothedColor.getCurrentValue()) * brown.getSample(channel) + (1 + smoothedColor.getCurrentValue()) * WhiteNoiseGenerator<SampleType>::getSample(channel);
					}
					else
					{
						return smoothedColor.getCurrentValue() * purple.getSample(channel) + (1 - smoothedColor.getCurrentValue()) * WhiteNoiseGenerator<SampleType>::getSample(channel);
					}
				}



			protected:

				juce::SmoothedValue<SampleType> smoothedColor;

				BrownNoiseGenerator<SampleType> brown;
				PurpleNoiseGenerator<SampleType> purple;
			};



		} //namespace Noise
	} //namespace dsp
} //namespace bdsp