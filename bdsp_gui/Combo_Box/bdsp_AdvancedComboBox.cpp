#include "bdsp_AdvancedComboBox.h"
namespace bdsp
{
	AdvancedComboBox::AdvancedComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:ComboBoxBase(param, universalsToUse, dispNames, changFunc),
		AdvancedList(this),
		iconPlacement(juce::RectanglePlacement::xLeft | juce::RectanglePlacement::yMid),
		textJustification(juce::Justification::centred)
	{
		pathStorage = paths;

		Listuniversals = std::make_unique<DesktopComponent>(universals);
		List = &AdvancedList;

		Listuniversals->addAndMakeVisible(AdvancedList);
		initMenuComponents();
	}

	AdvancedComboBox::AdvancedComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const juce::Array<juce::Path>& paths, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:AdvancedComboBox(nullptr, universalsToUse, paths, dispNames, changFunc)
	{
		if (!choiceNames.isEmpty())
		{
			setItems(choiceNames, paths, dispNames);
			numItems = choiceNames.size();
		}
		attach(param, universals->undoManager);
	}

	AdvancedComboBox::~AdvancedComboBox()
	{
	}

	/*void AdvancedComboBox::resized()
	{
		ComboBoxBase::resized();
		if (getNumItems() != 0)
		{
			auto w = getWidth() * menuW;
			auto h = List->getIdealHeight(w);

			auto desktopPos = getBoundsRelativeToDesktopManager().getCentre() + juce::Point<int>(-w / 2, getHeight() / 2);

			Listuniversals->setBounds(juce::Rectangle<int>(w, h).withPosition(desktopPos));
			List->setBoundsRelative(0, 0, 1, 1);
			List->resized();
		}

	}*/

	void AdvancedComboBox::attach(juce::RangedAudioParameter* param, juce::UndoManager* undo)
	{
		if (param != nullptr)
		{
			auto p = dynamic_cast<juce::AudioParameterChoice*>(param);
			if (p != nullptr) // if parameter has associated string array of names 
			{
				setItems(p->choices, pathStorage, displayNames);
			}

			ChoiceComponentCore::attach(param, undo);


		}
	}

	void AdvancedComboBox::setItems(const juce::StringArray& items, const juce::Array<juce::Path>& shapes, const juce::StringArray& dispNames)
	{
		AdvancedList.setItems(items, shapes);

		if (!dispNames.isEmpty())
		{
			altNames = dispNames;
		}
		else
		{
			altNames = items;
		}
	}

	void AdvancedComboBox::setRowsAndCols(int rows, int cols)
	{
		List->setRowsAndCols(rows, cols);
	}

	void AdvancedComboBox::setIconPlacement(juce::RectanglePlacement placement)
	{
		iconPlacement = placement;
	}

	void AdvancedComboBox::setJustification(juce::Justification justification)
	{
		textJustification = justification;
	}


	//================================================================================================================================================================================================


	AdvancedComboBox::AdvancedList::AdvancedList(AdvancedComboBox* p)
		:ListComponent(p)
	{
	}

	AdvancedComboBox::AdvancedList::~AdvancedList()
	{
	}



	void AdvancedComboBox::AdvancedList::setItems(const juce::StringArray& names, const juce::Array<juce::Path>& paths, const juce::Array<int>& returnValues)
	{
		jassert(names.size() == paths.size());

		clearItems();
		bool customReturns = !returnValues.isEmpty();

		for (int i = 0; i < names.size(); i++)
		{
			addItem(names[i], paths[i], customReturns ? returnValues[i] : i);
		}

	}

	void AdvancedComboBox::AdvancedList::addItem(const juce::String& name, juce::Path path, int returnValue)
	{
		List.add(std::make_unique<AdvancedListItem>(this, name, path, List.size(), returnValue));
		auto current = List.getLast();
		addAndMakeVisible(current);
		current->setFontIndex(parent->getFontIndex());

		copyAllExplicitColoursTo(*current);

		pushColorsToNewItem();

		setMaxText(name);

		highlightItem(0);
	}

	bool AdvancedComboBox::AdvancedList::keyPressed(const juce::KeyPress& key)
	{
		auto code = key.getKeyCode();

		if (code == key.returnKey)
		{
			selectItem(HighlightedIndex);
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}
		else if (code == key.leftKey)
		{
			scroll(true);
		}
		else if (code == key.rightKey)
		{
			scroll(false);
		}
		else if (code == key.upKey)
		{
			scroll(true, cols);
		}
		else if (code == key.downKey)
		{
			scroll(false, cols);
		}
		else if (code == key.escapeKey)
		{
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}

		return false;
	}


	//================================================================================================================================================================================================



	AdvancedComboBox::AdvancedList::AdvancedListItem::AdvancedListItem(ListComponent* p, const juce::String& name, juce::Path path, int idx, int retValue)
		:ListItem(p, idx, retValue)
	{
		parentACB = dynamic_cast<AdvancedComboBox*>(p->parent);
		icon = path;
		text = name;
	}

	void AdvancedComboBox::AdvancedList::AdvancedListItem::resized()
	{
		//forceAspectRatio(1.15);
		ListItem::resized();
	}

	void AdvancedComboBox::AdvancedList::AdvancedListItem::paint(juce::Graphics& g)
	{

		auto w = getWidth();
		auto h = getHeight();
		auto pathBounds = confineToAspectRatio(getLocalBounds().toFloat(), 1, parentACB->iconPlacement);
		auto textBounds = getLocalBounds().toFloat();


		if (parentACB->iconPlacement.testFlags(juce::RectanglePlacement::xLeft))
		{
			pathBounds.translate(parentACB->iconPadding.getLeft() * w, 0);

			pathBounds.removeFromTop(parentACB->iconPadding.getTop() * h);
			pathBounds.removeFromBottom(parentACB->iconPadding.getBottom() * h);

			textBounds.removeFromLeft(pathBounds.getRight() + parentACB->iconPadding.getRight() * w);
		}
		else if (parentACB->iconPlacement.testFlags(juce::RectanglePlacement::xRight))
		{
			pathBounds.translate(-parentACB->iconPadding.getRight() * w, 0);

			pathBounds.removeFromTop(parentACB->iconPadding.getTop() * h);
			pathBounds.removeFromBottom(parentACB->iconPadding.getBottom() * h);

			textBounds.removeFromRight(pathBounds.getX() + parentACB->iconPadding.getLeft() * w);
		}

		if (parentACB->iconPlacement.testFlags(juce::RectanglePlacement::yTop))
		{
			pathBounds.translate(0, parentACB->iconPadding.getTop() * h);

			pathBounds.removeFromLeft(parentACB->iconPadding.getLeft() * w);
			pathBounds.removeFromRight(parentACB->iconPadding.getRight() * w);

			textBounds.removeFromTop(pathBounds.getBottom() + parentACB->iconPadding.getBottom() * h);
		}
		else if (parentACB->iconPlacement.testFlags(juce::RectanglePlacement::yBottom))
		{
			pathBounds.translate(0, -parentACB->iconPadding.getBottom() * h);

			pathBounds.removeFromLeft(parentACB->iconPadding.getLeft() * w);
			pathBounds.removeFromRight(parentACB->iconPadding.getRight() * w);

			textBounds.removeFromBottom(pathBounds.getY() + parentACB->iconPadding.getTop() * h);
		}



		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;

		g.setColour(getHoverColor(getColor(highlightColor), getColor(parent->background), isHighlighted(), isMouseOver(), universals->hoverMixAmt).withMultipliedAlpha(alpha));

		if (parent->roundHighlights)
		{
			g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		}
		else
		{
			g.fillRect(getLocalBounds());
		}
		/*g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(pathBounds, universals->roundedRectangleCurve);*/




		g.setColour(getColor(isHighlighted() ? textHighlightedColor : textColor).withMultipliedAlpha(alpha));

		auto Font = universals->Fonts[getFontIndex()].getFont();

		g.setFont(resizeFontToFit(Font, textBounds.getWidth() * 0.9f, textBounds.getHeight() * 0.95f, parent->maxText));

		drawText(g, g.getCurrentFont(), getText(), textBounds, false, parentACB->textJustification);

		scaleToFit(icon, pathBounds);
		g.fillPath(icon);
	}




	//================================================================================================================================================================================================

	GroupedComboBox::GroupedComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const ItemGroupings& groupings, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:ComboBoxBase(param, universalsToUse, dispNames, changFunc),
		groupedList(this)
	{
		groups = groupings;
		Listuniversals = std::make_unique<DesktopComponent>(universals);
		List = &groupedList;

		Listuniversals->addAndMakeVisible(groupedList);

		setItems(param->choices, groupings, dispNames);
		initMenuComponents();
		attach(param, universals->undoManager);
	}

	GroupedComboBox::GroupedComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const ItemGroupings& groupings, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:GroupedComboBox(nullptr, universalsToUse, groupings, dispNames, changFunc)
	{
		if (!choiceNames.isEmpty())
		{
			setItems(choiceNames, groupings, dispNames);
			numItems = choiceNames.size();
		}
		attach(param, universals->undoManager);
	}

	GroupedComboBox::~GroupedComboBox()
	{
	}

	/*void GroupedComboBox::resized()
	{
		ComboBoxBase::resized();
		if (getNumItems() != 0)
		{
			auto w = getWidth() * menuW;
			auto h = List->getIdealHeight(w);

			auto desktopPos = getBoundsRelativeToDesktopManager().getCentre() + juce::Point<int>(-w / 2, getHeight() / 2);

			Listuniversals->setBounds(juce::Rectangle<int>(w, h).withPosition(desktopPos));
			List->setBoundsRelative(0, 0, 1, 1);
			List->resized();
		}

	}*/

	void GroupedComboBox::resized()
	{
		ComboBoxBase::resized();
		auto listBounds = List->getLocalBounds().toFloat();


		for (int g = 0; g < groups.size(); g++)
		{
			auto rect = shrinkRectangleToInt(listBounds.getProportion(juce::Rectangle<float>(0, (float)g / groups.size(), 1, 1.0f / groups.size())).reduced(universals->rectThicc));
			groupedList.setGroupBounds(g, rect);
		}
	}

	void GroupedComboBox::attach(juce::RangedAudioParameter* param, juce::UndoManager* undo)
	{
		if (param != nullptr)
		{
			auto p = dynamic_cast<juce::AudioParameterChoice*>(param);
			if (p != nullptr) // if parameter has associated string array of names 
			{
				setItems(p->choices, groups, displayNames);
			}

			ChoiceComponentCore::attach(param, undo);


		}
	}

	void GroupedComboBox::setItems(const juce::StringArray& items, const ItemGroupings& groupings, const juce::StringArray& dispNames)
	{
		groupedList.setItems(items, groupings);
		groupedList.setRowsAndCols(groupings.size(), 1);
		if (!dispNames.isEmpty())
		{
			altNames = dispNames;
		}
		else
		{
			altNames = items;
		}
	}




	//================================================================================================================================================================================================


	GroupedComboBox::GroupedList::GroupedList(GroupedComboBox* p)
		:ListComponent(p),
		GUI_Universals::Listener(p->universals)
	{
		parentGCB = p;
	}

	GroupedComboBox::GroupedList::~GroupedList()
	{
	}

	void GroupedComboBox::GroupedList::setGroupBounds(int groupNum, juce::Rectangle<int> bounds, float border)
	{
		if (border < 0)
		{
			border = universals->rectThicc;
		}

		auto r = parentGCB->groups[groupNum].first;

		Titles[groupNum]->setBounds(shrinkRectangleToInt(bounds.toFloat().getProportion(juce::Rectangle<float>(0, 0, 1, 0.5)).reduced(border)));
		for (int i = r.getStart(); i < r.getEnd(); i++)
		{
			List[i]->setBounds(shrinkRectangleToInt(bounds.toFloat().getProportion(juce::Rectangle<float>((float)(i - r.getStart()) / r.getLength(), 0.5, 1.0f / r.getLength(), 0.5)).reduced(border)));
		}

	}

	void GroupedComboBox::GroupedList::paint(juce::Graphics& g)
	{
		ListComponent::paint(g);
		int y;
		for (int gr = 0; gr < parentGCB->groups.size(); ++gr)
		{

			g.setColour(getColor(borderColor));
			y = (Titles[gr]->getBottom() + List[parentGCB->groups[gr].first.getStart()]->getY()) / 2;

			g.drawHorizontalLine(y, Titles[gr]->getX(), Titles[gr]->getRight());

			if (gr < parentGCB->groups.size() - 1)
			{
				g.setColour(juce::Colours::green);
				y = (Titles[gr + 1]->getY() + List[parentGCB->groups[gr].first.getStart()]->getBottom()) / 2;

				g.drawHorizontalLine(y, Titles[gr]->getX(), Titles[gr]->getRight());
			}

			auto group = parentGCB->groups[gr];

			for (int i = group.first.getStart(); i < group.first.getEnd() - 1; i++)
			{
				int x = (List[i]->getRight() + List[i + 1]->getX()) / 2;

				g.drawVerticalLine(x, List[i]->getY(), List[i]->getBottom());
			}

		}


	}


	void GroupedComboBox::GroupedList::setItems(const juce::StringArray& names, const ItemGroupings& groupings, const juce::Array<int>& returnValues)
	{

		clearItems();
		Titles.clear();
		bool customReturns = !returnValues.isEmpty();

		juce::Justification j = juce::Justification::centredRight;


		for (auto& g : groupings)
		{
			Titles.add(new Label(universals));
			Titles.getLast()->setText(g.second, juce::sendNotification);
			addAndMakeVisible(Titles.getLast());
		}

		int g = 0;
		for (int i = 0; i < names.size(); i++)
		{
			if (g >= groupings.size()) // group index beyond provided groupings
			{
				j = i == names.size() - 1 ? juce::Justification::centredLeft : juce::Justification::centred;
			}
			else if (i < groupings[g].first.getEnd()) // still within current grouping
			{
				j = juce::Justification::centred;
			}
			else // goto next group
			{
				g++;
				j = juce::Justification::centredRight;
			}
			addItem(names[i], j, customReturns ? returnValues[i] : i);
		}

	}

	void GroupedComboBox::GroupedList::addItem(const juce::String& name, juce::Justification just, int returnValue)
	{
		List.add(std::make_unique<GroupedListItem>(this, name, just, List.size(), returnValue));
		auto current = List.getLast();
		addAndMakeVisible(current);
		current->setFontIndex(parent->getFontIndex());

		copyAllExplicitColoursTo(*current);

		pushColorsToNewItem();

		setMaxText(name);

		highlightItem(0);
	}

	bool GroupedComboBox::GroupedList::keyPressed(const juce::KeyPress& key)
	{
		auto code = key.getKeyCode();

		if (code == key.returnKey)
		{
			selectItem(HighlightedIndex);
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}
		else if (code == key.leftKey)
		{
			scroll(true);
		}
		else if (code == key.rightKey)
		{
			scroll(false);
		}
		else if (code == key.upKey)
		{
			scroll(true, cols);
		}
		else if (code == key.downKey)
		{
			scroll(false, cols);
		}
		else if (code == key.escapeKey)
		{
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}

		return false;
	}

	void GroupedComboBox::GroupedList::GUI_UniversalsChanged()
	{
		for (int i = 0; i < Titles.size(); i++)
		{
			Titles[i]->setColour(juce::Label::textColourId, getColor(List[parentGCB->groups[i].first.getStart()]->textColor));
		}

	}


	//================================================================================================================================================================================================



	GroupedComboBox::GroupedList::GroupedListItem::GroupedListItem(ListComponent* p, const juce::String& name, juce::Justification just, int idx, int retValue)
		:ListItem(p, idx, retValue),
		justification(just)
	{
		parentGCB = dynamic_cast<GroupedComboBox*>(p->parent);
		text = name;
	}

	void GroupedComboBox::GroupedList::GroupedListItem::resized()
	{
		//forceAspectRatio(1.15);
		ListItem::resized();
	}

	void GroupedComboBox::GroupedList::GroupedListItem::paint(juce::Graphics& g)
	{




		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;
		alpha *= isHighlighted() ? 1.0f : 0.5f;
		g.setColour(getHoverColor(getColor(highlightColor), getColor(parent->background), isHighlighted(), isMouseOver(), universals->hoverMixAmt).withMultipliedAlpha(alpha));

		if (parent->roundHighlights)
		{
			g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		}
		else
		{
			g.fillRect(getLocalBounds());
		}
		/*g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
		g.fillRoundedRectangle(pathBounds, universals->roundedRectangleCurve);*/




		g.setColour(getColor(textColor).withMultipliedAlpha(alpha));

		auto Font = getFont();

		g.setFont(resizeFontToFit(Font, getWidth() * 0.9f, getHeight() * 0.95f, parent->maxText));

		drawText(g, g.getCurrentFont(), getText(), getLocalBounds(), false, justification);
	}


} // namespace bdsp