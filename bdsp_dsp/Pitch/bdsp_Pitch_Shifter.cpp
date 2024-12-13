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

			stretch.presetCheaper(spec.numChannels, spec.sampleRate);

			latencyComp.setLatency(getLatency());
			latencyComp.prepare(spec);


			reset();
		}
		template<typename SampleType>
		void PitchShifter<SampleType>::reset()
		{
			BaseProcessingUnit<SampleType>::reset();
			stretch.reset();
			latencyComp.reset();
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::initShiftAmount(SampleType semitones)
		{
			pitchAmt.setCurrentAndTargetValue(semitones);
		}
		template<typename SampleType>
		void PitchShifter<SampleType>::setShiftAmount(SampleType semitones)
		{
			pitchAmt.setTargetValue(semitones);
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::setSmoothingTime(SampleType timeInSeconds)
		{
			BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
			pitchAmt.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
		}



		template<typename SampleType>
		StereoSample<SampleType> PitchShifter<SampleType>::processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
		{
			return StereoSample<SampleType>();
		}

		template<typename SampleType>
		void PitchShifter<SampleType>::updateSmoothedVariables()
		{
			BaseProcessingUnit<SampleType>::updateSmoothedVariables();

			pitchAmt.getNextValue();
		}

		//================================================================================================================================================================================================

			//================================================================================================================================================================================================


		template<typename SampleType>
		StereoPitchShifter<SampleType>::StereoPitchShifter()
		{
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::initShiftAmount(SampleType semitonesL, SampleType semitonesR)
		{
			pitchAmtL.setCurrentAndTargetValue(semitonesL);
			pitchAmtR.setCurrentAndTargetValue(semitonesR);
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::setShiftAmount(SampleType semitonesL, SampleType semitonesR)
		{
			pitchAmtL.setTargetValue(semitonesL);
			pitchAmtR.setTargetValue(semitonesR);
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
		{
			BaseProcessingUnit<SampleType>::prepare(spec);

			stretchL.presetCheaper(1, spec.sampleRate);
			stretchR.presetCheaper(1, spec.sampleRate);

			latencyComp.setLatency(getLatency());
			latencyComp.prepare(spec);
			reset();
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::reset()
		{
			BaseProcessingUnit<SampleType>::reset();
			stretchL.reset();
			stretchR.reset();
			latencyComp.reset();
		}
		template<typename SampleType>
		StereoSample<SampleType> StereoPitchShifter<SampleType>::processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
		{
			return StereoSample<SampleType>();
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::updateSmoothedVariables()
		{
			BaseProcessingUnit<SampleType>::updateSmoothedVariables();

			pitchAmtL.getNextValue();
			pitchAmtR.getNextValue();
		}
		template<typename SampleType>
		void StereoPitchShifter<SampleType>::setSmoothingTime(SampleType timeInSeconds)
		{
			BaseProcessingUnit<SampleType>::setSmoothingTime(timeInSeconds);
			pitchAmtL.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
			pitchAmtR.reset(BaseProcessingUnit<SampleType>::sampleRate, timeInSeconds);
		}

		template class PitchShifter<float>;
		template class PitchShifter<double>;

		template class StereoPitchShifter<float>;
		template class StereoPitchShifter<double>;
	} // namespace dsp
} // namespace bdsp
