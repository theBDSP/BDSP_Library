#pragma once
namespace bdsp
{
	namespace dsp
	{
		template<typename SampleType, typename InterpolationType = DelayLineInterpolationTypes::Lagrange3rd>
		class SwingShuffleDelay : public BaseProcessingUnit<SampleType>
		{
		public:
			SwingShuffleDelay(int maxDelayInSamples, SampleType maxSwingShuffleAmt = 0.75, DSP_Universals<SampleType>* lookupToUse = nullptr);

			void setMaxDelay(int numSamples);

			SampleType getDelay(int channel) const;

			SampleType getTargetDelay(int channel) const;


			void updateSmoothedVariables() override;

			void prepare(const juce::dsp::ProcessSpec& spec) override;

			void reset() override;

			/**
			 * @brief 0.5 is straight, <0.5 shuffled, >0.5 swung
			 */
			void setSwingShuffleAmount(SampleType newValue);

			void setSmoothingTime(SampleType timeInSeconds) override;
			void setDelaySmoothingTime(SampleType timeInSeconds);

			//================================================================================================================================================================================================

			void setFeedback(SampleType newValue);
			void setPongMix(SampleType newValue);
			void setDelay(int channel, SampleType newValue);

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

				StereoSample<SampleType> inputSample, outputSample;

				for (size_t i = 0; i < numSamples; ++i)
				{
					inputSample = StereoSample<SampleType>(BaseProcessingUnit<SampleType>::internalDryBlock.getSample(0, i), BaseProcessingUnit<SampleType>::internalDryBlock.getSample(1, i));
					updateSmoothedVariables();
					outputSample = processSampleStereo(inputSample); // calculates the next stereo sample and processes it through the output panner

					BaseProcessingUnit<SampleType>::applyDryWetMix(inputSample, outputSample);

					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(0, i, inputSample.left + outputSample.left);
					BaseProcessingUnit<SampleType>::internalWetBlock.setSample(1, i, inputSample.right + outputSample.right);
				}

			}



			StereoSample<SampleType> processSampleStereo(const StereoSample<SampleType>& inputSample) noexcept
			{
				auto poppedSample2 = StereoSample<SampleType>(delay2.popSampleUpdateRead(0), delay2.popSampleUpdateRead(1));

				delay1.pushSample(0, inputSample.left + poppedSample2.left * delay1.getFeedback());
				delay1.pushSample(1, inputSample.right + poppedSample2.right * delay1.getFeedback());

				auto poppedSample1 = delay1.pingPongPopUpdateRead();
				delay2.pushSample(0, poppedSample1.left);
				delay2.pushSample(1, poppedSample1.right);


				return poppedSample1 + poppedSample2;

			}

		private:
			DelayLine<SampleType, DelayTypes::PingPong, InterpolationType> delay1, delay2;

			SampleType a = 0.25; // for LPFBCF

			SampleType prevOutL = 0, prevOutR = 0;
			std::vector<SampleType> baseDelay;
			SampleType swingShuffleAmount = 0.5;
			SampleType maxSwingShuffleAmount = 0.75;
		};


	} //namespace dsp
} //namespace bdsp