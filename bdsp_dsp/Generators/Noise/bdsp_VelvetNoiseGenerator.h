#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Basic velvet noise generator. The probability every sample that an impulse is generated can be set, the probability that generated impulse is positive or negative is always 50/50.
			 */
			template <typename SampleType>
			class VelvetNoiseGenerator : public GeneratorBase<SampleType>
			{
			public:

				SampleType getSample(int channel) override
				{
					SampleType v = rand.getNext(); // generate a random number
					if (v > probability) // check if random number shouldn't generate an impulse
					{
						return 0;
					}
					else if (v < halfProbability)
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
					halfProbability = probability / 2;
				}


			protected:
				RandomOfType<SampleType> rand;
				SampleType probability = 0.75;
				SampleType halfProbability = 0.75 / 2; // store this so we don't have to calculate it every sample

			};
		} //namespace Noise
	} //namespace dsp
} //namespace bdsp
