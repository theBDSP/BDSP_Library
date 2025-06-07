#include "bdsp_ComboBox.h"

namespace bdsp
{
	ComboBox::ComboBox(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:ComboBoxBase(param, universalsToUse, dispNames, changFunc)
	{
		ListHolder = std::make_unique<PopupMenu>(this, universals, true);
		List = &dynamic_cast<PopupMenu*>(ListHolder.get())->List;

		initMenuComponents();

		attach(param, universals->undoManager);

	}



	ComboBox::ComboBox(const juce::StringArray& choiceNames, juce::RangedAudioParameter* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
		:ComboBox(nullptr, universalsToUse, dispNames, changFunc)
	{


		if (!choiceNames.isEmpty())
		{
			setItems(choiceNames, dispNames);
			numItems = choiceNames.size();
		}
		attach(param, universals->undoManager);
	}





	ComboBox::~ComboBox()
	{

	}

	void ComboBox::setHeightRatio(float newRatio)
	{
		dynamic_cast<PopupMenu*>(ListHolder.get())->setHeightRatio(newRatio);
	}


	//========================================================================================================================================================================================================================





	void ComboBox::attach(juce::RangedAudioParameter* param, juce::UndoManager* undo)
	{
		if (param != nullptr)
		{
			auto p = dynamic_cast<juce::AudioParameterChoice*>(param);
			if (p != nullptr) // if parameter has associated string array of names 
			{
				setItems(p->choices, displayNames);
			}

			ChoiceComponentCore::attach(param, undo);


		}

	}


	void ComboBox::setItems(const juce::StringArray& items, const juce::StringArray& dispNames)
	{
		List->setItems(items);

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



	ComboBoxBase::ComboBoxBase(juce::AudioParameterChoice* param, GUI_Universals* universalsToUse, const juce::StringArray& dispNames, const std::function<void(int)>& changFunc)
        :Component(universalsToUse),
        ChoiceComponentCore(param, this)
	{
		;

		displayNames = dispNames;





		onChange = changFunc;


		button = std::make_unique<DropdownButton>(this);


		addAndMakeVisible(button.get());

	}



	ComboBoxBase::ComboBoxBase(const ComboBoxBase& other)
		:ComboBoxBase(other.parameter, other.universals, other.displayNames, other.onChange)
	{
		setLookAndFeel(&other.getLookAndFeel());
	}

	ComboBoxBase::~ComboBoxBase()
	{
	}

	void ComboBoxBase::initMenuComponents()
	{
		setFontIndex(0);



		setColorSchemeClassic(BDSP_COLOR_WHITE, BDSP_COLOR_DARK, NamedColorsIdentifier(BDSP_COLOR_BLACK), NamedColorsIdentifier(BDSP_COLOR_COLOR).withMultipliedAlpha(universals->lowOpacity));

		ListHolder->setLossOfFocusClosesWindow(true, this);

		ListHolder->onHide = [this]()
		{
			repaint();
		};

		ListHolder->onShow = [this]()
		{
			resized();
			repaint();
		};


		List->onSelect = [this](int i) // triggers param change too
		{
			selectItem(i);
		};
		List->onMouseWheelMove = [this](int i, const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
		{
			button->mouseWheelMove(e, mw);
		};

		ListHolder->setLossOfFocusClosesWindow(true, button.get());


	}


	DesktopComponent* ComboBoxBase::getListuniversals()
	{
		return ListHolder.get();
	}

	ListComponent* ComboBoxBase::getListComponent()
	{
		return List;
	}

	void ComboBoxBase::highlightItem(int i)
	{
		List->highlightItem(i);
	}

	void ComboBoxBase::scrollItems(bool up)
	{
		List->scroll(up);
	}

	void ComboBoxBase::setItemEnablement(int idx, bool enabled)
	{
		List->List[idx]->setEnabled(enabled);
	}

	void ComboBoxBase::showList()
	{
		ListHolder->show();
	}

	void ComboBoxBase::hideList()
	{
		ListHolder->hide();
	}

	bool ComboBoxBase::isListVisible()
	{
		return ListHolder->isVisible();
	}

	void ComboBoxBase::resized()
	{
		if (getWidth() > 0) // issues arrise trying to set child components to a size of 0
		{

			button->setBounds(getLocalBounds());

			auto w = getWidth() * menuW;
			auto pos = getBoundsRelativeToDesktopManager().getRelativePoint(0.5, 0.0);
			auto y = pos.y + getHeight() * 1.1;
			auto x = pos.x - w / 2;

			auto h = List->getIdealHeight(w);

			if (y + h > universals->desktopManager.getHeight()) // would be cut off
			{
				y = pos.y - getHeight() * 0.1 - h;
			}

			if (x < 0)
			{
				x = 0;
			}
			else if (x + w > universals->desktopManager.getWidth())
			{
				x = universals->desktopManager.getWidth() - w;
			}

			ListHolder->setBounds(juce::Rectangle<int>(x, y, w, h));
			List->setBounds(ListHolder->getLocalBounds());
			List->resized();
		}
	}

	void ComboBoxBase::setColorSchemeClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const juce::Array<NamedColorsIdentifier>& newHighlight)
	{
		backgroundUp = backgroundDown = newBkgd;
		if (newText.size() == 1)
		{
			definedTextColorUp = definedTextColorDown = newText.getFirst();
		}
		else
		{
			definedTextColorUp = definedTextColorDown = NamedColorsIdentifier();
		}
		List->setColorSchemeClassic(newBkgd, newBorder, newText, newHighlight);
		pushNewColors();
	}

	void ComboBoxBase::setColorSchemeInvertedClassic(const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText)
	{
		backgroundUp = backgroundDown = newBkgd;
		if (newText.size() == 1)
		{
			definedTextColorUp = definedTextColorDown = newText.getFirst();
		}
		else
		{
			definedTextColorUp = definedTextColorDown = NamedColorsIdentifier();
		}
		List->setColorSchemeInvertedClassic(newBkgd, newBorder, newText);
		pushNewColors();
	}

	void ComboBoxBase::setColorSchemeMinimal(const NamedColorsIdentifier& newMenuBkgd, const NamedColorsIdentifier& newBkgd, const NamedColorsIdentifier& newBorder, const juce::Array<NamedColorsIdentifier>& newText, const NamedColorsIdentifier& newDivider, float nonHighlightedAlpha)
	{
		backgroundUp = backgroundDown = newMenuBkgd;

		if (newText.size() == 1)
		{
			definedTextColorUp = definedTextColorDown = newText.getFirst();
		}
		else
		{
			definedTextColorUp = definedTextColorDown = NamedColorsIdentifier();
		}

		List->setColorSchemeMinimal(newBkgd, newBorder, newText, newDivider, nonHighlightedAlpha);
		pushNewColors();
	}



	void ComboBoxBase::setAllColors(const NamedColorsIdentifier& newBackgroundUp, const NamedColorsIdentifier& newBackgroundDown, const NamedColorsIdentifier& newTextUp, const NamedColorsIdentifier& newTextDown, const NamedColorsIdentifier& newMenuHighlight, const NamedColorsIdentifier& newMenuHighlightedText, const NamedColorsIdentifier& menuBackground, const NamedColorsIdentifier& menuText, const NamedColorsIdentifier& newBorder, const NamedColorsIdentifier& newDivider)
	{
		backgroundUp = newBackgroundUp;
		backgroundDown = newBackgroundDown;

		definedTextColorUp = newTextUp;
		definedTextColorDown = newTextDown;

		List->setAllColors(menuBackground, newMenuHighlight, menuText, newMenuHighlightedText, newBorder, newDivider);
		pushNewColors();
	}








	void ComboBoxBase::colorsChanged()
	{
	}




	void ComboBoxBase::setFontIndex(int idx)
	{
		ComponentCore::setFontIndex(idx);
		ListHolder->setFontIndex(idx);
		List->setFontIndex(idx);
	}


	void ComboBoxBase::setItemBorderRatio(float newRatio)
	{
		List->itemBorder = newRatio;
	}

	void ComboBoxBase::setRoundedHighlights(bool shouldRoundHighlights)
	{
		List->roundHighlights = shouldRoundHighlights;
	}

	void ComboBoxBase::setMenuWidth(float newW)
	{
		menuW = newW;
	}

	void ComboBoxBase::setMenuTextHRatio(float newH)
	{
		menuTextHRatio = newH;
		repaint();
	}

	void ComboBoxBase::setDropDownArrow(bool shoudlHaveDropDownArrow)
	{
		dropdownArrow = shoudlHaveDropDownArrow;
		repaint();
	}


	void ComboBoxBase::setIcons(const juce::Array<juce::Path>& iconsToAdd, const juce::BorderSize<float>& border)
	{
		jassert(iconsToAdd.size() == getNumItems());
		//icons.addArray(iconsToAdd);
		for (int i = 0; i < iconsToAdd.size(); ++i)
		{
			icons.add(new juce::Path(iconsToAdd[i]));
		}
		iconPadding = border;
	}

	void ComboBoxBase::setIconBorderSize(juce::BorderSize<float> newBorder)
	{
		iconPadding = newBorder;
	}

	void ComboBoxBase::paint(juce::Graphics& g)
	{
		float w = getWidth();
		float h = getHeight();
		auto border = w * 0.05;

		auto alpha = isEnabled() ? 1.0f : universals->disabledAlpha;

		auto hover = isHovering();// || Menu.isVisible();


		//================================================================================================================================================================================================








		//================================================================================================================================================================================================



		auto triW = dropdownArrow ? h * 0.3 : 0;

		auto textRect = juce::Rectangle<float>(0, 0, w - triW, h).reduced(border, 0);

		auto triRect = juce::Rectangle<float>().leftTopRightBottom(textRect.getRight() + border, 0, getWidth() - border, h);

		auto pathRect = juce::Rectangle<float>().leftTopRightBottom(textRect.getX() + iconPadding.getLeft() * w, textRect.getY() + iconPadding.getTop() * h, textRect.getRight() - iconPadding.getRight() * w, textRect.getBottom() - iconPadding.getBottom() * h);

		for (auto i : icons)
		{
			i->scaleToFit(pathRect.getX(), pathRect.getY(), pathRect.getWidth(), pathRect.getHeight(), true);
		}



		//================================================================================================================================================================================================

		auto bkgd = universals->hoverAdjustmentFunc(getColor(ListHolder->isVisible() ? backgroundDown : backgroundUp), hover).withMultipliedAlpha(alpha);
		auto txt = universals->hoverAdjustmentFunc(getColor(ListHolder->isVisible() ? textDown : textUp), hover).withMultipliedAlpha(alpha);

		//drawContainer(g, s, bkgd.withMultipliedSaturation(Menu.isVisible() ? 1.0 :universals->buttonDesaturation), bkgd.withMultipliedBrightness(1.25f).withAlpha(0.5f), universals->hoverAdjustmentFunc(getColor(background), hover).withMultipliedAlpha(alpha), borderColor * 1.5, borderColor, !Menu.isVisible());

		g.setColour(bkgd);


		juce::Path s = getRoundedRectangleFromCurveBools(getLocalBounds().toFloat(), curvesVal, universals->roundedRectangleCurve);
		g.fillPath(s);



		//g.setColour(universals->hoverAdjustmentFunc(getColor(background), hover).withMultipliedAlpha(alpha));
		//g.strokePath(s, juce::PathStrokeType(borderColor));



		//================================================================================================================================================================================================

		g.setColour(txt);
		if (icons.isEmpty())
		{

			auto font = universals->Fonts[getFontIndex()];
			g.setFont(resizeFontToFit(font.getFont(), textRect.getWidth() * 0.9, textRect.getHeight() * menuTextHRatio, font.getWidestString(altNames)));


			//ga.addLineOfText(g.getCurrentFont(), boxText, 0, 0);
			//juce::Path p;
			//ga.createPath(p);
			//p.applyTransform(juce::AffineTransform().translated(textRect.getCentreX() - p.getBounds().getCentreX(), textRect.getCentreY() - p.getBounds().getCentreY()));
			//g.fillPath(p);
			drawText(g, g.getCurrentFont(), boxText, textRect);
		}
		else
		{
			g.fillPath(*icons.getUnchecked(getIndex()));
		}



		//================================================================================================================================================================================================

		if (dropdownArrow)
		{

			juce::Path triangle;
			auto l = triRect.getWidth();
			h = sqrt(3) / 2 * l;
			triangle.addTriangle(juce::Point<float>(0, 0), juce::Point<float>(-l / 2, -h), juce::Point<float>(l / 2, -h));


			triangle.applyTransform(juce::AffineTransform().translated(triRect.getCentreX(), triRect.getCentreY() + h * 0.5));
			//while (triangle.getBounds().getHeight() > triRect.getHeight() * 0.8)
			//{
			//	triangle.applyTransform(juce::AffineTransform().scaled(0.9, 0.9, triRect.getCentreX(), triRect.getCentreY()));
			//}
			g.fillPath(triangle);
		}
	}

	void ComboBoxBase::setCornerCurves(CornerCurves curveValues)
	{
		curvesVal = curveValues;
		repaint();
	}



	const juce::StringArray& ComboBoxBase::getDisplayNames()
	{
		return displayNames;
	}

	ComboBoxBase::DropdownButton* ComboBoxBase::getButton()
	{
		return button.get();
	}

	void ComboBoxBase::parameterChanged(int idx)
	{
		currentIndex = idx;
		boxText = altNames[idx];//Menu.List.selectItem(idx, false);
		List->highlightItem(idx);

		updateColors();
		ListHolder->repaintThreadChecked();

		repaintThreadChecked();

		if (onChange.operator bool())
		{
			onChange(idx);
		}
	}

	void ComboBoxBase::updateColors()
	{
		if (definedTextColorUp.isEmpty() && !List->List.isEmpty())
		{
			textUp = List->List[currentIndex]->textColor;
		}
		else
		{
			textUp = definedTextColorUp;
		}

		if (definedTextColorDown.isEmpty() && !List->List.isEmpty())
		{
			textDown = List->List[currentIndex]->textHighlightedColor;
		}
		else
		{
			textDown = definedTextColorDown;
		}



		colorsChanged();

	}

	void ComboBoxBase::pushNewColors()
	{
		updateColors();

		repaint();
		List->repaint();
	}

	ComboBoxBase::DropdownButton::DropdownButton(ComboBoxBase* parent)
		:Component(parent->universals)
	{
		p = parent;
	}

	void ComboBoxBase::DropdownButton::mouseDown(const juce::MouseEvent& e)
	{
		if (p->isEnabled())
		{
			preClickState = p->ListHolder->isVisible();
		}
	}

	void ComboBoxBase::DropdownButton::mouseUp(const juce::MouseEvent& e)
	{
		if (p->isEnabled())
		{

			if (e.mods.isCommandDown() || e.mods.isAltDown() || e.mods.isCtrlDown() || e.mods.isMiddleButtonDown())
			{
				p->reset();
				p->hideList();
			}
			else
			{
				preClickState ? p->hideList() : p->showList();
				//preClickState = !preClickState;
			}
		}
	}

	void ComboBoxBase::DropdownButton::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		if (p->isEnabled())
		{
			if (!mw.isReversed)
			{
				if (mw.deltaY < 0)
				{
					p->scrollItems(false);
				}
				else if (mw.deltaY > 0)
				{
					p->scrollItems(true);
				}
			}
			else
			{
				if (mw.deltaY < 0)
				{
					p->scrollItems(true);
				}
				else if (mw.deltaY > 0)
				{
					p->scrollItems(false);
				}
			}
		}
	}



}// namnepace bdsp

//================================================================================================================================================================================================







