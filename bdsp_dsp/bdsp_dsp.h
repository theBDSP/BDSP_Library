#pragma once


#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"
#include "juce_data_structures/juce_data_structures.h"



#include <cstdarg>

#define BDSP_CONTROL_SAMPLE_RATE 500



namespace bdsp
{
	template <typename SampleType>
	class RandomOfType
	{

	public:
		template <typename T = SampleType>
		typename std::enable_if<std::is_same<T, float>::value, SampleType>::type
			getNext()
		{
			return rand.nextFloat();
		}

		template <typename T = SampleType>
		typename std::enable_if<std::is_same<T, double>::value, SampleType>::type
			getNext()
		{
			return  rand.nextDouble();
		}


	private:
		juce::Random rand;
	};


	


	namespace dsp
	{
	

	}

	// namespace dsp
} // namespace bdsp


//================================================================================================================================================================================================
//Distortion Types

#include "Distortion/Types/bdsp_DistortionTypeBase.h"
#include "Distortion/Types/bdsp_BitCrush.h"
#include "Distortion/Types/bdsp_Bump.h"
#include "Distortion/Types/bdsp_Drive.h"
#include "Distortion/Types/bdsp_Fuzz.h"
#include "Distortion/Types/bdsp_HardClip.h"
#include "Distortion/Types/bdsp_Pinch.h"
#include "Distortion/Types/bdsp_Power.h"
#include "Distortion/Types/bdsp_Saturation.h"
#include "Distortion/Types/bdsp_SinFold.h"
#include "Distortion/Types/bdsp_SoftClip.h"
#include "Distortion/Types/bdsp_Tape.h"
#include "Distortion/Types/bdsp_TriFold.h"
#include "Distortion/Types/bdsp_Tube.h"

//=====================================================================================================================
//Core
#include "Math/bdsp_MultiDimensionalLookup.h"
#include "Math/Lookups/bdsp_TrigLookups.h"
#include "Math/Lookups/bdsp_DistortionLookups.h"
#include "Math/Lookups/bdsp_PanningLookups.h"
#include "Math/Lookups/bdsp_WaveLookUps.h"
#include "Math/Lookups/bdsp_ChorusLookups.h"
#include "Core/bdsp_DSPUniversals.h"

#include "Core/bdsp_StereoSample.h"
#include "Core/bdsp_SampleSource.h"
#include "Core/bdsp_BaseProcessingUnit.h"
#include "Core/bdsp_LatencyCompensator.h"
#include "Core/bdsp_Oversampling.h"
#include "Core/bdsp_SampleRateReducer.h"

#include "Core/bdsp_SampleTimer.h"
#include "Core/bdsp_DemoTimer.h"

//================================================================================================================================================================================================
//Stereo
#include "Stereo/bdsp_StereoPanner.h"


#include "Core/bdsp_ProcessorChain.h"




//================================================================================================================================================================================================
//Distortion
#include "Distortion/bdsp_DistortionBase.h"
#include "Distortion/bdsp_BitCrusherDistortion.h"


#include "Distortion/bdsp_VariableDistortion.h"


//================================================================================================================================================================================================
//Dynamics
#include "Dynamics/bdsp_DynamicsBase.h"
#include "Dynamics/bdsp_Compressor.h"


//================================================================================================================================================================================================
//Filter
#include "Filter/bdsp_FilterBase.h"
#include "Filter/bdsp_BiQuad.h"
#include "Filter/bdsp_AllpassFilter.h"
#include "Filter/bdsp_ParametricEQ.h"



//=====================================================================================================================
//Misc

#include "Math/bdsp_Matrix.h"
#include "Math/bdsp_SmoothedMatrix.h"



//=====================================================================================================================
//Delay
#include "Delay/bdsp_DelayLine.h"

#include "Stereo/bdsp_StereoWidener.h"
//=====================================================================================================================
//Modulation
#include "Modulation/bdsp_Chorus.h"
#include "Modulation/bdsp_Flanger.h"
#include "Modulation/bdsp_Phaser.h"

//================================================================================================================================================================================================
//Pitch
#include "Pitch/bdsp_Pitch_Shifter.h"



//================================================================================================================================================================================================
// Generators
#include "Generators/bdsp_GeneratorBase.h"
#include "Generators/bdsp_BasicWaveGenerator.h"
#include "Generators/Noise/bdsp_NoiseGenerator.h"



//================================================================================================================================================================================================
//Multi Signal
#include "Multi_Signal/bdsp_MultiSignalProcessorBase.h"
#include "Multi_Signal/bdsp_RingModulation.h"






