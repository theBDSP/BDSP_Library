#include "bdsp_ListComponent.h"
namespace bdsp
{
	ListComponent::ListComponent(ComponentCore* p)
		:Component(p->universals),
		justification(juce::Justification::centred)
	{
		parent = p;

		dropShadow.setColor(getColor(BDSP_COLOR_PURE_BLACK));
	}
	ListComponent::~ListComponent()
	{
		List.clear();
	}

	void ListComponent::setRowsAndCols(int r, int c)
	{
		//jassert(r * c >= List.size() || List.isEmpty());

		rows = r;
		cols = c;

		resized();
	}

	void ListComponent::setColSplits(const juce::Array<std::pair<juce::Range<int>, int>>& splits)
	{
		colSplits = splits;
		resized();
	}



	void ListComponent::postionItemsWithinRectangle(const juce::Rectangle<float>& rect)
	{
		if (colSplits.isEmpty())
		{
			if (!List.isEmpty())
			{
				colsInLastRow = cols - (rows * cols - List.size());
			}



			for (int r = 0; r < rows - 1; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					List[r * cols + c]->setBounds(shrinkRectangleToInt(rect.getProportion(juce::Rectangle<float>(float(c) / cols, (float)r / rows, 1.0 / cols, 1.0 / rows)).reduced(insideBorder.getY() / 2.0)));
				}
			}

			float adjustedX = 0.5 - colsInLastRow / (2.0 * cols);
			//do last row
			for (int c = 0; c < colsInLastRow; ++c)
			{
				List[(rows - 1) * cols + c]->setBounds(shrinkRectangleToInt(rect.getProportion(juce::Rectangle<float>(adjustedX + float(c) / cols, (float)(rows - 1) / rows, 1.0 / cols, 1.0 / rows)).reduced(insideBorder.getY() / 2.0)));
			}

		}
		else
		{
			int maxRows = 0;
			for (auto s : colSplits)
			{
				if (s.first.getLength() > maxRows)
				{
					maxRows = s.first.getLength();
				}
			}

			int splitIdx = 0;
			for (int i = 0; i < List.size(); ++i)
			{
				if (i == colSplits[splitIdx].first.getEnd())
				{
					splitIdx++;
				}

				int c = colSplits[splitIdx].second;
				int r = i - colSplits[splitIdx].first.getStart();

				List[i]->setBounds(shrinkRectangleToInt(rect.getProportion(juce::Rectangle<float>(float(c) / colSplits.size(), (float)r / maxRows, 1.0 / colSplits.size(), 1.0 / maxRows)).reduced(insideBorder.getY() / 2.0)));

			}
		}
	}

	void ListComponent::resized()
	{
		if (!getBounds().isEmpty())
		{

			insideBorder = getLocalBounds().toFloat().reduced(getWidth() * itemBorder);

			dropShadow.setRadius(insideBorder.getY());
			postionItemsWithinRectangle(insideBorder);
		}
	}

	void ListComponent::paint(juce::Graphics& g)
	{
		juce::Path p;
		p.addRoundedRectangle(insideBorder, universals->roundedRectangleCurve);


		dropShadow.render(g, p);


		g.setColour(getColor(background));
		g.fillPath(p);


		//================================================================================================================================================================================================

		if (title.isNotEmpty())
		{
			g.setColour(getColor(BDSP_COLOR_PURE_BLACK));
			auto titleRect = insideBorder.withBottom(getWidth() * BDSP_POPUP_MENU_ITEM_HEIGHT_RATIO * titleHeightRatio);
			drawTextVerticallyCentered(g, universals->Fonts[2], title, titleRect);

			auto b = insideBorder.getX() / 2.0;
			g.drawHorizontalLine(titleRect.getBottom() + b, insideBorder.getX() + b, insideBorder.getRight() - b);
		}
	}

	void ListComponent::paintOverChildren(juce::Graphics& g)
	{
		juce::Path p;
		p.addRoundedRectangle(insideBorder, universals->roundedRectangleCurve);


		g.setColour(getColor(borderColor));
		g.strokePath(p, juce::PathStrokeType(universals->listBorderSize));

		g.setColour(getColor(divider));
		if (colSplits.isEmpty())
		{
			for (int i = 1; i < List.size(); ++i)
			{
				auto y = (List[i - 1]->getBottom() + List[i]->getY()) / 2.0f;
				g.fillRect(juce::Rectangle<float>(List[i]->getX(), y - (universals->dividerSize / 2), List[i]->getWidth(), universals->dividerSize));
			}
			for (int i = 1; i < cols; ++i)
			{
				auto x = i / (float)cols * getWidth();
				g.fillRect(juce::Rectangle<float>(x - (universals->dividerSize / 2), 2 * insideBorder.getY(), universals->dividerSize, getHeight() - 4 * insideBorder.getY()));
			}
		}
		else
		{
			for (auto s : colSplits)
			{
				for (int i = s.first.getStart() + 1; i < s.first.getEnd(); ++i)
				{
					auto y = (List[i - 1]->getBottom() + List[i]->getY()) / 2.0f;
					g.fillRect(juce::Rectangle<float>(List[i]->getX(), y - (universals->dividerSize / 2), List[i]->getWidth(), universals->dividerSize));
				}
			}

			for (int i = 0; i < colSplits.size() - 1; ++i)
			{
				auto x = (List[colSplits[i].first.getStart()]->getRight() + List[colSplits[i + 1].first.getStart()]->getX()) / 2.0f;
				g.fillRect(juce::Rectangle<float>(x - (universals->dividerSize / 2), 2 * insideBorder.getY(), universals->dividerSize, getHeight() - 4 * insideBorder.getY()));
			}
		}
	}

	float ListComponent::getIdealHeight(float width)
	{

		auto itemH = width * heightRatio;
		auto h = itemH * (getRows() + 2 * itemBorder + titleHeightRatio);
		return h;
	}



	void ListComponent::clearItems()
	{
		List.clear();
		maxText = juce::String();
	}

	void ListComponent::setItems(const juce::StringArray& items, const juce::Array<int>& returnValues)
	{
		clearItems();
		bool customReturns = !returnValues.isEmpty();

		for (int i = 0; i < items.size(); ++i)
		{
			List.add(new ListItem(this, i, customReturns ? returnValues[i] : i));
		}
	}

	void ListComponent::setMaxText(const juce::String& newText)
	{
		auto font = universals->Fonts[getFontIndex()].getFont().withHeight(1);


		auto n = font.getStringWidthFloat(newText);
		auto max = font.getStringWidthFloat(maxText);
		if (n > max)
		{
			maxText = newText;

		}


	}


	void ListComponent::setPreviewOnHighlight(bool newState)
	{
		previewOnHighlight = newState;
	}


	void ListComponent::selectItem(int idx, bool sendUpdate)
	{
		if (idx >= 0 && idx < List.size())
		{
			if (List[idx]->isEnabled())
			{


				highlightItem(idx);
				if (onSelect.operator bool() && sendUpdate)
				{
					onSelect(List[idx]->returnValue);
				}
			}
		}
	}

	void ListComponent::highlightItem(int idx)
	{
		if (idx >= 0 && idx < List.size())
		{
			if (List[idx]->isEnabled())
			{
				HighlightedIndex = idx;

				repaint();
			}
		}
	}

	void ListComponent::clearHighlights()
	{
		highlightItem(-1);
	}

	int ListComponent::getHighlightedIndex()
	{
		return HighlightedIndex;
	}

	void ListComponent::scroll(bool up, int num)
	{
		bool finished = false;
		int count = 0;
		for (int i = 0; i < getNum() && !finished; ++i)
		{

			int idx = juce::jlimit(0, getNum() - 1, up ? HighlightedIndex - 1 - i : HighlightedIndex + 1 + i);

			if (!List[idx]->isEnabled())
			{
				continue;
			}
			if (previewOnHighlight)
			{
				if (count == num - 1)
				{
					selectItem(idx);
					finished = true;
				}
				else
				{
					count++;
				}
			}
			else
			{
				if (count == num - 1)
				{
					highlightItem(idx);
					finished = true;
				}
				else
				{
					count++;
				}
			}

		}
	}

	int  ListComponent::getNum()
	{
		return List.size();
	}

	int ListComponent::getRows()
	{
		return (rows < 0 ? List.size() : rows);
	}

	int ListComponent::getCols()
	{
		return cols;
	}

	void ListComponent::renameItem(int idx, const juce::String& newText)
	{
		List[idx]->setText(newText);
	}








	void ListComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		if (!mw.isReversed)
		{
			if (mw.deltaY < 0)
			{
				scroll(false);
			}
			else if (mw.deltaY > 0)
			{
				scroll(true);
			}
		}
		else
		{
			if (mw.deltaY < 0)
			{
				scroll(true);
			}
			else if (mw.deltaY > 0)
			{
				scroll(false);
			}
		}
	}

	void ListComponent::setColorSchemeClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlight)
	{
		setAllColors(newBkgd, newHighlight, newText, newText, newBorder, NamedColorsIdentifier());
	}

	void ListComponent::setColorSchemeInvertedClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText)
	{
		setAllColors(newBkgd, newText, newText, newBkgd, newBorder, NamedColorsIdentifier());
	}

	void ListComponent::setColorSchemeMinimal(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const NamedColorsIdentifier& newDivider, float nonHighlightedAlpha)
	{
		auto highlightedText = newText;
		auto text = newText;

		for (int i = 0; i < text.size(); ++i)
		{
			text.set(i, highlightedText[i].withMultipliedAlpha(nonHighlightedAlpha));
		}

		setAllColors(newBkgd, NamedColorsIdentifier(), text, highlightedText, newBorder, newDivider);
	}

	void ListComponent::setAllColors(const NamedColorsIdentifier& newBkgd, const juce::Array<NamedColorsIdentifier>& newHighlight, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlightedText, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider)
	{

		background = newBkgd;
		borderColor = newBorder;
		divider = newDivider;

		juce::Array<NamedColorsIdentifier> txt = newText;
		if (newText.size() == 1)
		{
			txt.resize(List.size());
			txt.fill(newText.getFirst());
		}


		juce::Array<NamedColorsIdentifier> highlight = newHighlight;
		if (newHighlight.size() == 1)
		{
			highlight.resize(List.size());
			highlight.fill(newHighlight.getFirst());
		}


		juce::Array<NamedColorsIdentifier> highlightText = newHighlightedText;
		if (newHighlightedText.size() == 1)
		{
			highlightText.resize(List.size());
			highlightText.fill(newHighlightedText.getFirst());
		}

		for (int i = 0; i < List.size(); ++i)
		{
			List[i]->textColor = txt[i];
			List[i]->textHighlightedColor = highlightText[i];
			List[i]->highlightColor = highlight[i];
		}

		newItemColorAssigner = [=](int idx)
		{
			int i = idx < newHighlight.size() ? idx : newHighlight.size() - 1;
			List[idx]->textColor = txt[i];
			List[idx]->textHighlightedColor = highlightText[i];
			List[idx]->highlightColor = highlight[i];
		};
	}





	void ListComponent::pushColorsToNewItem()
	{
		if (newItemColorAssigner.operator bool())
		{
			newItemColorAssigner(List.size() - 1);
		}
	}



	//========================================================================================================================================================================================================================





	ListItem::ListItem(ListComponent* p, int idx, int retValue)
		:Component(p->universals)
	{
		parent = p;
		setWantsKeyboardFocus(true);
		setLookAndFeel(&parent->getLookAndFeel());
		parent->addAndMakeVisible(this);
		returnValue = retValue < 0 ? idx : retValue;
		index = idx;

	}
	ListItem::~ListItem()
	{
		setLookAndFeel(nullptr);
	}
	void ListItem::mouseUp(const juce::MouseEvent& e)
	{
		if (isEnabled())
		{
			parent->selectItem(index);
		}
	}



	void ListItem::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		if (parent->onMouseWheelMove.operator bool())
		{
			parent->onMouseWheelMove(returnValue, e, mw);
		}
	}


	void ListItem::mouseEnter(const juce::MouseEvent& e)
	{
		if (parent->onMouseEnter.operator bool())
		{
			parent->onMouseEnter(returnValue, e);
		}
	}

	void ListItem::mouseExit(const juce::MouseEvent& e)
	{
		if (parent->onMouseExit.operator bool())
		{
			parent->onMouseExit(returnValue, e);
		}
	}

	bool ListItem::keyPressed(const juce::KeyPress& key)
	{
		return false;
	}



	void ListItem::enablementChanged()
	{
		setInterceptsMouseClicks(isEnabled(), isEnabled());
	}

	bool ListItem::isFirst()
	{
		return index == 0;
	}

	bool ListItem::isLast()
	{
		return index == parent->List.size() - 1;
	}

	bool ListItem::isHighlighted()
	{
		return index == parent->getHighlightedIndex();
	}
	inline void ListItem::setText(const juce::String& s)
	{
		text = s;
		repaint();
	}
	inline juce::String ListItem::getText()
	{
		return text;
	}
} // namespace bdsp
