#include "bdsp_GeneralControlComponent.h"
namespace bdsp
{

	GeneralControlComponent::GeneralControlComponent(GUI_Universals* universalsToUse, int idx, juce::ValueTree* nameValueLocation, const juce::Identifier& nameID)
		:Component(universalsToUse),
		LinkableControlComponent(universalsToUse, idx),
		GUI_Universals::Listener(universalsToUse),
		hintBackground(universalsToUse)
	{
		nameValueLoc = nameValueLocation;
		nameValueID = nameID;
		;


		//================================================================================================================================================================================================

		title = std::make_unique<CenteredEditableLabel>(universals);


		title->setText(nameValueLoc->getProperty(nameValueID).toString(), juce::sendNotification);

		title->setHeightRatio(1.0);


		title->setMaxText(universals->Fonts[title->getFontIndex()].getWidestString(BDSP_MAX_MACRO_NAME_LENGTH));
		nameValueLoc->addListener(this);


		defaultName = nameValueID.toString().upToLastOccurrenceOf("NameID", false, false);
		int num = defaultName.getTrailingIntValue();
		defaultName = defaultName.replaceFirstOccurrenceOf(juce::String(num), " " + juce::String(num));
		name = defaultName;
		title->onEditorShow = [=]()
		{
			if (title->getText().compare(defaultName) == 0)
			{
				title->getCurrentTextEditor()->clear();
			}
			title->copyAllExplicitColoursTo(*title->getCurrentTextEditor());

			title->getCurrentTextEditor()->setInputRestrictions(BDSP_MAX_MACRO_NAME_LENGTH);
			//title->getTitle().getCurrentTextEditor()->setMultiLine(true);
			title->getCurrentTextEditor()->setScrollbarsShown(false);
			//slider.influenceLabel.getTitle().getCurrentTextEditor()->applyFontToAllText(slider.universals->Fonts[slider.influenceLabel.getTitle().getFontIndex()].getFont().withHeight(slider.influenceLabel.getTitle().getHeight() / 2.0));
		};

		title->onEditorHide = [=]()
		{
			title->setText(title->getText().trim(), juce::dontSendNotification);
			if (title->getText().isEmpty())
			{
				title->setText(defaultName, juce::sendNotification);
			}
			universals->undoManager->beginNewTransaction();
			nameValueLoc->setProperty(nameValueID, title->getTextValue(), universals->undoManager);
			name = title->getText();
		};

		//addMouseListener(&slider.influenceLabel.getTitle(), true);




		//labelHRatio = 2 / 3.0;
		title->setFontIndex(2);
//		title->setLookAndFeel(&universals->MasterMacroLNF);
		addAndMakeVisible(title.get());

		//================================================================================================================================================================================================

		GUI_UniversalsChanged();

		addAndMakeVisible(hintBackground, 0);

	}

	GeneralControlComponent::~GeneralControlComponent()
	{
		nameValueLoc->removeListener(this);
	}

	void GeneralControlComponent::GUI_UniversalsChanged()
	{

		title->setColour(juce::Label::backgroundColourId, getColor(BDSP_COLOR_PURE_BLACK));
		title->setColour(juce::Label::textColourId, getColor(BDSP_COLOR_WHITE));
		title->setColour(juce::Label::textWhenEditingColourId, getColor(BDSP_COLOR_WHITE));
		title->setColour(juce::TextEditor::textColourId, getColor(BDSP_COLOR_WHITE));
		title->setColour(juce::TextEditor::highlightedTextColourId, getColor(BDSP_COLOR_WHITE));
		title->setColour(juce::TextEditor::highlightColourId, getColor(BDSP_COLOR_WHITE).withAlpha(universals->lowOpacity));
		title->setColour(juce::CaretComponent::caretColourId, getColor(BDSP_COLOR_WHITE));


	}

	void GeneralControlComponent::resized()
	{
		auto reduced = getLocalBounds().getProportion(juce::Rectangle<float>(0, 0.175, 1, 1-0.175));

		auto topH = reduced.getY() * 0.8;
		titleRect = getLocalBounds().toFloat().withBottom(reduced.getY());


		title->setBounds(getLocalBounds().withHeight(topH).reduced(2 * topH, 0));

		dragComp.setBounds(getWidth() - topH, 0, topH, topH);
		transferComp.setBounds(0, 0, topH, topH);

		hintBackground.setBoundsRelative(0, 0, 1, 1);
	}

	void GeneralControlComponent::setName(const juce::String& newName)
	{
		nameValueLoc->setProperty(nameValueID, newName, universals->undoManager);
	}

	Component* GeneralControlComponent::getHintBackground()
	{
		return &hintBackground;
	}

	CenteredEditableLabel* GeneralControlComponent::getTitleComponent()
	{
		return title.get();
	}

	void GeneralControlComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& propertyID)
	{
		if (propertyID == nameValueID)
		{
			title->setText(treeWhosePropertyHasChanged.getProperty(propertyID).toString(), juce::sendNotification);
			name = title->getText();
		}
	}

} // namespace bdsp
