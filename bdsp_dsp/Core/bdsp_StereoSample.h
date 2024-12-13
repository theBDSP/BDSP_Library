#pragma once
namespace bdsp
{
	namespace dsp
	{
		/**
		 * A basic struct that holds a left and right sample pair
		 */
		template <typename SampleType>
		struct StereoSample
		{
			/**
			 * Intializes left and right to 0
			 */
			StereoSample()
			{
				StereoSample(0, 0);
			}

			/**
			 * Initializes left and right to provided values
			 */
			StereoSample(SampleType leftSample, SampleType rightSample)
			{
				left = leftSample;
				right = rightSample;
			}

			/**
			 * Adds 2 stereo samples together and returns the result
			 */
			StereoSample<SampleType> operator+(const StereoSample<SampleType> other) const
			{
				return StereoSample(left + other.left, right + other.right);
			}

			/**
			 * Adds another stereo sample to this one
			 */
			void operator+=(const StereoSample<SampleType>& other)
			{
				left += other.left;
				right += other.right;
			}

			/**
			 * Subtracts another stereo sample from this one and returns the result
			 */
			StereoSample<SampleType> operator-(const StereoSample<SampleType> other) const
			{
				return StereoSample<SampleType>(left - other.left, right - other.right);
			}

			/**
			 * Subtracts another stereo sample from this one
			 */
			void operator-=(const StereoSample<SampleType>& other)
			{
				left -= other.left;
				right -= other.right;
			}


			//================================================================================================================================================================================================
			/**
			 * Adds a constant value to both left and right and returns the result
			 */
			StereoSample<SampleType> operator+(const SampleType constant) const
			{
				return StereoSample<SampleType>(left + constant, right + constant);
			}

			/**
			 * Adds a constant value to both left and right
			 */
			void operator+=(const SampleType constant)
			{
				left += constant;
				right += constant;
			}


			/**
			 * Subtracts a constant value from both left and right and returns the result
			 */
			StereoSample<SampleType> operator-(const SampleType constant) const
			{
				return StereoSample<SampleType>(left - constant, right - constant);
			}

			/**
			 * Subtracts a constant value from both left and right
			 */
			void operator-=(const SampleType constant)
			{
				left -= constant;
				right -= constant;
			}


			//================================================================================================================================================================================================

			/**
			 * Applies a constant gain to left and right and returns the result
			 */
			StereoSample<SampleType> operator* (const SampleType gain) const
			{
				return StereoSample<SampleType>(left * gain, right * gain);
			}

			/**
			 * Applies a constant gain to left and right
			 */
			void operator*=(const SampleType gain)
			{
				left *= gain;
				right *= gain;
			}


			/**
			 * Multiplies left and right with another stereo sample's left and right and returns the result
			 */
			StereoSample<SampleType> operator* (const StereoSample<SampleType> gain) const
			{
				return StereoSample<SampleType>(left * gain.left, right * gain.right);
			}

			/**
			 * Multiplies left and right with another stereo sample's left and right
			 */
			void operator*=(const StereoSample<SampleType> gain)
			{
				left *= gain.left;
				right *= gain.right;
			}

			//================================================================================================================================================================================================



			/**
			 * Divides left and right by a constant value and returns the result
			 */
			StereoSample<SampleType> operator/ (const SampleType constant) const
			{
				return StereoSample<SampleType>(left / constant, right / constant);
			}

			/**
			 * Divides left and right by a constant value
			 */
			void operator/=(const SampleType constant)
			{
				left /= constant;
				right /= constant;
			}


			/**
			 * Divides left and right by another stereo sample's left and right and returns the result
			 */
			StereoSample<SampleType> operator/ (const StereoSample<SampleType> gain) const
			{
				return StereoSample<SampleType>(left / gain.left, right / gain.right);
			}


			/**
			 * Divides left and right by another stereo sample's left and right
			 */
			void operator/=(const StereoSample<SampleType> gain)
			{
				left /= gain.left;
				right /= gain.right;
			}


			SampleType left, right;
		};


		/**
		 * Adds a constant value to both left and right and returns the result
		 */
		template<typename T>
		StereoSample<T> operator+ (const T& constant, const StereoSample<T>& smp)
		{
			return smp + constant;
		}

		/**
		 * Subtracts a constant value from both left and right and returns the result
		 */
		template<typename T>
		StereoSample<T> operator- (const T& constant, const StereoSample<T>& smp)
		{
			return StereoSample<T>(constant - smp.left, constant - smp.right);
		}

		/**
		 * Applies a constant gain to both left and right and returns the result
		 */
		template<typename T>
		StereoSample<T> operator* (const T& gain, const StereoSample<T>& smp)
		{
			return smp * gain;
		}

		/**
		 * Divides a constant value by left and right and returns the result
		 */
		template<typename T>
		StereoSample<T> operator/ (const T& gain, const StereoSample<T>& smp)
		{
			return StereoSample<T>(gain / smp.left, gain / smp.right);
		}
	} // namespace dsp
} // namespace bdsp