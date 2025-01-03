#pragma once
namespace bdsp
{
	namespace dsp
	{

		template<typename T>
		class DistortionLookups
		{
		public:

			DistortionLookups()
			{
				// Add every distortion type to the map
				types.add(DistortionTypes::BitCrush<T>::Name, new DistortionTypes::BitCrush<T>());
				types.add(DistortionTypes::Bump<T>::Name, new DistortionTypes::Bump<T>());
				types.add(DistortionTypes::Drive<T>::Name, new DistortionTypes::Drive<T>());
				types.add(DistortionTypes::Fuzz<T>::Name, new DistortionTypes::Fuzz<T>());
				types.add(DistortionTypes::HardClip<T>::Name, new DistortionTypes::HardClip<T>());
				types.add(DistortionTypes::Pinch<T>::Name, new DistortionTypes::Pinch<T>());
				types.add(DistortionTypes::Power<T>::Name, new DistortionTypes::Power<T>());
				types.add(DistortionTypes::Saturation<T>::Name, new DistortionTypes::Saturation<T>());
				types.add(DistortionTypes::SinFold<T>::Name, new DistortionTypes::SinFold<T>());
				types.add(DistortionTypes::SoftClip<T>::Name, new DistortionTypes::SoftClip<T>());
				types.add(DistortionTypes::Tape<T>::Name, new DistortionTypes::Tape<T>());
				types.add(DistortionTypes::TriFold<T>::Name, new DistortionTypes::TriFold<T>());
				types.add(DistortionTypes::Tube<T>::Name, new DistortionTypes::Tube<T>());
			}
			virtual ~DistortionLookups() = default;

			inline DistortionTypeBase<T>* nameToDistortionType(const juce::String& name)
			{
				jassert(types.contains(name)); // if you hit this assertion, you are either providing a name that does not belong to a distortion type, or you did not add the type to the map
				return types.get(name);
			}

		private:

			OwnedMap<juce::String, DistortionTypeBase<T>, HashFunctions::String> types;

			JUCE_LEAK_DETECTOR(DistortionLookups)
		};

	}// namespace dsp

}//namespace bdsp
