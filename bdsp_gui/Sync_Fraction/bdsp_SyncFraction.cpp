#include "bdsp_SyncFraction.h"

namespace bdsp
{



	SyncFractionBase::SyncFractionBase(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRateFrac)
		:Component(universalsToUse)
	{
		divisionParam = divParam;
		bpmComp = BPMComp;
		isRate = isRateFrac;
	}

	SyncFractionBase::~SyncFractionBase() noexcept
	{
		for (auto l : listeners)
		{
			if (l.get() != nullptr)
			{
				removeListener(l);
			}
		}
	}

	void SyncFractionBase::init()
	{
		fractionSlider->addListener(this); // BPMComp Listener which inherets from slider listener

		msTimeSlider->addListener(this); // BPMComp Listener which inherets from slider listener

		auto altDivisionNames = juce::StringArray(divAlt);
		if (isRate)
		{
			altDivisionNames.set(0, "Hz");
		}

		auto divisionParameterChanged = [=](int i)
		{
			msTime->setVisible(i == 0);
			fraction->setVisible(i != 0);

			division->setIndex(i);

			sliderValueChanged(nullptr);
		};

		division->onParameterChanged = divisionParameterChanged;



		fractionSlider->setPopupDisplayEnabled(false, false, nullptr);
		fractionSlider->setChangeNotificationOnlyOnRelease(false);

		msTimeSlider->setPopupDisplayEnabled(false, false, nullptr);
		msTimeSlider->setChangeNotificationOnlyOnRelease(false);









		setInterceptsMouseClicks(false, true);






		bpmComp->addListener(this);

		division->parameterChanged(division->getIndex());
		division->onParameterChanged(division->getIndex());


	}

	void SyncFractionBase::attach(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterIDBase)
	{

		fractionSlider->attach(stateToUse, parameterIDBase + "FractionID");


		if (isRate)
		{
			msTimeSlider->attach(stateToUse, parameterIDBase + "HzRateID");
		}
		else
		{
			msTimeSlider->attach(stateToUse, parameterIDBase + "MsTimeID");
		}


		attached = true;
	}


	juce::String SyncFractionBase::getBaseID()
	{
		return fractionSlider->getComponentID().upToLastOccurrenceOf("Fraction", false, false) + "ID";
	}
	float SyncFractionBase::getTimeInMs(double bpm, bool isRate)
	{
		return calculateTimeInMs(bpm, msTimeSlider->getParameter()->getNormalisableRange().convertFrom0to1(msTimeSlider->getParameter()->getValue()), fractionSlider->getParameter()->getNormalisableRange().convertFrom0to1(fractionSlider->getParameter()->getValue()), division->getIndex(), isRate);
	}
	void SyncFractionBase::addListener(Listener* listenerToAdd)
	{
		listeners.addIfNotAlreadyThere(listenerToAdd);
		listenerToAdd->fracsListeningTo.addIfNotAlreadyThere(this);
	}
	void SyncFractionBase::removeListener(Listener* listenerToRemove)
	{
		listeners.removeAllInstancesOf(listenerToRemove);
		listenerToRemove->fracsListeningTo.removeAllInstancesOf(this);
	}
	void SyncFractionBase::valueChanged()
	{
		for (auto l : listeners)
		{
			if (l.get() != nullptr)
			{
				l->valueChanged(this);
			}
		}
	}

	//================================================================================================================================================================================================


	template <class fracType>
	SyncFraction<fracType>::SyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate)
		:SyncFractionBase(divParam, universalsToUse, BPMComp, baseName, isRate)
	{
		circleFrac = std::make_unique<fracType>(universalsToUse, baseName + "Fraction");
		CircleSlider& frac = *circleFrac.get();
		frac.addListener(this); // BPMComp Listener which inherets from slider listener
		frac.label.setTitle("Beats");
		fractionSlider = &frac;
		fraction = circleFrac.get();

		circleTime = std::make_unique<fracType>(universalsToUse, baseName + "MsTime");
		CircleSlider& ms = *circleTime.get();
		ms.addListener(this); // BPMComp Listener which inherets from slider listener
		ms.label.setTitle(isRate ? "Hz" : "ms");
		msTimeSlider = &ms;
		msTime = circleTime.get();

		divisionCombo = std::make_unique<ComboBox>(divParam, universalsToUse);
		division = divisionCombo.get();

		divisionLabel = std::make_unique<DivLabel>(divisionCombo.get(), getMsSlider());
		divisionLabel->linkToComponent(divisionCombo.get());

		divisionLabel->setFontIndex(ms.label.getFontIndex());
		divisionLabel->setJustificationType(juce::Justification::centred);
		ms.label.copyAllExplicitColoursTo(*divisionLabel.get());

		divisionLabel->setHeightRatio(ms.label.getHeightRatio());

		divisionLabel->setText("Division", juce::dontSendNotification);






		//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================//=====================================================================================================================

		addAndMakeVisible(divisionCombo.get());
		addAndMakeVisible(divisionLabel.get());




		addChildComponent(circleFrac.get());

		addAndMakeVisible(circleTime.get());










		divisionCombo->setHeightRatio(0.25);

		init();

	}


	template <class fracType>
	SyncFraction<fracType>::~SyncFraction()
	{
		setLookAndFeel(nullptr);

	}









	template <class fracType>
	void SyncFraction<fracType>::resized()
	{
		CircleSlider& frac = *circleFrac.get();
		CircleSlider& ms = *circleTime.get();







		frac.setBoundsToIncludeLabel(getLocalBounds().getProportion(juce::Rectangle<float>(timeX, 0, timeW, 1)), timeRectanglePlacement);
		ms.setBoundsToIncludeLabel(fractionSlider->getBounds(), timeRectanglePlacement);



		divisionCombo->setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(divX, 0, divW, 1)).withBottom(ms.getKnobBounds(true).getBottom()).withTop(ms.getKnobBounds(true).getY()));



	}

	template <class fracType>
	void SyncFraction<fracType>::setFracColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text, const NamedColorsIdentifier& textHighlight, const NamedColorsIdentifier& textEdit, const NamedColorsIdentifier& caret, const NamedColorsIdentifier& highlight)
	{

		CircleSlider& frac = *circleFrac.get();
		CircleSlider& ms = *circleTime.get();


		frac.setSliderColors(knob, value, valueTrack, valueTrackInside, text, textHighlight, textEdit, caret, highlight);
		ms.setSliderColors(knob, value, valueTrack, valueTrackInside, text, textHighlight, textEdit, caret, highlight);
		
		divisionCombo->setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), caret, highlight);


		divisionLabel->setColour(juce::Label::textColourId, getColor(text));
	}

	template <class fracType>
	void SyncFraction<fracType>::setFracColors(const NamedColorsIdentifier& value, const NamedColorsIdentifier& accent, float opacity)
	{

		CircleSlider& frac = *circleFrac.get();
		CircleSlider& ms = *circleTime.get();


		frac.setSliderColor(value, accent, opacity);
		ms.setSliderColor(value, accent, opacity);

		divisionCombo->setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), accent, value.withMultipliedAlpha(universals->lowOpacity));

		divisionLabel->setColour(juce::Label::textColourId, getColor(accent));

	}





	template <class fracType>
	void SyncFraction<fracType>::paint(juce::Graphics& g)
	{

	}

	template <class fracType>
	void SyncFraction<fracType>::lookAndFeelChanged()
	{
		circleFrac->setLookAndFeel(&getLookAndFeel());
		circleTime->setLookAndFeel(&getLookAndFeel());
		divisionLabel->setLookAndFeel(&getLookAndFeel());
	}






	template <class fracType>
	fracType* SyncFraction<fracType>::getFraction()
	{
		return circleFrac.get();
	}

	template <class fracType>
	fracType* SyncFraction<fracType>::getMsSlider()
	{
		return circleTime.get();
	}


	template <class fracType>
	ComboBox* SyncFraction<fracType>::getDivision()
	{
		return divisionCombo.get();
	}




	template <class fracType>
	void SyncFraction<fracType>::setSpacing(float timeXPos, float timeWidth, float divXPos, float divWidth, juce::RectanglePlacement timePlacement)
	{
		timeX = timeXPos;
		timeW = timeWidth;
		divX = divXPos;
		divW = divWidth;

		timeRectanglePlacement = timePlacement;

		resized();
	}

	template <class fracType>
	void SyncFraction<fracType>::setSpacing(float border, float width, juce::RectanglePlacement placement, juce::RectanglePlacement timePlacement)
	{
		auto total = 3 * border + 2 * width;
		if (total == 1)
		{
			setSpacing(border, width, border + width + border / 2, width);
		}
		else
		{
			float buffer;
			if (placement.testFlags(juce::RectanglePlacement::xMid))
			{
				buffer = (1 - total) / 2;
			}
			else if (placement.testFlags(juce::RectanglePlacement::xRight))
			{
				buffer = 1 - total;
			}
			else
			{
				buffer = 0;
			}

			setSpacing(buffer + border, width, buffer + border + width + border / 2, width, timePlacement);
		}
	}











}// namnepace bdsp

