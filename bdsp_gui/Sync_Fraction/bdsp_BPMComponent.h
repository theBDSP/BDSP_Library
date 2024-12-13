#pragma once



namespace bdsp
{
	class BPMComponent;
	class BPMFollower : public juce::RangedAudioParameter::Listener, public juce::AsyncUpdater
	{
	public:
		explicit BPMFollower(juce::RangedAudioParameter* userParameter, juce::RangedAudioParameter* linkParameter);
		~BPMFollower() = default;
		void parameterValueChanged(int parameterIndex, float newValue) override;


		void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

		double getValue();
		double getHostValue();
		double getUserValue();
		void updateHostValue(juce::AudioPlayHead::PositionInfo& info);
		void updateUserValue(double newValue);
		void setNoHost();
		/*	void setValue(double newValue, bool toggle = true)
			{
				userParam->beginChangeGesture();
				userParam->setValueNotifyingHost(newValue);
				userParam->endChangeGesture();
			}*/

		void toggle(bool shouldFollowHost);

		bool isFollowingHost();
		bool hasValidHost();

		void handleAsyncUpdate() override;

		void setComponent(BPMComponent* componentToAdd);

		void removeComponent();

		juce::RangedAudioParameter* getUserParameter();
		juce::RangedAudioParameter* getLinkParameter();

	private:
		bool followsHost = true;
		bool validHost = true;
		juce::RangedAudioParameter* userParam;
		juce::RangedAudioParameter* linkParam;
		double userValue = 10, hostValue = 10;
		BPMComponent* linkedComponent = nullptr;

	};

	class BPMComponent : public Component, public GUI_Universals::Listener
	{
	public:
		BPMComponent(BPMFollower* follower, GUI_Universals* universalsToUse, AlertWindow* alertToUse, const NamedColorsIdentifier& slider, const NamedColorsIdentifier& buttonDown, const NamedColorsIdentifier& buttonUp);
		~BPMComponent();

		void setColors(const NamedColorsIdentifier& slider, const NamedColorsIdentifier& buttonDown, const NamedColorsIdentifier& buttonUp);
		void resized() override;


		void setNoHost();


		void attach(juce::AudioProcessorValueTreeState& stateToUse);

		double getValue();

		void setHostBPM(double BPM);


		bool isFollowingHost();



		NumberSlider* getHostSlider();
		NumberSlider* getUserSlider();

		class Listener : public juce::Slider::Listener
		{
		public:
			Listener()
			{
			}
			~Listener() noexcept override
			{

			}

			virtual void valueChanged() = 0;


			// Inherited via Listener
			virtual void sliderValueChanged(juce::Slider* slider) override;

		};

		void addListener(Listener* listenerToAdd);
		void removeListener(Listener* listenerToRemove);

		void updateListeners();

		void modifierKeysChanged(const juce::ModifierKeys& modifiers) override;



		MultiShapeButton button, inc, dec, fineInc, fineDec;
		BPMFollower* linkedFollower;
	private:
		juce::Array<Listener*> listeners;

		NumberSlider hostBPM, userBPM;

		GUI_Universals* universals;

		juce::ModifierKeys currentMods;

		NamedColorsIdentifier sliderColor, buttonDownColor, buttonUpColor;

		AlertWindow* alert = nullptr;

		// Inherited via Listener
		virtual void GUI_UniversalsChanged() override;
	};

}// namnepace bdsp
