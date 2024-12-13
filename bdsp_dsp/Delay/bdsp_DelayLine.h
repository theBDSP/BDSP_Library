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
				Successive samples in the delay line will be interpolated using a 3rd callingOrder
				Lagrange interpolator. This method incurs more computational overhead than
				linear interpolation but reduces the low-pass filtering effect whilst
				remaining amenable to real time delay modulation.
			*/
			struct Lagrange3rd {};

			/**
				Successive samples in the delay line will be interpolated using 1st callingOrder
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
			void updateReadPointer(int channel);

			/**
			 * Sets a new delay time in samples. This takes a float-type so that derived class that can handle fractional delay times can override this funciton
			 */
			virtual void setDelay(SampleType newValue);

			/**
			 * @returns The delay time in samples tuncated to an int
			 */
			int getDelayInt();


			//==============================================================================

			void processInternal(bool isBypassed) noexcept override
			{
				if (isBypassed)
				{
					return;
				}

				const auto& inputBlock = BaseProcessingUnit<SampleType>::internalDryBlock;
				const auto& outputBlock = BaseProcessingUnit<SampleType>::internalWetBlock;

				const auto numChannels = outputBlock.getNumChannels();
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

			inline SampleType processSample(int channel, const SampleType& inputSample) noexcept override
			{
				return SampleType();
			}

		protected:

			juce::AudioBuffer<SampleType> bufferData;
			std::vector<int> writePos, readPos;
			int delayInt = 0, totalSize = 4;
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


			SampleType getDelay() const;
			SampleType getTargetDelay() const;



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

			StereoSample<SampleType> pingPongPop();

			StereoSample<SampleType> pingPongPopUpdateRead();





			void updateSmoothedVariables() override;

			void setSmoothingTime(SampleType timeInSeconds) override;

			void setFeedback(SampleType newValue);
			void setPongMix(SampleType newValue);
			void setDelay(SampleType newValue) override;
			void snapDelay(SampleType newValue); // sets delay without smoothing



			void initFeedback(SampleType newValue);
			void initPongMix(SampleType newValue);




			//==============================================================================

			template <class ContextType = juce::dsp::ProcessContextReplacing<SampleType>>
			void process(const ContextType& context) noexcept
			{
				const auto& inputBlock = context.getInputBlock();
				auto& outputBlock = context.getOutputBlock();

				if (context.isBypassed || DelayLineBase<SampleType>::bypassed)
				{
					outputBlock.copyFrom(inputBlock);
					return;
				}


				const auto numChannels = outputBlock.getNumChannels();
				const auto numSamples = outputBlock.getNumSamples();


				jassert(inputBlock.getNumChannels() == numChannels);
				jassert(inputBlock.getNumSamples() == numSamples);


				for (size_t i = 0; i < numSamples; ++i)
				{
					inputSample = StereoSample<SampleType>(inputBlock.getSample(0, i), inputBlock.getSample(1, i));
					updateSmoothedVariables();
					outputSample = wet.processSampleStereo(processSampleStereo(inputSample)); // calculate each sample

					inputSample *= DelayLineBase<SampleType>::smoothedDryMix.getCurrentValue();
					outputBlock.setSample(0, i, inputSample.left + outputSample.left);
					outputBlock.setSample(1, i, inputSample.right + outputSample.right);

				}

			}

		private:

			void updateDelay();

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, SampleType>::type
				interpolateSample(int channel) const
			{
				int index = (DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt) & (DelayLineBase<SampleType>::totalSize - 1);

				return DelayLineBase<SampleType>::bufferData.getSample(channel, index);
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, SampleType>::type
				interpolateSample(int channel) const
			{

				auto index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt;
				auto index2 = index1 + 1;


				index1 = index1 & (DelayLineBase<SampleType>::totalSize - 1);
				index2 = index2 & (DelayLineBase<SampleType>::totalSize - 1);

				auto value1 = DelayLineBase<SampleType>::bufferData.getSample(channel, index1);
				auto value2 = DelayLineBase<SampleType>::bufferData.getSample(channel, index2);

				return value1 + delayFrac * (value2 - value1);
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, SampleType>::type
				interpolateSample(int channel) const
			{


				auto index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt;
				auto index2 = index1 + 1;
				auto index3 = index2 + 1;
				auto index4 = index3 + 1;


				index1 = index1 & (DelayLineBase<SampleType>::totalSize - 1);
				index2 = index2 & (DelayLineBase<SampleType>::totalSize - 1);
				index3 = index3 & (DelayLineBase<SampleType>::totalSize - 1);
				index4 = index4 & (DelayLineBase<SampleType>::totalSize - 1);

				auto* samples = DelayLineBase<SampleType>::bufferData.getReadPointer(channel);

				auto value1 = samples[index1];
				auto value2 = samples[index2];
				auto value3 = samples[index3];
				auto value4 = samples[index4];

				auto d1 = delayFrac - 1.0f;
				auto d2 = delayFrac - 2.0f;
				auto d3 = delayFrac - 3.0f;

				auto d32 = d3 * 0.5f;

				auto c4 = d1 * d2 / 6.0f;
				auto c1 = -c4 * d3;
				auto c2 = d2 * d32;
				auto c3 = -d1 * d32;

				//auto c1 = -d1 * d2 * d3 / 6.0f;
				//auto c2 = d2 * d3 * 0.5f;
				//auto c3 = -d1 * d3 * 0.5f;
				//auto c4 = d1 * d2 / 6.0f;

				return value1 * c1 + delayFrac * (value2 * c2 + value3 * c3 + value4 * c4);
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, SampleType>::type
				interpolateSample(int channel)
			{

				auto index1 = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt;
				auto index2 = index1 + 1;

				index1 = index1 & (DelayLineBase<SampleType>::totalSize - 1);
				index2 = index2 & (DelayLineBase<SampleType>::totalSize - 1);


				auto value1 = DelayLineBase<SampleType>::bufferData.getSample(channel, index1);
				auto value2 = DelayLineBase<SampleType>::bufferData.getSample(channel, index2);

				//auto output = delayFrac == 0 ? value1 : value2 + alpha * (value1 - v[(size_t)channel]);
				auto output = !delayFrac * value1 + !!delayFrac * (value2 + alpha * (value1 - v[(size_t)channel]));
				v[(size_t)channel] = output;

				return output;
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, SampleType>::type
				interpolateSample(int channel) const
			{

				SampleType x = DelayLineBase<SampleType>::readPos[(size_t)channel] + DelayLineBase<SampleType>::delayInt;
				int a = lookup->trigLookups->getLanczosA();
				SampleType L;
				SampleType sum = 0;
				int floorX = floor(x) + DelayLineBase<SampleType>::totalSize; // only used in modulo operation
				for (int i = -a + 1; i <= a; ++i)
				{
					SampleType smp = DelayLineBase<SampleType>::bufferData.getSample(channel, (floorX + i) & (DelayLineBase<SampleType>::totalSize - 1));

					L = lookup->trigLookups->lanczos(delayFrac + i);

					sum += smp * L;
				}
				return sum;
			}
			//==============================================================================
			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, void>::type
				updateInternalVariables()
			{
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, void>::type
				updateInternalVariables()
			{
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, void>::type
				updateInternalVariables()
			{
				delayFrac += !!(DelayLineBase<SampleType>::delayInt > 0);
				DelayLineBase<SampleType>::delayInt -= !!(DelayLineBase<SampleType>::delayInt > 0);

			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, void>::type
				updateInternalVariables()
			{
				delayFrac += !!(delayFrac < (SampleType)0.618 && DelayLineBase<SampleType>::delayInt > 0);
				DelayLineBase<SampleType>::delayInt -= !!(delayFrac < (SampleType)0.618 && DelayLineBase<SampleType>::delayInt > 0);


				alpha = (1 - delayFrac) / (1 + delayFrac);
			}


			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, void>::type
				updateInternalVariables()
			{
			}


			//================================================================================================================================================================================================


			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::None>::value, void>::type
				generateLookupTables()
			{
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, void>::type
				generateLookupTables()
			{
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lagrange3rd>::value, void>::type
				generateLookupTables()
			{
			}

			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Thiran>::value, void>::type
				generateLookupTables()
			{
			}


			template <typename T = InterpolationType>
			typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Lanczos>::value, void>::type
				generateLookupTables()
			{
				lookup->trigLookups;
			}




			//========================================================================================================================================================================================================================
		public:

			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::Basic>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));
				pushSample(0, inputSample.left);

				pushSample(1, inputSample.right);

				return poppedSample;
			}



			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::FBCF>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));
				pushSample(0, inputSample.left + poppedSample.left * smoothedFeedback.getCurrentValue());

				pushSample(1, inputSample.right + poppedSample.right * smoothedFeedback.getCurrentValue());


				return poppedSample;
			}

			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::LPFBCF>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				poppedSample = StereoSample<SampleType>(popSampleUpdateRead(0), popSampleUpdateRead(1));


				prevOutL = poppedSample.left + a * (prevOutL - poppedSample.left); //Fewer multiplication version of: Popped * (1 - a) + prevOut * a;
				pushSample(0, inputSample.left + prevOutL * smoothedFeedback.getCurrentValue());

				prevOutR = poppedSample.right + a * (prevOutR - poppedSample.right); //Fewer multiplication version of: Popped * (1 - a) + prevOut * a;
				pushSample(1, inputSample.right + prevOutR * smoothedFeedback.getCurrentValue());


				return poppedSample;
			}


			template <typename T = DelayType>
			typename std::enable_if <std::is_same <T, DelayTypes::PingPong>::value, StereoSample<SampleType>>::type
				processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				poppedSample = pingPongPopUpdateRead();

				pushSample(0, inputSample.left + poppedSample.left * smoothedFeedback.getCurrentValue());
				pushSample(1, inputSample.right + poppedSample.right * smoothedFeedback.getCurrentValue());

				return poppedSample;
			}




		protected:

			DSP_Universals<SampleType>* lookup;
			SampleType delay = 0.0, delayFrac = 0.0, alpha = 0.0, targetDelay = 0;
			std::vector<SampleType> v;

			SampleType prevOutL = 0, prevOutR = 0, interpolatedSample;
			SampleType a = 0.25; // for LPFBCF
			StereoSample<SampleType> inputSample, outputSample, poppedSample;


			juce::SmoothedValue<SampleType> smoothedFeedback, smoothedPongMix;
			StereoPanner<SampleType> wet;




		private:
			JUCE_LEAK_DETECTOR(DelayLine)
		};


	} // namespace dsp
} //namespace bdsp
