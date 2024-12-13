#pragma once
#if BDSP_DEMO_VERSION

namespace bdsp
{
	class Editor;
	template <typename SampleType>
	class DemoAlert : public dsp::DemoTimer<SampleType>::Listener
	{

	public:

        DemoAlert(Editor* editorToUse, dsp::DemoTimer<SampleType>* timer);
		~DemoAlert()
		{
		}


        void changeListenerCallback(juce::ChangeBroadcaster* source) override;

		void setNewTimes(double secondsOn, double secondsOff) override
		{
			Body = "The demo version of " + juce::String(BDSP_APP_NAME) + " will mute all audio for " + juce::String(secondsOff) + " seconds every " + juce::String(secondsOn) + " seconds";
		}

	

		juce::String Header, Body;
	private:

		Editor* editor;
	};

} // namespace bdsp
#endif