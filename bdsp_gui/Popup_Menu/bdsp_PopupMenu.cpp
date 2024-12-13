#include "bdsp_PopupMenu.h"

namespace bdsp
{
	PopupMenu::PopupMenu(juce::Component* compToAttachTo, GUI_Universals* universalsToUse, bool preview)
		:List(this),
		DesktopComponent(universalsToUse),
		vp(universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		;
		setLossOfFocusClosesWindow(true, &vp.getVerticalScrollBar());
		resetLookAndFeels(universalsToUse);

		addAndMakeVisible(vp);
		vp.setScrollBarsShown(true, false);
		vp.setViewedComponent(&List, false);
		setVisible(false);


		List.previewOnHighlight = preview;

		setFontIndex(2);

	}
	PopupMenu::~PopupMenu()
	{
		setLookAndFeel(nullptr);
	}
	void PopupMenu::resized()
	{

		if (List.getNum() != 0)
		{
			auto itemH = getWidth() * List.heightRatio;
			auto h = itemH * (List.getRows() + 2 * List.itemBorder + List.titleHeightRatio);


			vp.setBoundsRelative(0, 0, 1, 1);
			vp.setScrollDistance(itemH * (1 + List.itemBorder));
			List.setBounds(0, 0, vp.getWidth(), h);
			List.resized();
		}
	}

	juce::Rectangle<int> PopupMenu::ensureBoundsAreContained(juce::Rectangle<int> bounds, int border)
	{
		auto desktop = universals->desktopManager.getBounds().reduced(border);
		auto out = bounds;
		if (!desktop.contains(bounds)) // popup would go off screen
		{
			if (out.getRight() > desktop.getRight()) // need to move left
			{
				out = out.withRightX(desktop.getRight());
			}
			if (out.getX() < desktop.getX()) // need to move right
			{
				out = out.withX(desktop.getX());
			}
			if (out.getBottom() > desktop.getBottom()) // need to move up
			{
				out = out.withBottomY(desktop.getBottom());
			}
			if (out.getY() < desktop.getY()) // need to move down
			{
				out = out.withY(desktop.getY());
			}

		}
		return out;
	}

	void PopupMenu::setBoundsContained(juce::Rectangle<int> bounds, int border)
	{
		auto b = ensureBoundsAreContained(bounds, border);
		setBounds(b);
	}



	void PopupMenu::setHeightRatio(float newRatio)
	{
		List.heightRatio = newRatio;
	}

	void PopupMenu::setItemBorderRatio(float newRatio)
	{
		List.itemBorder = newRatio;
	}

	void PopupMenu::dismissAllActiveMenus()
	{
		manager->hideAllComponents();
	}

	void PopupMenu::scroll(bool up)
	{
		List.scroll(up);
	}

	//void PopupMenu::setColors(const NamedColorsIdentifier& newBkgd, juce::Array<NamedColorsIdentifier> newHighlights, juce::Array<NamedColorsIdentifier> newTexts, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider)
	//{
	//	List.setColors(newBkgd, newHighlights, newTexts, newBorder, newDivider);

	//	GUI_UniversalsChanged();
	//}



	//void PopupMenu::setColors(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newHighlight, const NamedColorsIdentifier& newText, const NamedColorsIdentifier& newHighlightedText, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider)
	//{

	//	auto highlightText = newHighlightedText.isEmpty() ? newText : newHighlightedText;

	//	List.setColors(newBkgd, newHighlight, newText, highlightText, newBorder, newDivider);

	//	GUI_UniversalsChanged();
	//}









	bool PopupMenu::keyPressed(const juce::KeyPress& key)
	{
		return List.keyPressed(key);
	}

	void PopupMenu::lookAndFeelChanged()
	{
		List.setLookAndFeel(&getLookAndFeel());
		vp.setLookAndFeel(&getLookAndFeel());
	}



	void PopupMenu::resetLookAndFeels(GUI_Universals* universalsToUse)
	{
		//		//setLookAndFeel(&universalsToUse->MasterPopupLNF);
	}

	void PopupMenu::setFontIndex(int idx)
	{
		ComponentCore::setFontIndex(idx);
		for (auto i : List.List)
		{
			i->setFontIndex(idx);
		}
	}

	void PopupMenu::addTitle(const juce::String& text, float ratio)
	{
		List.title = text;
		List.titleHeightRatio = ratio;
	}

	void PopupMenu::GUI_UniversalsChanged()
	{
		if (!List.List.isEmpty())
		{
			int idx = List.getHighlightedIndex() == -1 ? 0 : List.getHighlightedIndex();
			vp.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, getColor(List.List[idx]->textColor));
		}
		List.repaint();
		for (auto i : List.List)
		{
			i->repaint();
		}

	}







	//========================================================================================================================================================================================================================



	PopupMenu::MenuList::MenuList(PopupMenu* p)
		:ListComponent(p)
	{
	}
	PopupMenu::MenuList::~MenuList()
	{
		List.clear();
	}






	void PopupMenu::MenuList::resized()
	{
		if (!getBounds().isEmpty())
		{

			insideBorder = getLocalBounds().toFloat().reduced(getWidth() * BDSP_POPUP_MENU_OUTLINE_RATIO);


			postionItemsWithinRectangle(insideBorder.withTrimmedTop(getWidth() * BDSP_POPUP_MENU_ITEM_HEIGHT_RATIO * titleHeightRatio));
		}
	}

	void PopupMenu::MenuList::addItem(const juce::String& text, int returnValue)
	{

		List.add(std::make_unique<MenuItem>(this, text, List.size(), returnValue));
		auto current = List.getLast();
		addAndMakeVisible(current);
		current->setFontIndex(parent->getFontIndex());

		copyAllExplicitColoursTo(*current);

		pushColorsToNewItem();

		setMaxText(text);

		highlightItem(0);

		setRowsAndCols(ceil((float)List.size() / cols), cols);
	}

	void PopupMenu::MenuList::setItems(const juce::StringArray& items, const juce::Array<int>& returnValues)
	{
		clearItems();
		bool customReturns = !returnValues.isEmpty();

		for (int i = 0; i < items.size(); ++i)
		{
			addItem(items[i], customReturns ? returnValues[i] : i);
		}


	}



	bool PopupMenu::MenuList::keyPressed(const juce::KeyPress& key)
	{
		auto code = key.getKeyCode();

		if (code == key.returnKey)
		{
			selectItem(HighlightedIndex);
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}
		else if (code == key.upKey)
		{
			scroll(true);
		}
		else if (code == key.downKey)
		{
			scroll(false);
		}
		else if (code == key.escapeKey)
		{
			dynamic_cast<DesktopComponent*>(parent)->hide();
		}

		return false;
	}








	//========================================================================================================================================================================================================================





	PopupMenu::MenuList::MenuItem::MenuItem(MenuList* p, const juce::String& name, int idx, int r)
		:ListItem(p, idx, r)
	{
		setText(name);
	}
	PopupMenu::MenuList::MenuItem::~MenuItem()
	{
	}

	void PopupMenu::MenuList::MenuItem::paint(juce::Graphics& g)
	{
		auto height = getHeight();
		auto width = getWidth();




		auto alpha = isEnabled() ? 1.0 : universals->disabledAlpha;



		g.setColour(getHoverColor(getColor(highlightColor), getColor(parent->background), isHighlighted(), isMouseOver(), universals->hoverMixAmt).withMultipliedAlpha(alpha));
		if (parent->roundHighlights)
		{
			g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
		}
		else
		{
			g.fillRect(getLocalBounds());
		}


		//g.setColour(findColour(isHighlighted()?juce::Label::backgroundColourId:juce::Label::textColourId));
		g.setColour(getColor(isHighlighted() ? textHighlightedColor : textColor).withMultipliedAlpha(alpha));

		auto Font = universals->Fonts[getFontIndex()].getFont();

		g.setFont(resizeFontToFit(Font, width * 0.9, height * 0.95, parent->maxText));

		drawText(g, g.getCurrentFont(), " " + getText() + " ", getLocalBounds(), false, parent->justification);

	}

}//namespace bdsp

