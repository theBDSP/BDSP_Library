#include "bdsp_DistortionLookups.h"

namespace bdsp
{
	namespace dsp
	{


		template<typename T>
		inline DistortionLookups<T>::DistortionLookups()
			:flagMap(std::initializer_list<DistortionType>{
				DistortionType("Drive", &DistortionLookups<T>::generateArcSinHDistortionTables, &DistortionLookups<T>::getArcSinHDistortion),
				DistortionType("BitCrush", &DistortionLookups<T>::generateBitCrushDistortionTables, &DistortionLookups<T>::getBitCrushDistortion),
				DistortionType("HardClip", &DistortionLookups<T>::generateHardClipTables, &DistortionLookups<T>::getHardClipDistortion),
				DistortionType("SoftClip", &DistortionLookups<T>::generateSoftClipTables, &DistortionLookups<T>::getSoftClipDistortion),
				DistortionType("Saturation", &DistortionLookups<T>::generateTanHDistortionTables, &DistortionLookups<T>::getTanHDistortion),
				DistortionType("Arc Tan", &DistortionLookups<T>::generateArcTanDistortionTables, &DistortionLookups<T>::getArcTanDistortion),
				DistortionType("Inverse Proportion", &DistortionLookups<T>::generateInverseProportionDistortionTables, &DistortionLookups<T>::getInverseProportionDistortion),
				DistortionType("Inverse Power", &DistortionLookups<T>::generateInversePowerDistortionTables, &DistortionLookups<T>::getInversePowerDistortion),
				DistortionType("Wave Fold", &DistortionLookups<T>::generateSineWaveFoldDistortionTables, &DistortionLookups<T>::getSineWaveFoldDistortion),
				DistortionType("Tri Wave Fold", &DistortionLookups<T>::generateTriWaveFoldDistortionTables, &DistortionLookups<T>::getTriWaveFoldDistortion),
				DistortionType("Half Clip", &DistortionLookups<T>::generatePositiveClipDistortionTables, &DistortionLookups<T>::getPositiveClipDistortion),
				DistortionType("Bump", &DistortionLookups<T>::generateDoubleBumpDistortionTables, &DistortionLookups<T>::getDoubleBumpDistortion),
				DistortionType("Pinch", &DistortionLookups<T>::generateTanHPinchDistortionTables, &DistortionLookups<T>::getTanHPinchDistortion)
				})
		{

		}

	}// namespace dsp

}//namespace bdsp
