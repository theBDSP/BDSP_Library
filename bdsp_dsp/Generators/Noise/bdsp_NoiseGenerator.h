#pragma once


namespace bdsp
{
	namespace dsp
	{
		template <typename SampleType>
		class WhiteNoiseGenerator : public GeneratorBase<SampleType>
		{
		public:

			SampleType getSample(int channel, int smp) override
			{
				return rand.getNext();
			}


		protected:
			RandomOfType<SampleType> rand;


		};


		template <typename SampleType>
		class VelvetNoiseGenerator : public GeneratorBase<SampleType>
		{
		public:

			SampleType getSample(int channel, int smp) override
			{
				SampleType v = rand.getNext();
				if (v > probability)
				{
					return 0;
				}
				else if (v < probability / 2)
				{
					return -1;
				}
				else
				{
					return 1;
				}
			}

			void setProbability(SampleType newValue)
			{
				probability = newValue;
			}


		protected:
			RandomOfType<SampleType> rand;
			SampleType probability = 0.75;

		};

		template <typename SampleType, class BaseSignal = WhiteNoiseGenerator<SampleType>>
		class BrownNoiseGenerator : public BaseSignal
		{
		public:

			SampleType getSample(int channel, int smp) override
			{
				SampleType out = a * prev + BaseSignal::getSample(channel, smp);
				prev = out;
				return out;
			}


		protected:
			SampleType prev = 0;
			SampleType a = 0.99;


		};

		template <typename SampleType, class BaseSignal = WhiteNoiseGenerator<SampleType>>
		class PurpleNoiseGenerator : public BaseSignal
		{
		public:

			SampleType getSample(int channel, int smp) override
			{
				SampleType next = BaseSignal::getSample(channel, smp);
				SampleType out = (prev + next) / 2;
				prev = next;
				return out;
			}


		protected:
			SampleType prev = 0;
		};




		template <typename SampleType>
		class ColoredNoiseMono : public WhiteNoiseGenerator<SampleType>
		{
		public:

			ColoredNoiseMono() = default;

			virtual ~ColoredNoiseMono() = default;

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
			void initColor(SampleType color) // -1 = red, 0 = white, 1 = blue
			{
				smoothedColor.setCurrentAndTargetValue(color);
			}
			void setColor(SampleType color) // -1 = red, 0 = white, 1 = blue
			{
				smoothedColor.setTargetValue(color);
			}
			SampleType getSample(int channel, int smp) override
			{
			
					if (smoothedColor.getCurrentValue() < 0)
					{
						return abs(smoothedColor.getCurrentValue()) * brown.getSample(channel, smp) + (1 + smoothedColor.getCurrentValue()) * WhiteNoiseGenerator<SampleType>::getSample(channel, smp);
					}
					else
					{
						return smoothedColor.getCurrentValue() * purple.getSample(channel, smp) + (1 - smoothedColor.getCurrentValue()) * WhiteNoiseGenerator<SampleType>::getSample(channel, smp);
					}
				
			}

			

		protected:

			juce::SmoothedValue<SampleType> smoothedColor;

			BrownNoiseGenerator<SampleType> brown;
			PurpleNoiseGenerator<SampleType> purple;

			


		};



		template <typename SampleType>
		class ColoredNoiseStereo : public ColoredNoiseMono<SampleType>
		{
		public:

			ColoredNoiseStereo()
				:ColoredNoiseMono<SampleType>()
			{
			}

			virtual ~ColoredNoiseStereo() = default;

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
				ColoredNoiseMono<SampleType>::updateSmoothedVariables();
				smoothedStereoWidth.getNextValue();
			}

			void setSmoothingTime(SampleType timeInSeconds) override
			{
				ColoredNoiseMono<SampleType>::setSmoothingTime(timeInSeconds);
				smoothedStereoWidth.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}


			SampleType getSample(int channel, int smp) override
			{

				if (channel == 0)
				{
					prevValue = ColoredNoiseMono<SampleType>::getSample(channel, smp);
					return prevValue;
				}
				else
				{
					SampleType mix = smoothedStereoWidth.getCurrentValue();
					return mix * ColoredNoiseMono<SampleType>::getSample(channel, smp) + (1 - mix) * prevValue;
				}
			}


		protected:
			SampleType prevValue = 0;
			juce::SmoothedValue<SampleType> smoothedStereoWidth;
		};


	} // namespace dsp
} // namespace bdsp
