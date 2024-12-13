#pragma once

namespace bdsp
{
	namespace dsp
	{
		// counts samples and calls function at the beggining of the block where the count would be hit

		/**
		 * Executes a lambda after a certain number of audio samples. This can't call the lambda more than once per block it processes, but if you need to call a function that often on the audio thread you shouldn't be using this class anyways.
		 */
		class SampleTimer
		{
		public:
			SampleTimer()
			{
			}

			~SampleTimer()
			{

			}

			/**
			 * Arms the timer and sets how long to wait before calling the lambda 
			 * @param numberOfSamples The number of samples to wait before calling the lambda
			 */
			void startTimer(juce::uint64 numberOfSamples)
			{
				count = 0;
				target = numberOfSamples;
				shouldRun = true;
			}

			/**
			 * Starts the timer without setting a new time
			 */
			void restartTimer()
			{
				count = 0;
				shouldRun = true;
			}

			/**
			 * Arms the timer and sets how long to wait before calling the lambda
			 * @param numberOfMS How long to wait in milliseconds before calling the lambda
			 */
			void startTimerMS(double numberOfMS)
			{
				startTimer(numberOfMS * sampleRate / 1000.0);
			}


			/**
			 * Tests if the timer would end within a certain amount of samples and calls the lambda if it would
			 * @param blockSize The number of samples to test
			 */
			void runTimer(size_t blockSize)
			{
				if (count < target && shouldRun) // only keep counting if we haven't hit the target yet and are allowed to run the timer
				{
					count += blockSize;
					if (count >= target && onTimerEnd.operator bool()) // if the count is hit and there is a valid funciton to call
					{
						onTimerEnd();
					}
				}

			}

			/**
			 * Stops the timer without resetting the count
			 */
			void pauseTimer()
			{
				shouldRun = false;
			}

			bool isRunning()
			{
				return!(count >= target) && shouldRun;
			}

			juce::uint64 getTimeInSamples()
			{
				return count;
			}

			double getTimeInMs()
			{
				return count / sampleRate * 1000.0;
			}
	

			void setSampleRate(double newSampleRate)
			{
				sampleRate = newSampleRate;
			}


			std::function<void()> onTimerEnd; // the lambda to call at the end of the timer

		private:

			juce::uint64 count = 0, target = 0;
			double sampleRate = 44100;
			bool shouldRun = false;
			JUCE_LEAK_DETECTOR(SampleTimer)

		};


	}// namespace dsp
}// namnepace bdsp


