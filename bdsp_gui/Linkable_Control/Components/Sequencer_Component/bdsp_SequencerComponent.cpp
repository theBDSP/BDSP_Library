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
		case SawDown:
			return lookups->waveLookups->lookupTri(0, proportion, false);
			break;
		case AccDown:
			return 1.0f - proportion * proportion;
			break;
		case DecDown:
			return (1.0f - proportion) * (1.0f - proportion);
			break;
		case SawUp:
			return lookups->waveLookups->lookupTri(1, proportion, false);
			break;
		case AccUp:
			return proportion * proportion;
			break;
		case DecUp:
			return 1.0f - (1.0f - proportion) * (1.0f - proportion);
			break;
		case SquareFull:
			return 1;
			break;
		case SquareHalf:
			return lookups->waveLookups->lookupSqr(0.5, 1.0 - proportion, false);
			break;
		case Triangle:
			return lookups->waveLookups->lookupTri(0.5, proportion, false);
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
		auto rateBounds = juce::Rectangle<float>(0, 0, 4 * square.getWidth(), square.getHeight());

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


			switch (SequencerShapes(i))
			{

			case SawDown:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -1);
				p.lineTo(1, 0);
				hintText = "Saw Down";
				break;
			case AccDown:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -10);
				p.quadraticTo(5, -10, 10, 0);
				hintText = "Accelerate Down";
				break;	
			case DecDown:
				p.startNewSubPath(0, 0);
				p.lineTo(0, -10);
				p.quadraticTo(5, 0, 10, 0);
				hintText = "Decelerate Down";
				break;
			case SawUp:
				p.startNewSubPath(0, 0);
				p.lineTo(1, -1);
				p.lineTo(1, 0);
				hintText = "Saw Up";
				break;
			case AccUp:
				p.startNewSubPath(0, 0);
				p.quadraticTo(5, 0, 10, -10);
				p.lineTo(10, 0);
				hintText = "Accelerate Up";
				break;
			case DecUp:
				p.startNewSubPath(0, 0);
				p.quadraticTo(5, -10, 10, -10);
				p.lineTo(10, 0);
				hintText = "Decelerate Up";
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
