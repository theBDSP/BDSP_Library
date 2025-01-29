#include "bdsp_TabsComponent.h"
namespace bdsp
{

	TabsComponent::TabsComponent(GUI_Universals* universalsToUse, const juce::StringArray& tabNames)
		:Component(universalsToUse)
	{
		;
		setNames(tabNames);

		setColors(BDSP_COLOR_DARK, BDSP_COLOR_COLOR);


	}


	void TabsComponent::setVertical(bool shouldBeVertical)
	{
		isVertical = shouldBeVertical;
		resized();
	}

	void TabsComponent::setTabRatio(float newTabRatio)
	{
		tabRatio = newTabRatio;
		resized();
	}

	void TabsComponent::setCorners(CornerCurves newCorners)
	{
		corners = newCorners;
		repaint();
	}

	void TabsComponent::setNames(const juce::StringArray& newNames)
	{
		tabNames = newNames;
		buttons.clear();
		pages.clear();

		for (int i = 0; i < newNames.size(); ++i)
		{

			pages.add(new juce::Component());
			addChildComponent(pages.getLast(), 0);

			buttons.add(new TextButton(universals));
			addAndMakeVisible(buttons.getLast());
			buttons.getLast()->setText(newNames[i]);

			buttons.getLast()->setRadioGroupId(1);
			buttons.getLast()->setClickingTogglesState(true);



			buttons.getLast()->setHasOutline(false);

			buttons.getLast()->setFontIndex(1);
			//buttons.getLast()->setGradientBackground(true);

			buttons.getLast()->onClick = [=]()
			{
				if (buttons[i]->getToggleState())
				{
					notifyTabSelection(i);
					//universals->desktopManager.hideAllComponents();
				}
				for (int j = 0; j < pages.size(); ++j)
				{
					pages[j]->setVisible(j == selectedTab);
					for (auto c : pages[j]->getChildren())
					{
						c->setVisible(j == selectedTab);
					}
				}

			};
		}
		if (!buttons.isEmpty())
		{
			buttons.getFirst()->triggerClick();
		}
		repaint();
	}

	void TabsComponent::setHasCutout(bool shouldHaveCutout, float ratio)
	{
		cutoutRatio = ratio;


		hasCutout = shouldHaveCutout;
		if (!getBounds().isEmpty())
		{
			resized();
		}


	}

	juce::Rectangle<int> TabsComponent::getCutout() const
	{
		return hasCutout ? cutoutRect : juce::Rectangle<int>();
	}

	void TabsComponent::resized()
	{
		juce::Rectangle<int> pageBounds;
		if (isVertical)
		{
			tabBar = getLocalBounds().withWidth(getWidth() * tabRatio);

			cutoutRect = juce::Rectangle<int>(tabBar.getWidth(), cutoutRatio * tabBar.getWidth()).withCentre(tabBar.getCentre());

			pageBounds = getLocalBounds().withLeft(tabBar.getRight());
		}
		else
		{
			tabBar = getLocalBounds().withHeight(getHeight() * tabRatio);

			cutoutRect = juce::Rectangle<int>(cutoutRatio * tabBar.getHeight(), tabBar.getHeight()).withCentre(tabBar.getCentre());

			pageBounds = getLocalBounds().withTop(tabBar.getBottom());
		}



		auto frac = 1.0 / buttons.size();



		auto buttonFont = universals->Fonts[buttons.getFirst()->getFontIndex()];
		auto maxText = buttonFont.getWidestString(tabNames);
		auto stringWRatio = buttonFont.getFont().withHeight(1).getStringWidthFloat(maxText);

		auto w = isVertical ? tabBar.getWidth() : (tabBar.getWidth() - (hasCutout ? cutoutRect.getWidth() : 0)) * frac;
		auto h = isVertical ? (tabBar.getHeight() - (hasCutout ? cutoutRect.getHeight() : 0)) * frac : tabBar.getHeight();
		auto ratio = 0.75 * w / (stringWRatio * h);

		for (int i = 0; i < buttons.size(); ++i)
		{
			if (scaleTitles)
			{
				buttons[i]->setTextHeightRatio(ratio);
			}



			if (isVertical)
			{
                buttons[i]->setBounds(juce::Rectangle<int>(0, i * h, w, h).translated(tabBar.getX(), tabBar.getY() + (hasCutout ? (2 * i / buttons.size() > 0 ? cutoutRect.getHeight() : 0) : 0)));
			}
			else
			{
                buttons[i]->setBounds(juce::Rectangle<int>(i * w, 0, w, h).translated(tabBar.getX() + (hasCutout ? (2 * i / buttons.size() > 0 ? cutoutRect.getWidth() : 0) : 0), tabBar.getY()));
			}


			pages[i]->setBounds(pageBounds);// .reduced(universals->roundedRectangleCurve)));
		}

	}

	void TabsComponent::paint(juce::Graphics& g)
	{
		//int idx = getSelectedTab();

		//g.setColour(getColor(bkgd));
		//for (int i = 0; i < buttons.size() - 1; ++i)
		//{
		//	g.drawVerticalLine(tabBar.getRelativePoint(1.0 / buttons.size() * (i + 1), 0.0).x, tabBar.getRelativePoint(0.0, 0.15).y, tabBar.getRelativePoint(0.0, 0.85).y);
		//}

		//g.setColour(buttons[idx]->getColor(buttons[idx]->textDown));
		//g.drawHorizontalLine(buttons[idx]->getBottom(), buttons[idx]->getBounds().getRelativePoint(0.2, 0.0).x, buttons[idx]->getBounds().getRelativePoint(0.8, 0.0).x);


		g.setColour(getColor(bkgd));
		if (isVertical)
		{
			g.fillPath(getRoundedRectangleFromCurveBools(getLocalBounds().toFloat().withLeft(tabBar.getRight()), CornerCurves(corners & CornerCurves(CornerCurves::topRight | CornerCurves::bottomRight)), universals->roundedRectangleCurve));
		}
		else
		{
			g.fillPath(getRoundedRectangleFromCurveBools(getLocalBounds().toFloat().withTop(tabBar.getBottom()), CornerCurves(corners & CornerCurves(CornerCurves::bottomLeft | CornerCurves::bottomRight)), universals->roundedRectangleCurve));
		}


		for (int i = 0; i < buttons.size(); ++i)
		{

			CornerCurves buttonCurves;
			if (i == 0)
			{
				buttonCurves = CornerCurves(CornerCurves::topLeft & corners);
			}
			else if (i == buttons.size() - 1)
			{
				if (isVertical)
				{
					buttonCurves = CornerCurves(CornerCurves::bottomLeft & corners);
				}
				else
				{
					buttonCurves = CornerCurves(CornerCurves::topRight & corners);
				}
			}
			else
			{
				buttonCurves = CornerCurves(0);
			}

			g.setColour(getColor(buttons[i]->getToggleState() ? bkgd : bkgd.withMultipliedAlpha(universals->disabledAlpha)));

			g.fillPath(getRoundedRectangleFromCurveBools(buttons[i]->getBounds().toFloat(), buttonCurves, universals->roundedRectangleCurve));
		}

	}






	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& buttonColor)
	{
		setColors(bkgdColor, buttonColor.withMultipliedAlpha(universals->disabledAlpha), buttonColor);
	}

	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& textUp, const NamedColorsIdentifier& textDown)
	{
		bkgd = bkgdColor;


		for (auto b : buttons)
		{

			b->textUp = textUp;
			b->textDown = textDown;

			b->backgroundDown = NamedColorsIdentifier();
			b->backgroundUp = NamedColorsIdentifier();
		}
	}


	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const juce::Array<NamedColorsIdentifier>& buttonColors)
	{

		jassert(buttonColors.size() == buttons.size());


		bkgd = bkgdColor;

		for (int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->textUp = buttonColors[i].withMultipliedAlpha(universals->disabledAlpha);
			buttons[i]->textDown = buttonColors[i];

			buttons[i]->backgroundDown = NamedColorsIdentifier();
			buttons[i]->backgroundUp = NamedColorsIdentifier();
		}
	}

	TextButton* TabsComponent::getButton(int idx)
	{
		return buttons[idx];
	}

	juce::Rectangle<int> TabsComponent::getUsableArea()
	{
		return pages.getFirst()->getBounds();
	}

	juce::Rectangle<int> TabsComponent::getTabArea()
	{
		return tabBar;
	}

	juce::Rectangle<int> TabsComponent::getTabArea(int idx)
	{
		return buttons[idx]->getBounds();
	}

	juce::Component* TabsComponent::getPage(int idx)
	{
		return pages[idx];
	}



	void TabsComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{
		//if (tabBar.contains(e.position))
		//{
		//	mw.isReversed ? selectedTab -= signum(mw.deltaY) : selectedTab += signum(mw.deltaY);
		//	selectedTab = juce::jlimit(0, buttons.size() - 1, selectedTab);
		//	buttons[selectedTab]->triggerClick();
		//}

	}

	int TabsComponent::getSelectedTab() const
	{
		return selectedTab;
	}

	void TabsComponent::selectTab(int idx)
	{
		buttons[idx]->triggerClick();
	}


	void TabsComponent::notifyTabSelection(int idx)
	{
		selectedTab = idx;
		for (auto l : listeners)
		{
			l->tabSelected(this);
		}

		if (onTabChange)
		{
			onTabChange(idx);
		}
		getParentComponent()->repaint();
	}



	void TabsComponent::addListener(Listener* listenerToAdd)
	{
		listeners.addIfNotAlreadyThere(listenerToAdd);
	}

	void TabsComponent::removeListener(Listener* listenerToRemove)
	{
		listeners.removeAllInstancesOf(listenerToRemove);
	}

	bool TabsComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
	{
		auto cast = dynamic_cast<LinkableControlComponent::DragComp*>(dragSourceDetails.sourceComponent.get());


		return cast != nullptr && isEnabled();
	}

	void TabsComponent::itemDragEnter(const SourceDetails& dragSourceDetails)
	{
		itemDragMove(dragSourceDetails);
	}

	void TabsComponent::itemDragMove(const SourceDetails& dragSourceDetails)
	{
		for (int i = 0; i < buttons.size(); ++i)
		{
			if (buttons[i]->contains(buttons[i]->getLocalPoint(this, dragSourceDetails.localPosition)))
			{
				buttons[i]->setToggleState(true, juce::sendNotification);
				buttons[i]->onClick();
				break;
			}
		}
	}

	void TabsComponent::itemDropped(const SourceDetails& dragSourceDetails)
	{
	}




} // namespace bdsp
