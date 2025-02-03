#include "bdsp_Pitch_Shifter.h"

namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType>
		PitchShifter<SampleType>::PitchShifter()
		{
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			BaseProcessingUnit<SampleType>::prepare(spec);

			pitchAmt.clear();
			stretch.clear();
			for (size_t c = 0; c < spec.numChannels; ++c)
			{
				stretch.add(new signalsmith::stretch::SignalsmithStretch<SampleType>());
				stretch[c]->presetCheaper(1, spec.sampleRate);

				pitchAmt.add(new juce::SmoothedValue<SampleType>());
			}

			latencyComp.setLatency(getLatency());
			latencyComp.prepare(spec);


			reset();
		}
		template<typename SampleType>
		void PitchShifter<SampleType>::reset()
		{
			BaseProcessingUnit<SampleType>::reset();

			for (auto* s : stretch)
			{
				s->reset();
			}
			latencyComp.reset();
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::initShiftAmount(int channel, SampleType semitones)
		{
			pitchAmt[channel]->setCurrentAndTargetValue(semitones);
		}
		template<typename SampleType>
		void PitchShifter<SampleType>::setShiftAmount(int channel, SampleType semitones)
		{
			pitchAmt[channel]->setTargetValue(semitones);
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::setSmoothingTime(SampleType timeInSeconds)
		{
			BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
			for (auto& p : pitchAmt)
			{
				p->reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			}

		}


		template<typename SampleType>
		void PitchShifter<SampleType>::updateSmoothedVariables()
		{
			BaseProcessingUnit<SampleType>::updateSmoothedVariables();

			for (auto* p : pitchAmt)
			{
				p->getNextValue();
			}
		}

		template class PitchShifter<float>;
		template class PitchShifter<double>;

	} // namespace dsp
} // namespace bdsp
