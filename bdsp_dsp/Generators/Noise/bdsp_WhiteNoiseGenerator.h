#pragma once
namespace bdsp
{
	namespace dsp
	{
		namespace Noise
		{
			/**
			 * Very basic white noise generator
			 */
			template <typename SampleType>
			class WhiteNoiseGenerator : public GeneratorBase<SampleType>
			{
			public:

				SampleType getSample(int channel) override
				{
					return 2 * rand.getNext() - 1;
				}


			protected:
				RandomOfType<SampleType> rand;
			};


		} // namespace Noise
	} // namespace dsp
} // namespace bdsp
