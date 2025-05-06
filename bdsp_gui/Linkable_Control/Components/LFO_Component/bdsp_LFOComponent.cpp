#include "bdsp_LFOComponent.h"

namespace bdsp
{

	//================================================================================================================================================================================================



	LFOParameterListener::LFOParameterListener(dsp::DSP_Universals<float>* lookupsToUse)
	{
		lookups = lookupsToUse;
		lookups->waveLookups.operator->();
	}

	LFOParameterListener::~LFOParameterListener()
	{
	}

	void LFOParameterListener::setParameters(ControlParameter* rateTimeParam, ControlParameter* rateFracParam, juce::AudioParameterChoice* rateDivParam, ControlParameter* shape, ControlParameter* skew, ControlParameter* amplitude, juce::AudioParameterBool* polarity, BPMFollower* bpmFollow)
	{
		BPM = bpmFollow;
		rateDivisionParameter = rateDivParam;
		rateMSParameter = rateTimeParam;
		rateFracParameter = rateFracParam;

		shapeParameter = shape;
		skewParameter = skew;
		amplitudeParameter = amplitude;
		polarityParameter = polarity;
		parametersSet = true;
	}



	void LFOParameterListener::updateVariables(float progressDelta)
	{
		if (parametersSet)
		{

			periodMs = calculateTimeInMs(BPM->getValue(), rateMSParameter->getActualValue(), rateFracParameter->getActualValue(), rateDivisionParameter->getIndex(), true);
			//auto period = periodMs / 1000.0 * BDSP_CONTROL_SAMPLE_RATE;// period of LFO in samples @ control sample rate
			//progressIncrement = 1.0 / period;
			progressIncrement = 1000.0 / (periodMs * BDSP_CONTROL_SAMPLE_RATE);
			shape = shapeParameter->getActualValue();
			skew = skewParameter->getActualValueNormalized();
			amplitude = amplitudeParameter->getActualValue();

			isBipolar = polarityParameter->get();

			float tmp;
			progress = modf(progress + progressDelta, &tmp);
			v = lookups->waveLookups->getLFOValue(shape, skew, progress, isBipolar, amplitude);
		}
	}


	//================================================================================================================================================================================================


	LFOControllerObject::LFOControllerObject(dsp::DSP_Universals<float>* lookupsToUse)
		:LFOParameterListener(lookupsToUse)
	{
	}

	LFOControllerObject::~LFOControllerObject()
	{
	}



	float LFOControllerObject::update()
	{
		updateVariables(progressIncrement);
		value = v;
		polarity = isBipolar;
		pushToListeners();

		return value;
	}



	//================================================================================================================================================================================================


	LFOComponent::LFOComponent(GUI_Universals* universalsToUse, BPMComponent* BPMComp, int lfoIDX, juce::ValueTree* nameValueLocation)
		:GeneralControlComponent(universalsToUse, BDSP_NUMBER_OF_MACROS + lfoIDX, nameValueLocation, juce::Identifier("LFO" + juce::String(lfoIDX + 1) + "NameID"))
	{
		setFontIndex(2);
		displayName = "LFO " + juce::String(lfoIDX + 1);



		ID = "LFO" + juce::String(lfoIDX + 1) + "ID";

		auto baseID = displayName.removeCharacters(" ");



		//================================================================================================================================================================================================



		setInterceptsMouseClicks(true, true);


		rate = std::make_unique<RangedContainerSyncFraction>(dynamic_cast<juce::AudioParameterChoice*>(universalsToUse->APVTS->getParameter(baseID + "RateDivisionID")), universalsToUse, BPMComp, displayName, true);

		rate->getMsSlider()->setHintBarText(defaultName + "'s frequency in Hertz");
		rate->getFraction()->setHintBarText(defaultName + "'s period");
		rate->getDivision()->setHintBarText(defaultName + "'s rate division");


		rate->setCorners(CornerCurves(CornerCurves::topLeft | CornerCurves::bottomLeft));
		addAndMakeVisible(rate.get());

		rateLabel = std::make_unique<Label>(universals);
		addAndMakeVisible(rateLabel.get());
		rateLabel->addHoverPartner(rate.get());
		rateLabel->setText("Rate", juce::sendNotification);
		rateLabel->setFontIndex(0);

		//================================================================================================================================================================================================



		shape = std::make_unique<RangedContainerSlider>(universalsToUse, baseID + " Shape");


		shape->slider.addCatchValues({ 0.5 });
		//shape->slider.label.setTitle("Shape");
		shape->slider.setHintBarText("The shape of " + defaultName + "'s function");


		shape->slider.setCornerCurves(CornerCurves(0));
		addAndMakeVisible(shape.get());


		shapeLabel = std::make_unique<Label>(universals);
		addAndMakeVisible(shapeLabel.get());
		shapeLabel->addHoverPartner(shape.get());

		shapeLabel->setText("Shape", juce::sendNotification);
		shapeLabel->setFontIndex(0);
		//================================================================================================================================================================================================


		skew = std::make_unique<RangedContainerSlider>(universalsToUse, baseID + " Skew");

		skew->slider.setType(BaseSlider::Type::CenterZero);
		skew->slider.addCatchValues({ 0.5 });
		//	skew->slider.label.setTitle("Skew");
		skew->slider.setHintBarText("Stretches " + defaultName + "'s function");
		skew->slider.setCornerCurves(CornerCurves(0));

		addAndMakeVisible(skew.get());


		skewLabel = std::make_unique<Label>(universals);
		addAndMakeVisible(skewLabel.get());
		skewLabel->addHoverPartner(skew.get());
		skewLabel->setText("Skew", juce::sendNotification);
		skewLabel->setFontIndex(0);
		//================================================================================================================================================================================================



		amplitude = std::make_unique<RangedContainerSlider>(universalsToUse, baseID + " Amplitude");


		//amplitude->slider.label.setTitle("Amp");
		amplitude->slider.setHintBarText("Gain applied to " + defaultName + "'s output");


		amplitude->slider.setCornerCurves(CornerCurves(0));

		addAndMakeVisible(amplitude.get());


		ampLabel = std::make_unique<Label>(universals);
		addAndMakeVisible(ampLabel.get());
		ampLabel->addHoverPartner(amplitude.get());

		ampLabel->setText("Amp", juce::sendNotification);
		ampLabel->setFontIndex(0);
		//================================================================================================================================================================================================


		LFOIndex = lfoIDX;

		polarity = std::make_unique<PathButton>(universals, true, true);


		juce::Path uni, bi;

		uni = createCross(juce::Point<float>(), 0.25);


		bi.addPath(uni);
		bi.addRectangle(-1, 1.5, 2, 0.25);

		polarity->setPath(bi, uni);

		polarity->setClickingTogglesState(true);


		addAndMakeVisible(polarity.get());

		polarity->onStateChange = [=]
		{
			polarity->setHintBarText("Sets " + defaultName + " to output " + juce::String(polarity->getToggleState() ? "only positive values" : "positive and negative values"));
		};



		//================================================================================================================================================================================================

		//modSnap = std::make_unique<PathButton>(universals, true, true);


		//modSnap->setPath(universals->commonPaths.locked, universals->commonPaths.unlocked);

		//modSnap->setClickingTogglesState(true);


		//addAndMakeVisible(modSnap.get());

		//modSnap->onStateChange = [=]
		//{
		//	modSnap->setHintBarText((modSnap->getToggleState() ? "Un-Locks" : "Locks") + juce::String(" modulated tempo-sync rates of " + defaultName + " to standard rates"));
		//};



		//================================================================================================================================================================================================





		//================================================================================================================================================================================================


		BPM = BPMComp;
		addAndMakeVisible(dragComp);
		addAndMakeVisible(transferComp);



	}



	LFOComponent::~LFOComponent()
	{
	}


	void LFOComponent::colorSet()
	{

		//rate->setFracColors(c, BDSP_COLOR_BLACK);
		rate->setFracColors(BDSP_COLOR_PURE_BLACK, c, BDSP_COLOR_KNOB, BDSP_COLOR_PURE_BLACK, BDSP_COLOR_MID);
		if (c.ID.toString() == BDSP_COLOR_WHITE)
		{
			rate->getDivision()->setColorSchemeClassic(BDSP_COLOR_KNOB, NamedColorsIdentifier(), NamedColorsIdentifier(BDSP_COLOR_BLACK), c.withMultipliedAlpha(universals->lowOpacity));
		}




		shape->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());
		skew->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());
		amplitude->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, NamedColorsIdentifier(), NamedColorsIdentifier());

		//skew->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, BDSP_COLOR_MID, BDSP_COLOR_LIGHT);// , c, c, c, caret, c.withMultipliedAlpha(universals->lowOpacity));
		//amplitude->slider.setSliderColors(BDSP_COLOR_PURE_BLACK, c, BDSP_COLOR_MID, BDSP_COLOR_LIGHT);// , c, c, c, caret, c.withMultipliedAlpha(universals->lowOpacity));

		polarity->setColor(BDSP_COLOR_PURE_BLACK, BDSP_COLOR_PURE_BLACK);
		//modSnap->setColor(BDSP_COLOR_PURE_BLACK, BDSP_COLOR_PURE_BLACK);
		/*polarity->setTwoColorScheme(BDSP_COLOR_KNOB, BDSP_COLOR_PURE_BLACK);
		hostLinked->setTwoColorScheme(BDSP_COLOR_KNOB, BDSP_COLOR_PURE_BLACK);
		modSnap->setTwoColorScheme(BDSP_COLOR_KNOB, BDSP_COLOR_PURE_BLACK);*/

		if (visualizer != nullptr)
		{
			visualizer->setColor(c, c);
		}


		rateLabel->setColour(juce::Label::textColourId, getColor(c));
		shapeLabel->setColour(juce::Label::textColourId, getColor(c));
		skewLabel->setColour(juce::Label::textColourId, getColor(c));
		ampLabel->setColour(juce::Label::textColourId, getColor(c));

	}


	void LFOComponent::setControlObject(LinkableControl* newControlObject)
	{
		LinkableControlComponent::setControlObject(newControlObject);
		attach();
		auto LFOObject = dynamic_cast<LFOControllerObject*>(newControlObject);
		auto polarityParam = dynamic_cast<juce::AudioParameterBool*>(polarity->param);




		LFOObject->setParameters(rate->getMsSlider()->controlParam, rate->getFraction()->controlParam, rate->getDivision()->getParameter(), shape->getControlParamter(), skew->getControlParamter(), amplitude->getControlParamter(), polarityParam, BPM->linkedFollower);
		visualizer = std::make_unique<LFOVisualizer>(universals, this, LFOObject->lookups);
		//newControlObject->addListener(visualizer.get());
		visualizer->setColor(c, c);

		polarityParam->sendValueChangedMessageToListeners(polarityParam->get());
		addAndMakeVisible(visualizer.get());



	}

	void LFOComponent::attach()
	{
		auto baseID = displayName.removeCharacters(" ");
		rate->attach(*universals->APVTS, baseID + "Rate");
		shape->slider.attach(*universals->APVTS, baseID + "ShapeID");
		skew->slider.attach(*universals->APVTS, baseID + "SkewID");
		amplitude->slider.attach(*universals->APVTS, baseID + "AmplitudeID");
		polarity->attach(*universals->APVTS, baseID + "PolarityID");


		// needs to be attached first
		rate->getLockButton()->onStateChange = [=]
		{
			rate->getLockButton()->setHintBarText(juce::String(rate->getLockButton()->getToggleState() ? "Un-Restricts" : "Restricts") + " modulated values of" + defaultName + "'s rate to valid values" + juce::String(rate->getLockButton()->isEnabled() ? "" : "(When " + defaultName + " is tempo-synced)"));
		};

	}

	void LFOComponent::resized()
	{
		GeneralControlComponent::resized();




		auto gap = titleRect.getBottom() - title->getBottom();

		auto reduced = getLocalBounds().toFloat().withTop(titleRect.getBottom());


		visualizer->setBounds(shrinkRectangleToInt(reduced.getProportion(juce::Rectangle<float>(0, 0, 1, universals->controlUniversals.visualizerHieghtRatio)).withTrimmedBottom(gap / 2)));

		//reduced = reduced.getProportion(juce::Rectangle<float>(0, 0, 0.5, 1));


		auto y = visualizer->getBottom() + gap;
		auto h = (getHeight() - y) * universals->controlUniversals.containerSliderHeightRatio;

		//polarity->setBounds(shrinkRectangleToInt(buttonRect.getProportion(juce::Rectangle<float>(0, 0, 1, 0.5))));


		auto x = visualizer->getX();
		auto b = 0;

		auto rateW = getWidth() * 0.4;
		auto othersW = (getWidth() - x - rateW - 4 * b) / 3.5;

		rate->setBounds(x, y, rateW, h);
		shape->setBounds(rate->getRight() + b, y, othersW, h);
		skew->setBounds(shape->getRight() + b, y, othersW, h);
		amplitude->setBounds(skew->getRight() + b, y, othersW, h);

		polarity->setPathBorder(universals->rectThicc);
		polarity->setBounds(juce::Rectangle<int>(amplitude->getRight() + b, y, othersW / 2, h).reduced(universals->rectThicc));

		rateLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(rate->getX(), rate->getBottom(), rate->getRight(), getHeight()).reduced(0, universals->rectThicc));
		shapeLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(shape->getX(), shape->getBottom(), shape->getRight(), getHeight()).reduced(0, universals->rectThicc));
		skewLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(skew->getX(), skew->getBottom(), skew->getRight(), getHeight()).reduced(0, universals->rectThicc));
		ampLabel->setBounds(juce::Rectangle<int>().leftTopRightBottom(amplitude->getX(), amplitude->getBottom(), amplitude->getRight(), getHeight()).reduced(0, universals->rectThicc));


	}

	void LFOComponent::paint(juce::Graphics& g)
	{
		//g.setColour(getColor(BDSP_COLOR_MID));
		//g.fillRoundedRectangle(buttonRect, universals->roundedRectangleCurve);

		//auto w = juce::jmax(getHeight() * 0.005, 1.0);
		//g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		//g.drawRoundedRectangle(buttonRect, universals->roundedRectangleCurve, w);

		juce::Path polarityBKGD;
		polarityBKGD.addRoundedRectangle(amplitude->getRight(), amplitude->getY(), getWidth() - amplitude->getRight(), amplitude->getHeight(), universals->roundedRectangleCurve, universals->roundedRectangleCurve, false, true, false, true);
		g.setColour(getColor(BDSP_COLOR_KNOB));
		g.fillPath(polarityBKGD);

	}

	void LFOComponent::paintOverChildren(juce::Graphics& g)
	{



		auto w = juce::jmax(universals->dividerSize, 1.0f);



		g.setColour(getColor(BDSP_COLOR_DARK));
		g.drawRoundedRectangle(visualizer->getBounds().toFloat(), universals->roundedRectangleCurve, w);
		g.drawRoundedRectangle(juce::Rectangle<float>().leftTopRightBottom(rate->getX(), rate->getY(), visualizer->getRight(), rate->getBottom()), universals->roundedRectangleCurve, w);

		if (w <= 1)
		{
			g.drawVerticalLine(visualizer->getBounds().getCentreX(), visualizer->getY(), visualizer->getBottom());
			g.drawVerticalLine((shape->getRight() + skew->getX()) / 2, shape->getY(), shape->getBottom());
			g.drawVerticalLine((skew->getRight() + amplitude->getX()) / 2, skew->getY(), skew->getBottom());

		}
		else
		{
			g.fillRect(juce::Rectangle<float>(visualizer->getBounds().getCentreX() - w / 2, visualizer->getY(), w, visualizer->getHeight()));
			g.fillRect(juce::Rectangle<float>((shape->getRight() + skew->getX()) / 2 - w / 2, shape->getY(), w, shape->getHeight()));
			g.fillRect(juce::Rectangle<float>((skew->getRight() + amplitude->getX()) / 2 - w / 2, skew->getY(), w, skew->getHeight()));
		}

	}

	Component* LFOComponent::getVisualizer()
	{
		return visualizer.get();
	}

	//void LFOComponent::paint(juce::Graphics& g)
	//{

	//	g.setColour(universals->colors.getColor(c));
	//	drawText(g, universals->Fonts[getFontIndex()].getFont().withHeight(visualizer->getY()), displayName, getLocalBounds().withBottom(visualizer->getY()));
	//}















	juce::Array<juce::WeakReference<HoverableComponent>> LFOComponent::getComponentsThatShareHover()
	{
		juce::Array<juce::WeakReference<HoverableComponent>> out({ this, rate.get(), shape.get(),skew.get(), polarity.get(), visualizer.get(), &dragComp });

		return out;
	}










	//================================================================================================================================================================================================



	LFOSection::LFOSection(GUI_Universals* universalsToUse, BPMComponent* BPMComp, const  NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation)
		:TexturedContainerComponent(universalsToUse, backgroundColor)
	{

		;
		for (int i = 0; i < BDSP_NUMBER_OF_LFOS; ++i)
		{
			LFOHolders.add(std::make_unique<Component>(universals));
			addAndMakeVisible(LFOHolders[i]);
			LFOs.add(std::make_unique<LFOComponent>(universals, BPMComp, i, nameValueLocation));
			LFOHolders[i]->addAndMakeVisible(LFOs[i]);
			LFOs.getLast()->setColor(BDSP_COLOR_COLOR, BDSP_COLOR_COLOR);
		}


	}



	LFOComponent* LFOSection::getLFO(int idx)
	{
		return LFOs[idx];
	}

	void LFOSection::setVertical(bool shouldBeVertical)
	{
		isVertical = shouldBeVertical;
		resized();
	}




	void LFOSection::resized()
	{

		auto w = 1.0 / LFOs.size();
		for (int i = 0; i < LFOs.size(); ++i)
		{
			if (isVertical)
			{
				LFOHolders[i]->setBounds(shrinkRectangleToInt(getLocalBounds().getProportion(juce::Rectangle<float>(0, i * w, 1, w))));
			}
			else
			{
				LFOHolders[i]->setBounds(shrinkRectangleToInt(getLocalBounds().getProportion(juce::Rectangle<float>(i * w, 0, w, 1))));
			}
			LFOs[i]->setBoundsRelative(0.05, 0.05, 0.9, 0.9);
		}
	}

	void LFOSection::paint(juce::Graphics& g)
	{
		TexturedContainerComponent::paint(g);

		//for (int i = 0; i < LFOs.size() - 1; ++i)
		//{
		//	auto x = (LFOHolders[i]->getRight() + LFOHolders[i + 1]->getX()) / 2.0;
		//	drawDivider(g, juce::Line<float>(x, universals->dividerBorder, x, getHeight() - universals->dividerBorder), universals->colors.getColor(BDSP_COLOR_LIGHT), universals->dividerSize);
		//}
	}

	void LFOSection::setTitleFontIndex(int index)
	{
		for (auto l : LFOs)
		{
			l->getTitleComponent()->setFontIndex(index);
		}
	}

	void LFOSection::visibilityChanged()
	{
		for (auto l : LFOs)
		{
			l->setVisible(isVisible());
		}
	}






}// namnepace bdsp
