#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * Base struct for all different types of distortion.
		 * To define a new type of distortion derive from this class, make sure to add an instance of the derived class into the map of distortion types in the distortion lookups class,
		 * and initialize it with a unique static name.
		 */
		template<typename T>
		struct DistortionTypeBase
		{
			/**
			 * Creates and initializes a distortion type.
			 * @param RMSValues The RMS (amplitude, not dB) values of white noise processed by this distortion type. Each element represents the RMS value for a particular distortion amount.
			 * In the future I'm looking to add a tool that can automatically measure and return these values for a given tranfer function.
			 * @param iconData The path data created from the Projucer svg path converter tool.
			 * @param iconDataSize The size of the iconData array.
			 */
			template<typename floatType>
			DistortionTypeBase(const juce::Array<floatType>& RMSValues, const unsigned char* iconData, size_t iconDataSize)
			{
				T minVal = arrayMin(RMSValues);
                
				/**
				 * Returns the reciprocal (normalized to the minimum of all RMS Values) of the RMS gain for a given distortion amount.
				*/
				const std::function<T(T)> CompensationGainFunc = [=](T k)
				{
					int idx = juce::jmap(k, T(0), T(RMSValues.size() - 1)); // Index of RMSValues for a the distortion amount k

					return minVal / RMSValues[idx];
				};
				CompensationGain.initialise(CompensationGainFunc, 0, 1, RMSValues.size()); // intialize lookup table with 1 point per element in the RMSValues array. Interpolation between array elemnts is handled by the lookup table.
				//================================================================================================================================================================================================
				setIcon(iconData, iconDataSize);
			}

			virtual ~DistortionTypeBase() = default;


			/**
			 * Sets the path to use as the icon for this distortion type. Provide the data for the stroked version, the filled version will be automatically created.
			 * @param iconData The path data created from the Projucer svg path converter tool.
			 * @param iconDataSize The size of the iconData array.
			 */
			void setIcon(const unsigned char* iconData, size_t iconDataSize)
			{
				iconOutline.clear();
				iconOutline.loadPathFromData(iconData, iconDataSize);

				auto bounds = iconOutline.getBounds();
				iconFill = iconOutline;

				// Encloses the path by extending the end of the path to the y-center, adding a line throught the center, and closing the path
				if (iconFill.getPointAlongPath(0).x < iconFill.getCurrentPosition().x) // L to R
				{
					iconFill.lineTo(bounds.getRelativePoint(1.0, 0.5));
					iconFill.lineTo(bounds.getRelativePoint(0.0, 0.5));
				}
				else // R to L
				{
					iconFill.lineTo(bounds.getRelativePoint(0.0, 0.5));
					iconFill.lineTo(bounds.getRelativePoint(1.0, 0.5));
				}
				iconFill.closeSubPath();
			}

			juce::Path getIcon(bool isFill)
			{
				return isFill ? iconFill : iconOutline;
			}

			/**
			 * Override this in you derived class to implement your distortion algorithm
			 */
			virtual T processSample(T in, T amt, bool isScaled) = 0;


		protected:
			juce::dsp::LookupTableTransform<T> TransferFunction; // lookup table for transfer curve of distortion type (may be unused in some types that do not limit the output to [-1,1])
			juce::dsp::LookupTableTransform<T> CompensationGain; // lookup table for gain to apply to maintain consistent output amplitude as the distortion amount varies




		private:
			juce::Path iconOutline, iconFill;
		};


	} //namespace dsp
} //namespace bdsp
