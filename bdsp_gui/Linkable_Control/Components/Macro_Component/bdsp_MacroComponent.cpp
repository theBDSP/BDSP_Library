#include "bdsp_MacroComponent.h"

namespace bdsp
{

	MacroControllerObject::MacroControllerObject(juce::RangedAudioParameter* macroParameter)
	{
		macroParam = macroParameter;

		macroParam->addListener(this);
	}

	juce::RangedAudioParameter* MacroControllerObject::getMacroParameter()
	{
		return macroParam;
	}



	float MacroControllerObject::update()
	{
		return value;
	}
	void MacroControllerObject::parameterValueChanged(int parameterIndex, float newValue)
	{
		value = newValue;
		//update();
	}
	void MacroControllerObject::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
	{
	}
	//================================================================================================================================================================================================


	MacroComponent::MacroComponent(GUI_Universals* universalsToUse, int macroIdx, juce::ValueTree* nameValueLocation)
        :ComponentCore(this, universalsToUse),
        LinkableControlComponent(universalsToUse, macroIdx),
        GUI_Universals::Listener(universalsToUse),
        slider(universalsToUse, "Macro " + juce::String(macroIdx + 1), true)

	{
		displayName = "M" + juce::String(macroIdx + 1);
		defaultName = "Macro " + juce::String(macroIdx + 1);

		name = nameValueLocation->getProperty(juce::Identifier("Macro" + juce::String(macroIdx + 1) + "NameID")).toString();
		slider.label.setTitle(name);

		ID = defaultName.removeCharacters(" ") + "ID";

		//slider.influenceLabel.setTitle(title);







		//================================================================================================================================================================================================

		setInterceptsMouseClicks(true, true);



		removeAllHoverListeners();
		slider.label.removeHoverPartner(&slider);
		slider.label.getTitle().removeHoverPartner(&slider);

		slider.label.removeMouseListener(this);
		slider.label.removeMouseListener(&slider);
		slider.label.getTitle().removeMouseListener(this);
		slider.label.getTitle().removeMouseListener(&slider);
		slider.label.getTitle().setEditable(true, true, false);


		slider.label.getTitle().setMaxText(slider.universals->Fonts[slider.label.getTitle().getFontIndex()].getWidestString(BDSP_MAX_MACRO_NAME_LENGTH));
		nameValueLocation->addListener(this);

		slider.label.getTitle().setRepaintsOnMouseActivity(true);


		slider.label.getTitle().onEditorShow = [=]()
		{
			if (slider.label.getTitle().getText() == defaultName)
			{
				slider.label.getTitle().getCurrentTextEditor()->clear();
			}

			//slider.label.getTitle().copyAllExplicitColoursTo(*slider.label.getTitle().getCurrentTextEditor());


			slider.label.getTitle().getCurrentTextEditor()->setInputRestrictions(BDSP_MAX_MACRO_NAME_LENGTH);
			//slider.label.getTitle().getCurrentTextEditor()->setMultiLine(true);
			slider.label.getTitle().getCurrentTextEditor()->setScrollbarsShown(false);
			//slider.influenceLabel.getTitle().getCurrentTextEditor()->applyFontToAllText(slider.universals->Fonts[slider.influenceLabel.getTitle().getFontIndex()].getFont().withHeight(slider.influenceLabel.getTitle().getHeight() / 2.0));
		};

		slider.label.getTitle().onEditorHide = [=]()
		{
			slider.label.getTitle().setText(slider.label.getTitle().getText().trim(), juce::dontSendNotification);
			if (slider.label.getTitle().getText().isEmpty())
			{
				slider.label.getTitle().setText("Macro " + juce::String(macroIdx + 1), juce::sendNotification);
			}
			universals->undoManager->beginNewTransaction();
			name = slider.label.getTitle().getTextValue().toString();
			nameValueLocation->setProperty("Macro" + juce::String(macroIdx + 1) + "NameID", slider.label.getTitle().getTextValue(), universals->undoManager);
		};

		//addMouseListener(&slider.influenceLabel.getTitle(), true);

		macroIndex = macroIdx;


		//labelHRatio = 2 / 3.0;

//		slider.label.getTitle().setLookAndFeel(&slider.universals->MasterMacroLNF);
		slider.label.getTitle().setPaintingIsUnclipped(true);

		addAndMakeVisible(slider);

		addAndMakeVisible(dragComp);
		addAndMakeVisible(transferComp);

		nameValueLoc = nameValueLocation;

		GUI_UniversalsChanged();
	}


	MacroComponent::~MacroComponent()
	{
		slider.label.getTitle().setLookAndFeel(nullptr);
		nameValueLoc->removeListener(this);
	}


	void MacroComponent::colorSet()
	{
		slider.setSliderColor(c, caret);
	}

	void MacroComponent::resized()
	{


		//forceAspectRatio(1.25 / (1.125 + BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO + BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO));

		auto reduced = getLocalBounds();

		auto r = reduced.getHeight() * 0.5;


		//auto s = (juce::MathConstants<float>::sqrt2 - 1) * r;

		slider.setBoundsToIncludeLabel(reduced.withTrimmedTop(0.25 * r));
		slider.label.setBounds(juce::Rectangle<int>().leftTopRightBottom(0, slider.getBottom(), getWidth(), getHeight()));

		auto s = 2 * slider.getY();

		auto x1 = juce::jmin(1.5 * s, (double)slider.getX());
		auto x2 = juce::jmax(getWidth() - 1.5 * s, (double)slider.getRight());

		transferComp.setBounds(juce::Rectangle<int>(s, s).withCentre(juce::Point<int>(x1, slider.getY())));
		dragComp.setBounds(juce::Rectangle<int>(s, s).withCentre(juce::Point<int>(x2, slider.getY())));




	}

	void MacroComponent::parentHierarchyChanged()
	{
		//if (getParentComponent() != nullptr && hasArrows)
		//{
		//	getParentComponent()->addAndMakeVisible(dragComp);
		//}
		slider.parentHierarchyChanged();
	}



	CircleSlider* MacroComponent::getSlider()
	{
		return &slider;
	}

	void MacroComponent::setName(const juce::String& newName)
	{
		nameValueLoc->setProperty("Macro" + juce::String(macroIndex + 1) + "NameID", newName, universals->undoManager);
	}













	void MacroComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& propertyID)
	{
		if (propertyID == juce::Identifier("Macro" + juce::String(macroIndex + 1) + "NameID"))
		{
			name = treeWhosePropertyHasChanged.getProperty(propertyID).toString();
			slider.label.getTitle().setText(name, juce::sendNotification);
		}
	}

	juce::Array<juce::WeakReference<HoverableComponent>> MacroComponent::getComponentsThatShareHover()
	{

		return { &slider, &dragComp };

	}

	void MacroComponent::GUI_UniversalsChanged()
	{
		slider.label.getTitle().setColour(juce::Label::backgroundColourId, getColor(BDSP_COLOR_PURE_BLACK));
		slider.label.getTitle().setColour(juce::TextEditor::highlightColourId, getColor(BDSP_COLOR_WHITE));
		slider.label.getTitle().setColour(juce::CaretComponent::caretColourId, getColor(BDSP_COLOR_WHITE));

		slider.label.getTitle().setColour(juce::TextEditor::highlightColourId, slider.label.findColour(juce::TextEditor::highlightColourId));
		slider.label.getTitle().setColour(juce::TextEditor::highlightedTextColourId, slider.label.findColour(juce::TextEditor::textColourId));

		//slider.influenceLabel.getTitle().setColour(juce::Label::textColourId, getColor(slider.textColor));
		//slider.influenceLabel.getTitle().setColour(juce::Label::textWhenEditingColourId, getColor(slider.textEditColor));
		//slider.influenceLabel.getTitle().setColour(juce::TextEditor::highlightedTextColourId, getColor(slider.textHighlightColor));

	}






	//================================================================================================================================================================================================




	MacroSection::MacroSection(GUI_Universals* universalsToUse, const  NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation)
		:TexturedContainerComponent(universalsToUse, backgroundColor)
	{
		layout = { BDSP_NUMBER_OF_MACROS };
		;
		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			macros.add(std::make_unique<MacroComponent>(universals, i, nameValueLocation));
			addAndMakeVisible(macros[i]);
			macros.getLast()->setColor(BDSP_COLOR_WHITE, BDSP_COLOR_COLOR);
			macros.getLast()->getSlider()->attach(*universalsToUse->APVTS, macros.getLast()->getID().toString());
		}


	}



	MacroComponent* MacroSection::getMacro(int idx)
	{
		return macros[idx];
	}




	void MacroSection::resized()
	{
		int i = 0;
		auto bH = 0.025;
		auto h = (1.0 - (layout.size() + 1) * bH) / layout.size();
		for (int r = 0; r < layout.size(); ++r)
		{
			auto w = 1.0 / (layout[r] + 1.25);
			auto border = (1 - layout[r] * w) / (layout[r] + 1);
			for (int c = 0; c < layout[r]; ++c)
			{
				macros[i]->setBounds(shrinkRectangleToInt(getLocalBounds().getProportion(juce::Rectangle<float>((c + 1) * border + c * w, (r + 1) * bH + r * h, w, h - 2 * bH))))/*.getProportion(juce::Rectangle<float>(0.025, 0.05, 0.95, 0.9))))*/;
				++i;
			}
		}
	}

	void MacroSection::paint(juce::Graphics& g)
	{
		TexturedContainerComponent::paint(g);

		for (int i = 0; i < macros.size() - 1; ++i)
		{
			auto x = (macros[i]->getRight() + macros[i + 1]->getX()) / 2;

			if (shouldDrawDivider)
			{
				drawDivider(g, juce::Line<float>(x, universals->dividerBorder, x, getHeight() - universals->dividerBorder), universals->colors.getColor(BDSP_COLOR_LIGHT), universals->dividerSize);
			}

		}

	}

	void MacroSection::setLayout(juce::Array<int>& newLayout)
	{
		layout = newLayout;
		resized();
	}







}// namnepace bdsp
