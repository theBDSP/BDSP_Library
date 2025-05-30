#include "bdsp_SequencerComponent.h"

namespace bdsp
{
	SequencerParameterListener::SequencerParameterListener(dsp::DSP_Universals<float>* lookupsToUse)
	{
		lookups = lookupsToUse;
		lookups->waveLookups.operator->();
	}

	SequencerParameterListener::~SequencerParameterListener()
	{
	}

	void SequencerParameterListener::setParameters(juce::Array<juce::AudioParameterChoice*> shapes, juce::Array<Parameter*> amts, ControlParameter* rateFracParam, juce::AudioParameterChoice* rateDivParam, juce::AudioParameterInt* numStepsParameter, BPMFollower* bpmFollow)
	{
		shapeParameters = shapes;
		amtParamters = amts;

		rateDivisionParameter = rateDivParam;
		rateFracParameter = rateFracParam;

		stepsParameter = numStepsParameter;

		BPM = bpmFollow;
		parametersSet = true;
	}

	float SequencerParameterListener::getValueForShape(SequencerShapes shape, float proportion)
	{
		switch (shape)
		{
		case Empty:
			return 0;
			break;
		case Triangle:
			return lookups->waveLookups->lookupTri(0.5, proportion, false);
			break;
		case SawDown:
			return lookups->waveLookups->lookupTri(0, proportion, false);
			break;
		case SawUp:
			return lookups->waveLookups->lookupTri(1, proportion, false);
			break;
		case SinDown:
			return lookups->waveLookups->lookupSin(0, proportion, false);
			break;
		case SinUp:
			return lookups->waveLookups->lookupSin(1, proportion, false);
			break;
		case SquareFull:
			return 1;
			break;
		case SquareHalf:
			return lookups->waveLookups->lookupSqr(0.5, 1.0 - proportion, false);
			break;

		}
	}



	void SequencerParameterListener::updateVariables(float progressDelta)
	{
		if (parametersSet)
		{

			periodMs = calculateTimeInMs(BPM->getValue(), 0, rateFracParameter->getActualValue(), rateDivisionParameter->getIndex() + 1, true);
			//auto period = periodMs / 1000.0 * BDSP_CONTROL_SAMPLE_RATE;// period of Sequencer in samples @ control sample rate
			//progressIncrement = 1.0 / period;
			progressIncrement = 1000.0 / (periodMs * BDSP_CONTROL_SAMPLE_RATE);

			numSteps = stepsParameter->get();
			progress = fmodf(progress + progressDelta, numSteps);
			int step = static_cast<int>(progress);
			float proportion = progress - step;

			int shape = shapeParameters[step]->getIndex();

			v = amtParamters[step]->getActualValue() * getValueForShape(SequencerShapes(shape), proportion);
		}
	}


	//================================================================================================================================================================================================


	SequencerControllerObject::SequencerControllerObject(dsp::DSP_Universals<float>* lookupsToUse)
		:SequencerParameterListener(lookupsToUse)
	{
	}

	SequencerControllerObject::~SequencerControllerObject()
	{
	}



	float SequencerControllerObject::update()
	{
		updateVariables(progressIncrement);
		value = v;
		pushToListeners();

		return value;
	}



	//================================================================================================================================================================================================


	SequencerComponent::SequencerComponent(GUI_Universals* universalsToUse, BPMComponent* BPMComp, int SequencerIDX, juce::ValueTree* nameValueLocation)
		:GeneralControlComponent(universalsToUse, BDSP_NUMBER_OF_MACROS + BDSP_NUMBER_OF_LFOS + BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS + SequencerIDX, nameValueLocation, juce::Identifier("Sequencer" + juce::String(SequencerIDX + 1) + "NameID"))
	{
		setFontIndex(2);
		displayName = "Seq " + juce::String(SequencerIDX + 1);


		ID = "Sequencer" + juce::String(SequencerIDX + 1) + "ID";

		auto baseID = displayName.removeCharacters(" ");



		//================================================================================================================================================================================================



		setInterceptsMouseClicks(true, true);



		SequencerIndex = SequencerIDX;

		BPM = BPMComp;
		addAndMakeVisible(dragComp);
		addAndMakeVisible(transferComp);



	}



	SequencerComponent::~SequencerComponent()
	{
	}


	void SequencerComponent::colorSet()
	{
		if (visualizer != nullptr)
		{
			visualizer->getVis()->setColor(c);
		}


		if (rateFrac != nullptr)
		{
			rateFrac->setFracColors(BDSP_COLOR_PURE_BLACK, c, BDSP_COLOR_KNOB, BDSP_COLOR_BLACK, NamedColorsIdentifier());
		}
	}


	void SequencerComponent::setControlObject(LinkableControl* newControlObject)
	{
		LinkableControlComponent::setControlObject(newControlObject);
		auto SequencerObject = dynamic_cast<SequencerControllerObject*>(newControlObject);

		juce::Array<juce::AudioParameterChoice*>shapeParams;
		juce::Array<Parameter*>amtParams;

		for (int i = 0; i < BDSP_SEQUENCER_STEPS; ++i)
		{
			shapeParams.add(dynamic_cast<juce::AudioParameterChoice*>(universals->APVTS->getParameter(ID.toString().upToLastOccurrenceOf("ID", false, false) + "Shape" + juce::String(i + 1) + "ID")));
			amtParams.add(dynamic_cast<Parameter*>(universals->APVTS->getParameter(ID.toString().upToLastOccurrenceOf("ID", false, false) + "Amt" + juce::String(i + 1) + "ID")));
		}

		auto rateFracParam = dynamic_cast<ControlParameter*>(universals->APVTS->getParameter(ID.toString().upToLastOccurrenceOf("ID", false, false) + "RateFractionID"));
		auto rateDivisionParam = dynamic_cast<juce::AudioParameterChoice*>(universals->APVTS->getParameter(ID.toString().upToLastOccurrenceOf("ID", false, false) + "RateDivisionID"));

		auto numStepsParam = dynamic_cast<juce::AudioParameterInt*>(universals->APVTS->getParameter(ID.toString().upToLastOccurrenceOf("ID", false, false) + "NumStepsID"));

		SequencerObject->setParameters(shapeParams, amtParams, rateFracParam, rateDivisionParam, numStepsParam, BPM->linkedFollower);



		visualizer = std::make_unique<SequencerVisualizer>(universals, this);
		//newControlObject->addListener(visualizer.get());
		addAndMakeVisible(visualizer.get());


		visualizer->getVis()->cutoutColor = parentSection->background;

		rateFrac = std::make_unique<RangedContainerSyncFraction>(rateDivisionParam, universals, BPM, ID.toString().upToLastOccurrenceOf("ID", false, false) + "Rate", true);
		addAndMakeVisible(rateFrac.get());
		rateFrac->setCorners(CornerCurves::all);

		stepsSlider = std::make_unique<NumberSlider>(universals, numStepsParam->paramID.upToLastOccurrenceOf("ID", false, false));
		addAndMakeVisible(stepsSlider.get());
		stepsSlider->numbers.setJustificationType(juce::Justification::centred);


		attach();
		colorSet();
	}

	void SequencerComponent::attach()
	{
		rateFrac->attach(*universals->APVTS, ID.toString().upToLastOccurrenceOf("ID", false, false) + "Rate");
		stepsSlider->attach(*universals->APVTS, stepsSlider->getComponentID());
	}

	void SequencerComponent::resized()
	{
		GeneralControlComponent::resized();


		float visH = 0.7;
		auto visArea = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 1 - visH, 1, visH));


		visualizer->setBounds(shrinkRectangleToInt(visArea));

		titleRect = juce::Rectangle<float>(0, 0, getWidth(), visArea.getY() - universals->rectThicc);




		auto square = titleRect.withWidth(titleRect.getHeight());
		auto rateBounds = juce::Rectangle<float>(0, 0, 3 * square.getWidth(), square.getHeight());

		auto rightRect = titleRect.withTrimmedLeft(getWidth() - rateBounds.getRight());
		auto transferBounds = rightRect.getProportion(juce::Rectangle<float>(0, 0, 1.0 / 3.0, 1));
		auto dragBounds = rightRect.getProportion(juce::Rectangle<float>(1.0 / 3.0, 0, 1.0 / 3.0, 1));
		auto stepBounds = rightRect.getProportion(juce::Rectangle<float>(2.0 / 3.0, 0, 1.0 / 3.0, 1));

		rateFrac->setBounds(shrinkRectangleToInt(rateBounds.withTrimmedRight(universals->rectThicc)));

		transferComp.setBounds(shrinkRectangleToInt(transferBounds).reduced(universals->rectThicc));
		dragComp.setBounds(shrinkRectangleToInt(dragBounds).reduced(universals->rectThicc));
		stepsSlider->setBounds(shrinkRectangleToInt(stepBounds).reduced(universals->rectThicc, 0));

		auto inset = juce::jmax(rateBounds.getRight(), getWidth() - dragBounds.getX());
		titleRect = titleRect.withSizeKeepingCentre(getWidth() - 2 * inset, titleRect.getHeight());
		title->setBounds(shrinkRectangleToInt(titleRect));
	}

	void SequencerComponent::paint(juce::Graphics& g)
	{
		g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(titleRect, universals->roundedRectangleCurve);
		auto stepsRect = juce::Rectangle<float>(titleRect.getHeight(), titleRect.getHeight()).withCentre(stepsSlider->getBounds().toFloat().getCentre());
		g.fillRoundedRectangle(stepsRect, universals->roundedRectangleCurve);
	}



	Component* SequencerComponent::getVisualizer()
	{
		return visualizer.get();
	}

	void SequencerComponent::setParentSection(SequencerSection* p)
	{
		parentSection = p;
	}

	SequencerSection* SequencerComponent::getParentSection()
	{
		return parentSection;
	}








	juce::Array<juce::WeakReference<HoverableComponent>> SequencerComponent::getComponentsThatShareHover()
	{
		juce::Array<juce::WeakReference<HoverableComponent>> out({ this, visualizer.get(), &dragComp });

		return out;
	}










	//================================================================================================================================================================================================



	SequencerSection::SequencerSection(GUI_Universals* universalsToUse, BPMComponent* BPMComp, const  NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation)
		:TexturedContainerComponent(universalsToUse, backgroundColor)
	{

		for (int i = 0; i < BDSP_NUMBER_OF_SEQUENCERS; ++i)
		{
			SequencerHolders.add(std::make_unique<Component>(universals));
			addAndMakeVisible(SequencerHolders[i]);
			Sequencers.add(std::make_unique<SequencerComponent>(universals, BPMComp, i, nameValueLocation));
			SequencerHolders[i]->addAndMakeVisible(Sequencers[i]);
			Sequencers.getLast()->setColor(BDSP_COLOR_COLOR, BDSP_COLOR_COLOR);
			Sequencers.getLast()->setParentSection(this);

		}

		for (int i = 1; i < SequencerShapes::NUM; ++i)
		{
			shapeButtons.add(new PathButton(universals, true));
			addAndMakeVisible(shapeButtons.getLast());
			shapeButtons.getLast()->setRadioGroupId(1);

			juce::String hintText;

			juce::Path p;
			juce::Path sine;
			static const unsigned char sinePathData[] = { 110,109,167,229,78,69,75,28,139,68,98,240,227,78,69,75,28,139,68,57,226,78,69,200,27,139,68,131,224,78,69,85,27,139,68,98,204,222,78,69,210,26,139,68,215,220,78,69,220,25,139,68,32,219,78,69,230,24,139,68,98,105,217,78,69,240,23,139,68,179,215,78,69, 111,22,139,68,252,213,78,69,247,20,139,68,98,69,212,78,69,3,19,139,68,142,210,78,69,137,17,139,68,216,208,78,69,24,15,139,68,98,33,207,78,69,28,13,139,68,106,205,78,69,180,10,139,68,180,203,78,69,192,7,139,68,98,189,201,78,69,81,5,139,68,8,200,78,69, 92,2,139,68,81,198,78,69,108,255,138,68,98,154,196,78,69,254,251,138,68,228,194,78,69,15,249,138,68,45,193,78,69,36,245,138,68,98,118,191,78,69,182,241,138,68,192,189,78,69,204,237,138,68,9,188,78,69,225,233,138,68,98,82,186,78,69,242,229,138,68,156, 184,78,69,12,226,138,68,166,182,78,69,163,221,138,68,98,239,180,78,69,63,217,138,68,57,179,78,69,211,212,138,68,130,177,78,69,107,208,138,68,98,203,175,78,69,133,203,138,68,21,174,78,69,160,198,138,68,94,172,78,69,186,193,138,68,98,167,170,78,69,213, 188,138,68,241,168,78,69,239,183,138,68,58,167,78,69,11,179,138,68,98,131,165,78,69,167,173,138,68,205,163,78,69,193,168,138,68,216,161,78,69,95,163,138,68,98,33,160,78,69,251,157,138,68,106,158,78,69,154,152,138,68,180,156,78,69,54,147,138,68,98,253, 154,78,69,210,141,138,68,70,153,78,69,113,136,138,68,144,151,78,69,13,131,138,68,98,217,149,78,69,171,125,138,68,35,148,78,69,72,120,138,68,108,146,78,69,230,114,138,68,98,181,144,78,69,5,109,138,68,192,142,78,69,162,103,138,68,10,141,78,69,64,98,138, 68,98,83,139,78,69,221,92,138,68,156,137,78,69,123,87,138,68,230,135,78,69,149,82,138,68,98,47,134,78,69,49,77,138,68,121,132,78,69,208,71,138,68,194,130,78,69,233,66,138,68,98,11,129,78,69,134,61,138,68,85,127,78,69,161,56,138,68,158,125,78,69,188,51, 138,68,98,231,123,78,69,215,46,138,68,242,121,78,69,241,41,138,68,59,120,78,69,136,37,138,68,98,132,118,78,69,163,32,138,68,206,116,78,69,59,28,138,68,23,115,78,69,81,24,138,68,98,96,113,78,69,236,19,138,68,170,111,78,69,128,15,138,68,243,109,78,69,149, 11,138,68,98,60,108,78,69,166,7,138,68,134,106,78,69,192,3,138,68,207,104,78,69,82,0,138,68,98,216,102,78,69,228,252,137,68,35,101,78,69,119,249,137,68,108,99,78,69,135,246,137,68,98,181,97,78,69,25,243,137,68,255,95,78,69,167,240,137,68,72,94,78,69, 183,237,137,68,98,145,92,78,69,72,235,137,68,219,90,78,69,209,232,137,68,36,89,78,69,95,230,137,68,98,109,87,78,69,99,228,137,68,183,85,78,69,111,226,137,68,0,84,78,69,252,224,137,68,98,9,82,78,69,8,223,137,68,84,80,78,69,19,222,137,68,157,78,78,69,152, 220,137,68,98,230,76,78,69,162,219,137,68,48,75,78,69,31,219,137,68,121,73,78,69,41,218,137,68,98,194,71,78,69,166,217,137,68,12,70,78,69,166,217,137,68,85,68,78,69,166,217,137,68,108,85,68,78,69,81,28,139,68,101,0,0 };

			sine.loadPathFromData(sinePathData, sizeof(sinePathData));

			switch (SequencerShapes(i))
			{

			case SawDown:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -1);
				p.lineTo(1, 0);
				hintText = "Saw Down";
				break;
			case SawUp:
				p.startNewSubPath(0, 0);
				p.lineTo(1, -1);
				p.lineTo(1, 0);
				hintText = "Saw Up";
				break;
			case SinDown:
				p = sine;
				hintText = "Sine Down";
				break;
			case SinUp:
				p = sine;
				p.applyTransform(juce::AffineTransform(-1, 0, 0, 0, 1, 0));
				hintText = "Sine Up";
				break;
			case SquareFull:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -1);
				p.lineTo(1, -1);
				p.lineTo(1, 0);
				hintText = "Square Full";
				break;
			case SquareHalf:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -1);
				p.lineTo(0.5, -1);
				p.lineTo(0.5, 0);
				p.lineTo(1, 0);
				hintText = "Square Half";
				break;
			case Triangle:
				p.startNewSubPath(0, 0);
				p.lineTo(0.5, -1);
				p.lineTo(1, 0);
				hintText = "Triangle";
				break;
			}

			juce::PathStrokeType(p.getBounds().getWidth() * 0.05).createStrokedPath(p, p);

			shapeButtons.getLast()->setPath(p);
			shapeButtons.getLast()->setColor(BDSP_COLOR_WHITE, NamedColorsIdentifier(BDSP_COLOR_WHITE).withMultipliedAlpha(universals->lowOpacity));

			shapeButtons.getLast()->setHintBarText(hintText);
		}

		shapeButtons.getFirst()->setToggleState(true, true);

	}



	SequencerComponent* SequencerSection::getSequencer(int idx)
	{
		return Sequencers[idx];
	}



	void SequencerSection::resized()
	{

		float buttonH = 0.1;
		buttonRect = getLocalBounds().toFloat().withHeight(getHeight() * buttonH).reduced(universals->rectThicc);
		auto reduced = getLocalBounds().toFloat().withTop(buttonRect.getBottom()).reduced(universals->rectThicc);


		for (int i = 0; i < shapeButtons.size(); ++i)
		{
			shapeButtons[i]->setBounds(shrinkRectangleToInt(buttonRect.getProportion(juce::Rectangle<float>((float)i / (shapeButtons.size()), 0, 1.0f / (shapeButtons.size()), 1))).reduced(universals->rectThicc));
		}

		auto h = 1.0 / Sequencers.size();
		for (int i = 0; i < Sequencers.size(); ++i)
		{
			SequencerHolders[i]->setBounds(shrinkRectangleToInt(reduced.getProportion(juce::Rectangle<float>(0, i * h, 1, h))));
			Sequencers[i]->setBounds(SequencerHolders[i]->getLocalBounds().reduced(0, universals->rectThicc));
		}
	}

	void SequencerSection::paint(juce::Graphics& g)
	{
		TexturedContainerComponent::paint(g);

		g.setColour(getColor(BDSP_COLOR_DARK));
		g.fillRoundedRectangle(buttonRect, universals->roundedRectangleCurve);

		for (int i = 0; i < shapeButtons.size() - 1; ++i)
		{
			float x = (shapeButtons[i]->getRight() + shapeButtons[i + 1]->getX()) / 2.0f;

			drawDivider(g, juce::Line<float>(x, buttonRect.getY(), x, buttonRect.getBottom()), getColor(BDSP_COLOR_MID), universals->dividerSize);
		}
	}


	void SequencerSection::setTitleFontIndex(int index)
	{
		for (auto s : Sequencers)
		{
			s->getTitleComponent()->setFontIndex(index);
		}
	}

	void SequencerSection::visibilityChanged()
	{
		for (auto s : Sequencers)
		{
			s->setVisible(isVisible());
		}
	}


	int SequencerSection::getSelectedShape()
	{
		for (int i = 0; i < shapeButtons.size(); ++i)
		{
			if (shapeButtons[i]->getToggleState())
			{
				return i + 1;
			}
		}
		return -1;
	}

	void SequencerSection::colorsChanged()
	{
		for (auto* s : Sequencers)
		{
			auto vis = dynamic_cast<SequencerVisualizer*>(s->getVisualizer());
			if (vis != nullptr)
			{
				vis->getVis()->cutoutColor = background;
			}
		}
	}






}// namnepace bdsp
