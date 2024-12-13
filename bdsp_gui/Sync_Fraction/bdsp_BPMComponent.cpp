#include "bdsp_BPMComponent.h"

namespace bdsp
{

	BPMComponent::BPMComponent(BPMFollower* follower, GUI_Universals* universalsToUse, AlertWindow* alertToUse, const NamedColorsIdentifier& slider, const NamedColorsIdentifier& buttonDown, const NamedColorsIdentifier& buttonUp)
		:Component(universalsToUse),
		hostBPM(this, universalsToUse, "Host BPM"),
		userBPM(this, universalsToUse, "User BPM"),
		button(universalsToUse, true),
		inc(universalsToUse),
		dec(universalsToUse),
		fineInc(universalsToUse),
		fineDec(universalsToUse),
		currentMods(juce::ModifierKeys()),
		GUI_Universals::Listener(universalsToUse)
	{
		;
		alert = alertToUse;

		sliderColor = slider;
		buttonDownColor = buttonDown;
		buttonUpColor = buttonUp;

		linkedFollower = follower;
		linkedFollower->setComponent(this);

		addAndMakeVisible(button);
		universals = universalsToUse;

		button.setClickingTogglesState(true);
		button.onClick = [=]()
		{
			userBPM.setVisible(!button.getToggleState());
			userBPM.numbers.setVisible(userBPM.isVisible());

			hostBPM.setVisible(button.getToggleState());
			hostBPM.numbers.setVisible(hostBPM.isVisible());
			repaint();
			linkedFollower->toggle(button.getToggleState());

			inc.setEnabled(!button.getToggleState());
			fineInc.setEnabled(!button.getToggleState());
			dec.setEnabled(!button.getToggleState());
			fineDec.setEnabled(!button.getToggleState());
			updateListeners();
		};


		//================================================================================================================================================================================================

		inc.setRepeatSpeed(250, 50);
		dec.setRepeatSpeed(250, 50);
		fineInc.setRepeatSpeed(250, 50);
		fineDec.setRepeatSpeed(250, 50);

		inc.setTriggeredOnMouseDown(true);
		dec.setTriggeredOnMouseDown(true);
		fineInc.setTriggeredOnMouseDown(true);
		fineDec.setTriggeredOnMouseDown(true);

		inc.onClick = [=]()
		{
			userBPM.setValue(userBPM.getValue() + 1);
		};
		dec.onClick = [=]()
		{
			userBPM.setValue(userBPM.getValue() - 1);
		};
		fineInc.onClick = [=]()
		{
			userBPM.setValue(userBPM.getValue() + 0.1);
		};
		fineDec.onClick = [=]()
		{
			userBPM.setValue(userBPM.getValue() - 0.1);
		};
		addAndMakeVisible(inc);
		addAndMakeVisible(dec);
		addAndMakeVisible(fineInc);
		addAndMakeVisible(fineDec);






		hostBPM.setEnabled(false);
		hostBPM.numbers.setEditable(false);

		button.setToggleState(true, juce::sendNotification); // inits to false, need this to trigger state change callback
		button.setToggleState(linkedFollower->isFollowingHost(), juce::sendNotification);
		if (!linkedFollower->hasValidHost())
		{
			setNoHost();
		}



		//		hostBPM.setLookAndFeel(&universalsToUse->MasterBaseLNF);
		//		userBPM.setLookAndFeel(&universalsToUse->MasterBaseLNF);

		//		hostBPM.numbers.setLookAndFeel(&universalsToUse->MasterBPMComponentLNF);
		//		userBPM.numbers.setLookAndFeel(&universalsToUse->MasterBPMComponentLNF);

		//		button.setLookAndFeel(&universalsToUse->MasterBaseLNF);

		hostBPM.numbers.setJustificationType(juce::Justification::centredBottom);
		userBPM.numbers.setJustificationType(juce::Justification::centredBottom);

		hostBPM.numbers.setFontIndex(0);
		userBPM.numbers.setFontIndex(0);

		setColors(sliderColor, buttonDownColor, buttonUpColor);

		userBPM.onValueChange = [=]
		{
			linkedFollower->updateUserValue(userBPM.getValue());
		};

		setWantsKeyboardFocus(true);

		button.setToggleState(follower->isFollowingHost(), juce::sendNotification);
		hostBPM.setValue(follower->getHostValue());
		userBPM.setValue(follower->getUserValue());


		userBPM.snapToValueFunction = [=](double val, juce::Slider::DragMode m)
		{

			auto out = juce::jlimit(userBPM.getMinimum(), userBPM.getMaximum(), round(val));
			jassert(fmod(out, 1) == 0);
			return out;
		};



	}
	BPMComponent::~BPMComponent()
	{
		linkedFollower->removeComponent();

		hostBPM.setLookAndFeel(nullptr);
		userBPM.setLookAndFeel(nullptr);

		hostBPM.numbers.setLookAndFeel(nullptr);
		userBPM.numbers.setLookAndFeel(nullptr);

		button.setLookAndFeel(nullptr);
	}

	void BPMComponent::setColors(const NamedColorsIdentifier& slider, const NamedColorsIdentifier& buttonDown, const  NamedColorsIdentifier& buttonUp)
	{

		sliderColor = slider;
		buttonDownColor = buttonDown;
		buttonUpColor = buttonUp;
		GUI_UniversalsChanged();



	}

	void BPMComponent::resized()
	{
		auto b = 0.1f;
		//hostBPM.setBoundsRelative(0.25, 0, 0.5, 0.5 - b / 2);

		//userBPM.setBounds(hostBPM.getBounds());
		//button.setBoundsRelative(0, 0, 0.25 - b, 1);

		//inc.setBoundsRelative(0.75 + b, 0, 0.25 - b, 0.5);
		//dec.setBoundsRelative(0.75 + b, 0.5, 0.25 - b, 0.5);

		hostBPM.setBoundsRelative(0, 0, 1, 0.66f - b / 2);

		userBPM.setBounds(hostBPM.getBounds());
		button.setBoundsRelative(0.4f, 0.66f + b / 2, 0.2, 0.33f - b / 2);

		dec.setBoundsRelative(0, 0.66f + b / 2, 0.2f, 0.33f - b / 2);
		fineDec.setBoundsRelative(0.2f, 0.66f + b / 2, 0.2f, 0.33f - b / 2);
		inc.setBoundsRelative(0.8f, 0.66f + b / 2, 0.2f, 0.33f - b / 2);
		fineInc.setBoundsRelative(0.6f, 0.66f + b / 2, 0.2f, 0.33f - b / 2);

	}



	void BPMComponent::setNoHost()
	{
		button.setToggleState(false, juce::sendNotification);
		button.setEnabled(false);

		alert->addMessageToQueue("Ok", "BPM Not Found", BDSP_APP_NAME + juce::String(" was unable to find a host tempo to sync to. Tempo will have to be set manually within ") + BDSP_APP_NAME);
	}



	void BPMComponent::attach(juce::AudioProcessorValueTreeState& stateToUse)
	{
		userBPM.attach(stateToUse, linkedFollower->getUserParameter()->paramID);

		hostBPM.valueFromTextFunction = userBPM.valueFromTextFunction;
		hostBPM.textFromValueFunction = userBPM.textFromValueFunction;
		hostBPM.setRange(userBPM.getRange(), userBPM.getInterval());

		button.attach(stateToUse, linkedFollower->getLinkParameter()->paramID);
	}

	double BPMComponent::getValue()
	{
		return isFollowingHost() ? hostBPM.getValue() : userBPM.getValue();
	}

	void BPMComponent::setHostBPM(double BPM)
	{
		hostBPM.setValue(BPM);
	}


	bool BPMComponent::isFollowingHost()
	{
		return hostBPM.isVisible();
	}



	NumberSlider* BPMComponent::getHostSlider()
	{
		return &hostBPM;
	}

	NumberSlider* BPMComponent::getUserSlider()
	{
		return &userBPM;
	}

	void BPMComponent::addListener(Listener* listenerToAdd)
	{
		userBPM.addListener(listenerToAdd);
		hostBPM.addListener(listenerToAdd);

		listeners.addIfNotAlreadyThere(listenerToAdd);
	}

	void BPMComponent::removeListener(Listener* listenerToRemove)
	{
		userBPM.removeListener(listenerToRemove);
		hostBPM.removeListener(listenerToRemove);

		listeners.removeAllInstancesOf(listenerToRemove);
	}
	void BPMComponent::updateListeners()
	{
		for (auto l : listeners)
		{
			l->valueChanged();
		}
	}

	void BPMComponent::modifierKeysChanged(const juce::ModifierKeys& modifiers)
	{
		currentMods = modifiers;
	}

	void BPMComponent::GUI_UniversalsChanged()
	{
		userBPM.numbers.setColour(juce::Label::textColourId, getColor(sliderColor));
		userBPM.numbers.setColour(juce::Label::textWhenEditingColourId, getColor(sliderColor));
		userBPM.numbers.setColour(juce::CaretComponent::caretColourId, getColor(sliderColor));
		userBPM.numbers.setColour(juce::TextEditor::textColourId, getColor(sliderColor));
		userBPM.numbers.setColour(juce::TextEditor::highlightedTextColourId, getColor(sliderColor));
		userBPM.numbers.setColour(juce::TextEditor::highlightColourId, getColor(sliderColor).withAlpha(0.25f));

		hostBPM.numbers.setColour(juce::Label::textColourId, getColor(sliderColor));
		hostBPM.numbers.setColour(juce::Label::textWhenEditingColourId, getColor(sliderColor));
		hostBPM.numbers.setColour(juce::CaretComponent::caretColourId, getColor(sliderColor));
		hostBPM.numbers.setColour(juce::TextEditor::textColourId, getColor(sliderColor));
		hostBPM.numbers.setColour(juce::TextEditor::highlightedTextColourId, getColor(sliderColor));
		hostBPM.numbers.setColour(juce::TextEditor::highlightColourId, getColor(sliderColor).withAlpha(0.25f));



		//================================================================================================================================================================================================




		Shape s(universals->commonPaths.linkSymbol, buttonDownColor, juce::Rectangle<float>(1, 1));
		s.fillOff = buttonUpColor;

		button.clearShapes();
		button.addShapes(s);

		//================================================================================================================================================================================================
		auto rect = juce::Rectangle<float>(0.2f, 0.2f, 0.6f, 0.6f);
		juce::GlyphArrangement ga;
		ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), "+1", 0, 0);
		juce::Path text;
		ga.createPath(text);


		Shape incS(text, buttonDownColor, rect);

		inc.clearShapes();
		inc.addShapes(incS);


		//================================================================================================================================================================================================


		ga.clear();
		text.clear();

		ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), "-1", 0, 0);
		ga.createPath(text);


		Shape decS(text, buttonDownColor, rect);
		dec.clearShapes();
		dec.addShapes(decS);

		//================================================================================================================================================================================================


		ga.clear();
		text.clear();

		ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), "+0.1", 0, 0);
		ga.createPath(text);


		Shape fineIncS(text, buttonDownColor, rect);
		fineInc.clearShapes();
		fineInc.addShapes(fineIncS);

		//================================================================================================================================================================================================


		ga.clear();
		text.clear();

		ga.addLineOfText(universals->Fonts[getFontIndex()].getFont(), "-0.1", 0, 0);
		ga.createPath(text);


		Shape fineDecS(text, buttonDownColor, rect);
		fineDec.clearShapes();
		fineDec.addShapes(fineDecS);

		repaint();
	}

	void BPMComponent::Listener::sliderValueChanged(juce::Slider* slider)
	{
		valueChanged();
	}
	//=====================================================================================================================

	BPMFollower::BPMFollower(juce::RangedAudioParameter* userParameter, juce::RangedAudioParameter* linkParameter)
	{
		userParam = userParameter;
		userParam->addListener(this);

		linkParam = linkParameter;

	}

	void BPMFollower::parameterValueChanged(int parameterIndex, float newValue)
	{
		userValue = userParam->getNormalisableRange().convertFrom0to1(newValue);
	}

	void BPMFollower::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
	{

	}

	void BPMFollower::updateHostValue(juce::AudioPlayHead::PositionInfo& info)
	{
		hostValue = info.getBpm().hasValue() ? *info.getBpm() : 120.0;

		triggerAsyncUpdate();

	}

	void BPMFollower::updateUserValue(double newValue)
	{
		userValue = newValue;
	}

	void BPMFollower::setNoHost()
	{
		toggle(false);
		validHost = false;
		triggerAsyncUpdate();
	}

	double BPMFollower::getValue()
	{
		//jassert(isfinite(userValue) && userValue != 0);
		return followsHost ? hostValue : userValue;
	}

	double BPMFollower::getHostValue()
	{
		return hostValue;
	}

	double BPMFollower::getUserValue()
	{
		return userValue;
	}

	void BPMFollower::toggle(bool shouldFollowHost)
	{
		followsHost = shouldFollowHost;
	}

	bool BPMFollower::isFollowingHost()
	{
		return followsHost;
	}

	bool BPMFollower::hasValidHost()
	{
		return validHost;
	}

	void BPMFollower::handleAsyncUpdate()
	{
		if (linkedComponent != nullptr)
		{
			linkedComponent->setHostBPM(hostValue);
			if (!validHost)
			{
				linkedComponent->setNoHost();
			}
		}
	}

	void BPMFollower::setComponent(BPMComponent* componentToAdd)
	{
		linkedComponent = componentToAdd;
		linkedComponent->setHostBPM(hostValue);
		if (!validHost)
		{
			linkedComponent->setNoHost();
		}
	}

	void BPMFollower::removeComponent()
	{
		linkedComponent = nullptr;
	}

	juce::RangedAudioParameter* BPMFollower::getUserParameter()
	{
		return userParam;
	}

	juce::RangedAudioParameter* BPMFollower::getLinkParameter()
	{
		return linkParam;
	}




}// namnepace bdsp
