#pragma once


namespace bdsp
{
	namespace dsp
	{

		/**
		 * Silences audio at certain intervals for demo versions
		 */
		template <typename SampleType>
		class DemoTimer : public BaseProcessingUnit<SampleType>
		{
		public:

			DemoTimer()
			{

			}

			~DemoTimer()
			{

			}



			/**
			 * Sets the time intervals for silencing
			 * @param secondsNormal The amount of time before silencing is applied
			 * @param secondsSilence How long the silence lasts
			 */
			void setTimeIntervals(double secondsNormal, double secondsSilence)
			{

				std::function<void()> normalStart = [=]() // lambda that starts the normal-timer
				{
					normalTimer.startTimerMS(secondsNormal * 1000);
					BaseProcessingUnit<SampleType>::setBypassed(true);
				};

				std::function<void()> silenceStart = [=]() // lambda that starts the silence-timer
				{
					silenceTimer.startTimerMS(secondsSilence * 1000);
					BaseProcessingUnit<SampleType>::setBypassed(false);
				};


				// set each timer to start the other when they end
				silenceTimer.onTimerEnd = normalStart;
				normalTimer.onTimerEnd = silenceStart;

				// init the off timer and pause it
				silenceTimer.startTimerMS(secondsSilence * 1000);
				silenceTimer.pauseTimer();

				// start the on timer
				normalTimer.startTimerMS(secondsNormal * 1000);
				BaseProcessingUnit<SampleType>::setBypassed(true);

				// push new intervals to all listeners
				for (auto* l : listeners)
				{
					l->setNewTimes(secondsNormal, secondsSilence);
				}
			}


			void prepare(const juce::dsp::ProcessSpec& spec) override
			{
				BaseProcessingUnit<SampleType>::prepare(spec);
				silenceTimer.setSampleRate(spec.sampleRate);
				normalTimer.setSampleRate(spec.sampleRate);

			}

			void processInternal(bool isBypassed) noexcept override
			{

				runTimers(BaseProcessingUnit<SampleType>::internalWetBlock.getNumSamples()); // run the timers

				if (isBypassed || BaseProcessingUnit<SampleType>::bypassed) // will only be bypassed if set by the silenceTimer
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.copyFrom(BaseProcessingUnit<SampleType>::internalDryBlock);
					return;
				}
				else
				{
					BaseProcessingUnit<SampleType>::internalWetBlock.clear();
				}
			}

			//================================================================================================================================================================================================
			// Boilerplate listener code
			class Listener
			{
			public:

				Listener(DemoTimer<SampleType>* parent)
				{
					p = parent;
					p->addListener(this);


				}

				virtual ~Listener()
				{
					p->removeListener(this);
				}

				virtual void setNewTimes(double secondsOn, double secondsOff) = 0;



			protected:
				DemoTimer<SampleType>* p;



			};

			void addListener(Listener* listenerToAdd)
			{
				listeners.add(listenerToAdd);
			}
			void removeListener(Listener* listenerToRemove)
			{
				listeners.removeAllInstancesOf(listenerToRemove);
			}

			juce::Array<Listener*> listeners;
			//================================================================================================================================================================================================


			SampleTimer normalTimer, silenceTimer;
		private:


			void runTimers(size_t blockSize)
			{
				silenceTimer.runTimer(blockSize);
				normalTimer.runTimer(blockSize);
			}




			JUCE_LEAK_DETECTOR(DemoTimer)

		};





		template class DemoTimer<float>;
		template class DemoTimer<double>;

	}//namespace dsp
}// namnepace bdsp


