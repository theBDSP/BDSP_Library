#pragma once



namespace bdsp
{
	namespace dsp
	{

		namespace DelayTypes
		{
			struct Basic {}; // no feedback

			struct FBCF {}; // Feedback Comb Filter

			struct LPFBCF {}; // low pass FBCF

			struct PingPong {};

		}

		namespace DelayLineInterpolationTypes
		{
			/**
				No interpolation. Any fractional delay values will be rounded to the nearst integer.
			 */
			struct None {};

			/**
				Successive samples in the delay line will be linearly interpolated. This
				type of interpolation has a low compuational cost where the delay can be
				modulated in real time, but it also introduces a low-pass filtering effect
				into your audio signal.
			*/
			struct Linear {};

			/**
				Successive samples in the delay line will be interpolated using a 3rd order
				Lagrange interpolator. This method incurs more computational overhead than
				linear interpolation but reduces the low-pass filtering effect whilst
				remaining amenable to real time delay modulation.
			*/
			struct Lagrange3rd {};

			/**
				Successive samples in the delay line will be interpolated using 1st order
				Thiran interpolation. This method is very efficient, and features a flat
				amplitude frequency response in exchange for less accuracy in the phase
				response. This interpolation method is stateful so is unsuitable for
				applications requiring fast delay modulation.
			*/
			struct Thiran {};

			/**
				Interpolates using windowed sinc fucntion very high quality. This can be very expensive however.
			*/
			struct Lanczos {};
		}

		/**
		 * Very basic delay line. Provides no iterpolation for sub-sample delays. Lightweight alternative to the full class if you don't need that.
		 */
		template<typename SampleType>
		class DelayLineBase : public BaseProcessingUnit<SampleType>
		{
		public:

			/**
			 *  Creates an empty delay line of zero size.
			 *	You must set a max delay before you call prepare, otherwise you'll create a buffer of zero size in the prepare call.
			 */
			DelayLineBase();


			/**
			 * Creates a delay line and sets it maximum delay size
			 */
			DelayLineBase(int maxDelayInSamples);


			/**
			 *Sets the maximum possible delay
			 */
			void setMaxDelay(int numSamples);

			//==============================================================================

			void prepare(const juce::dsp::ProcessSpec& spec) override;


			void reset() override;

			/**
			 * Pop a single sample from the buffer
			 */
			virtual SampleType popSample(int channel);

			/**
			 * Pop a single sample from the buffer and increment the read pointer
			 */
			virtual SampleType popSampleUpdateRead(int channel);


			/**
			 * Push a single sample to the buffer
			 */
			void pushSample(int channel, SampleType smp);

			/**
			 * Push a single sample to the buffer and update the write pointer
			 */
			virtual void updateReadPointer(int channel);

			/**
			 * Sets a new delay time in samples. This takes a float-type so that derived class that can handle fractional delay times can override this funciton
			 */
			virtual void setDelay(int channel, SampleType newValue);

			/**
			 * @returns The delay time in samples tuncated to an int
			 */
			int getDelayInt(int channel);

			
			//==============================================================================

			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed)
				{
					return;
				}

				const auto& inputBlock = BaseProcessingUnit<SampleType>::internalDryBlock;
				const auto& outputBlock = BaseProcessingUnit<SampleType>::internalWetBlock;

				const auto numSamples = outputBlock.getNumSamples();



				for (size_t i = 0; i < numSamples; ++i)
				{
					auto inputSample = StereoSample<SampleType>(inputBlock.getSample(0, i), inputBlock.getSample(1, i));
					BaseProcessingUnit<SampleType>::updateSmoothedVariables();

					// push left and right sample into the buffer
					pushSample(0, inputSample.left);
					pushSample(1, inputSample.right);


					// pop from each channel in the buffer to the output block
					outputBlock.setSample(0, i, popSampleUpdateRead(0));
					outputBlock.setSample(1, i, popSampleUpdateRead(1));
				}
				BaseProcessingUnit<SampleType>::applyDryWetMix();
			}


		protected:

			juce::AudioBuffer<SampleType> bufferData;
			std::vector<int> writePos, readPos, delayInt;
			int totalSize = 4;
		};



		template <typename SampleType, typename DelayType, typename InterpolationType = DelayLineInterpolationTypes::None>
		class DelayLine : public DelayLineBase<SampleType>
		{
		public:

			/**
			 *  Creates an empty delay line of zero size.
			 *	You must set a max delay before you call prepare, otherwise you'll create a buffer of zero size in the prepare call.
			 */
			DelayLine();


			/**
			 * Creates a delay line, sets it maximum delay size, and stores a pointer to the DSP Globals struct (used for certain interpolation methods)
			 */
			DelayLine(int maxDelayInSamples, DSP_Universals<SampleType>* lookupToUse = nullptr);


			/**
			 * @return The current delay time in samples
			 */
			SampleType getDelay(int channel) const;

			/**
			 * @return The target delay time in samples to be reached after smoothing
			 */
			SampleType getTargetDelay(int channel) const;



			void prepare(const juce::dsp::ProcessSpec& spec) override;

			void reset() override;

			/**
			 * Pops and interpolates a delayed sample from the buffer
			 */
			SampleType popSample(int channel) override;


			/**
			 * Pops and interpolates a delayed sample from the buffer and increments the read pointer
			 */
			SampleType popSampleUpdateRead(int channel) override;

			/**
			 * Pops and interpolates a delayed stereo sample from the buffer, and inter-mixes the two channels according to the value of soothedPongMix
			 */
			StereoSample<SampleType> pingPongPop();


			/**
			 * Pops and interpolates a delayed stereo sample from the buffer, inter-mixes the two channels according to the value of soothedPongMix, and updates each channel's read pointer
			 */
			StereoSample<SampleType> pingPongPopUpdateRead();



			void updateReadPointer(int channel) override;

			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;
			void setDelaySmoothingTime(SampleType timeInSeconds);

			//================================================================================================================================================================================================

			void setFeedback(SampleType newValue);
			void setPongMix(SampleType newValue);
			void setDelay(int channel, SampleType newValue) override;

			/**
			 *  Sets the delay without smoothing
			 */
			void snapDelay(int channel, SampleType newValue);



			void initFeedback(SampleType newValue);
			void initPongMix(SampleType newValue);

			void setReversed(bool shouldBeReversed);


			SampleType getFeedback();
			SampleType getPongMix();
			bool getReversed();

			//================================================================================================================================================================================================

			void processInternal(bool isBypassed) noexcept override
			{

				if (isBypassed)
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}


				const auto numChannels = BaseProcessingUnit<SampleType>::internalWetBlock.getNumChannels();
				const auto numSamples = BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples();


				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumChannels() == numChannels);
				jassert(BaseProcessingUnit<SampleType>::internalDryBlock.getNumSamples() == numSamples);


				for (size_t i = 0; i < numSamples; ++i)
				{
					inputSample = StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i));
					updateSmoothedVariables();
					outputSample = outputPanner.processSampleStereo(processSampleStereo(inputSample)); // calculates the next stereo sample and processes it through the output panner

					inputSample *= DelayLineBase<SampleType>::smoothedDryMix.getCurrentValue();
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, inputSample.left + outputSample.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, inputSample.right + outputSample.right);

				}

			}

		protected:
			/**
			 * Grabs the next smoothed value of the delay and updates all the related variables to the new value
			 */
			virtual void updateDelay();

			virtual int getChannel(int channel) const
			{
				return channel;
			}

		private:
			//================================================================================================================================================================================================
			// Interpolate Sample Implementations

			/**
			 * No interpolation
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, SampleType>::type
				interpolateSample(int channel) const
			{
				int index;
				if (reversed)
				{
					index = (reversePos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel]) % DelayLineBase<SampleType>::totalSize;
				}
				else
				{
					index = (DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel]) % DelayLineBase<SampleType>::totalSize;
				}

				return DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), index);
			}

			/**
			 * Linear interpolation
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, SampleType>::type
				interpolateSample(int channel) const
			{

				int index1;
				if (reversed)
				{
					index1 = reversePos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}
				else
				{
					index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}


				auto index2 = index1 + 1;


				index1 = index1 % DelayLineBase<SampleType>::totalSize;
				index2 = index2 % DelayLineBase<SampleType>::totalSize;

				SampleType value1 = DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), index1);
				SampleType value2 = DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), index2);

				return value1 + delayFrac[(size_t)channel] * (value2 - value1);
			}

			/**
			 * Lagrange interpolation
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, SampleType>::type
				interpolateSample(int channel) const
			{


				int index1;
				if (reversed)
				{
					index1 = reversePos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}
				else
				{
					index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}
				auto index2 = index1 + 1;
				auto index3 = index2 + 1;
				auto index4 = index3 + 1;


				index1 = index1 % DelayLineBase<SampleType>::totalSize;
				index2 = index2 % DelayLineBase<SampleType>::totalSize;
				index3 = index3 % DelayLineBase<SampleType>::totalSize;
				index4 = index4 % DelayLineBase<SampleType>::totalSize;

				auto* samples = DelayLineBase<SampleType>::bufferData.getReadPointer(getChannel(channel));

				auto value1 = samples[index1];
				auto value2 = samples[index2];
				auto value3 = samples[index3];
				auto value4 = samples[index4];

				auto d1 = delayFrac[(size_t)channel] - 1.0f;
				auto d2 = delayFrac[(size_t)channel] - 2.0f;
				auto d3 = delayFrac[(size_t)channel] - 3.0f;


				/*
				 * We do a bit of rearranging to spare a few multiplications.
				 * For clarity, these are the numbers we are calculating.
					c1 = -d1 * d2 * d3 / 6.0f;
					c2 = d2 * d3 * 0.5f;
					c3 = -d1 * d3 * 0.5f;
					c4 = d1 * d2 / 6.0f;
				 */

				auto d32 = d3 * 0.5f;

				auto c4 = d1 * d2 / 6.0f;
				auto c1 = -c4 * d3;
				auto c2 = d2 * d32;
				auto c3 = -d1 * d32;


				return value1 * c1 + delayFrac[(size_t)channel] * (value2 * c2 + value3 * c3 + value4 * c4);
			}

			/**
			 * Thiran interpolation
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, SampleType>::type
				interpolateSample(int channel)
			{

				int index1;
				if (reversed)
				{
					index1 = reversePos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}
				else
				{
					index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel];
				}
				auto index2 = index1 + 1;

				index1 = index1 % DelayLineBase<SampleType>::totalSize;
				index2 = index2 % DelayLineBase<SampleType>::totalSize;


				auto value1 = DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), index1);
				auto value2 = DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), index2);

				/*
				* To avoid branching from the following, we replace the ternary operator with a few bool conversions and multiplies
				* output = (delayFrac == 0) ? value1 : value2 + alpha * (value1 - v[(size_t)channel]);
				*/
				auto output = !delayFrac[(size_t)channel] * value1 + !!delayFrac[(size_t)channel] * (value2 + alpha[(size_t)channel] * (value1 - v[(size_t)channel]));
				v[(size_t)channel] = output;

				return output;
			}

			/**
			 * Lanczos interpolation
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, SampleType>::type
				interpolateSample(int channel) const
			{

				SampleType index;
				if (reversed)
				{
					index = (reversePos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel]);
				}
				else
				{
					index = (DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt[(size_t)channel]);
				}
				int a = lookup->trigLookups->getLanczosA();
				SampleType L;
				SampleType sum = 0;
				int floorIndex = floor(index) + DelayLineBase<SampleType>::totalSize; // only used in modulo operation
				for (int i = -a + 1; i <= a; ++i)
				{
					SampleType smp = DelayLineBase<SampleType>::bufferData.getSample(getChannel(channel), (floorIndex + i) % DelayLineBase<SampleType>::totalSize);

					L = lookup->trigLookups->lanczos(delayFrac[(size_t)channel] + i);

					sum += smp * L;
				}
				return sum;
			}

			//================================================================================================================================================================================================
			// Update Internal Variable Implementations

			/**
			 * None
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, void>::type
				updateInternalVariables()
			{
			}

			/**
			 * Linear
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, void>::type
				updateInternalVariables()
			{
			}


			/**
			 * Lagrange
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, void>::type
				updateInternalVariables()
			{
				for (size_t c = 0; c < delayFrac.size(); ++c)
				{
					delayFrac[c] += (DelayLineBase<SampleType>::delayInt[c] > 0);
					DelayLineBase<SampleType>::delayInt[c] -= (DelayLineBase<SampleType>::delayInt[c] > 0);
				}

			}

			/**
			 * Thiran
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, void>::type
				updateInternalVariables()
			{
				for (size_t c = 0; c < delayFrac.size(); ++c)
				{
					delayFrac[c] += (delayFrac[c] < (SampleType)0.618 && DelayLineBase<SampleType>::delayInt[c] > 0);
					DelayLineBase<SampleType>::delayInt[c] -= (delayFrac[c] < (SampleType)0.618 && DelayLineBase<SampleType>::delayInt[c] > 0);


					alpha[c] = (1 - delayFrac[c]) / (1 + delayFrac[c]);
				}
			}


			/**
			 * Lanczos
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, void>::type
				updateInternalVariables()
			{
			}


			//================================================================================================================================================================================================
			// Generate Lookup Table Implementations


			/**
			 * None
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, void>::type
				generateLookupTables()
			{
			}

			/**
			 * Linear
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, void>::type
				generateLookupTables()
			{
			}

			/**
			 * Lagrange
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, void>::type
				generateLookupTables()
			{
			}

			/**
			 * Thiran
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, void>::type
				generateLookupTables()
			{
			}


			/**
			 * Lanczos
			 */
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, void>::type
				generateLookupTables()
			{
				lookup->trigLookups.operator->();
			}

			//========================================================================================================================================================================================================================
			// Process Sample Stereo Implementations
		public:

			/**
			 * Basic
			 * Delays each channel and outputs those results to the same channel
			 */
			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::Basic>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				auto poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));
				this->pushSample(0, inputSample.left);

				this->pushSample(1, inputSample.right);

				return poppedSample;
			}


			/**
			 * FBCF (Feedback Comb Filter)
			 * Delays each channel, pushes the results back into the buffer and outputs the results to the same channel
			 */
			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::FBCF>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				auto poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));
				this->pushSample(0, inputSample.left + poppedSample.left * smoothedFeedback.getCurrentValue());

				this->pushSample(1, inputSample.right + poppedSample.right * smoothedFeedback.getCurrentValue());


				return poppedSample;
			}

			/**
			 * LPFBCF (Low-Pass Feedback Comb Filter)
			 * Delays each channel, pushes the results mixed with the last sample's results back into the buffer and outputs the results to the same channel
			 */

			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::LPFBCF>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				auto poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));


				prevOutL = poppedSample.left + a * (prevOutL - poppedSample.left); //Fewer multiplication version of: Popped * (1 - a) + prevOut * a;
				this->pushSample(0, inputSample.left + prevOutL * smoothedFeedback.getCurrentValue());

				prevOutR = poppedSample.right + a * (prevOutR - poppedSample.right); //Fewer multiplication version of: Popped * (1 - a) + prevOut * a;
				this->pushSample(1, inputSample.right + prevOutR * smoothedFeedback.getCurrentValue());


				return poppedSample;
			}


			/**
			 * PingPong
			 * Delays each channel, inter-mixes the resulting channels, pushes the results back into the buffer, and outputs the results to the same channel
			 */

			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::PingPong>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				auto poppedSample = pingPongPopUpdateRead();

				this->pushSample(0, inputSample.left + poppedSample.left * smoothedFeedback.getCurrentValue());
				this->pushSample(1, inputSample.right + poppedSample.right * smoothedFeedback.getCurrentValue());

				return poppedSample;
			}




		protected:

			DSP_Universals<SampleType>* lookup;
			std::vector<SampleType> delay, delayFrac, targetDelay;
			std::vector<SampleType> v;

			SampleType delaySmoothingTime = 0.25;

			SampleType prevOutL = 0, prevOutR = 0, interpolatedSample;
			std::vector<SampleType>  alpha; // for Thiran
			SampleType a = 0.25; // for LPFBCF
			StereoSample<SampleType> inputSample, outputSample;


			juce::SmoothedValue<SampleType> smoothedFeedback, smoothedPongMix;
			StereoPanner<SampleType> outputPanner;

			std::vector<int> reversePos, reverseOffset;

			bool reversed = false;


		private:
			JUCE_LEAK_DETECTOR(DelayLine)
		};


	} // namespace dsp
} //namespace bdsp
