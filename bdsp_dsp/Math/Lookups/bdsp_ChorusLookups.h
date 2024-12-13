#pragma once


#define BDSP_CHORUS_MIN_VOICES 2
#define BDSP_CHORUS_MAX_VOICES 8

namespace bdsp
{
	namespace dsp
	{
		// holds useful lookup tables to reduce computational load during process block
		template<typename T>
		class ChorusLookups
		{
		public:
			ChorusLookups()
			{
				chorusPhases.resize(BDSP_CHORUS_MAX_VOICES);
				chorusVelocityMults.resize(BDSP_CHORUS_MAX_VOICES);
				for (int i = 0; i < BDSP_CHORUS_MAX_VOICES; ++i)
				{
					chorusPhases.set(i, T(i) / BDSP_CHORUS_MAX_VOICES);
					chorusVelocityMults.set(i, (i % 2 == 0 ? -1 : 1) * chorusPhases[i] * 0.1 + 1);
				}

			}


			virtual ~ChorusLookups()
			{
			}

		

			juce::Array<T>& getChorusPhases()
			{
				return chorusPhases;
			}
			juce::Array<T>& getChorusVelocityMults()
			{
				return chorusVelocityMults;
			}


		private:
			juce::Array<T> chorusPhases, chorusVelocityMults;


			JUCE_LEAK_DETECTOR(ChorusLookups)
		};
	}// namespace dsp

}//namespace bdsp
