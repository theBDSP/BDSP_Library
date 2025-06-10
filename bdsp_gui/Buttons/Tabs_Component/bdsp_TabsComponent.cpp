#include "bdsp_TabsComponent.h"
namespace bdsp
{

	TabsComponent::TabsComponent(GUI_Universals* universalsToUse, const juce::StringArray& tabNames)
		:Component(universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		;
		setNames(tabNames);

		setColors(BDSP_COLOR_DARK, BDSP_COLOR_COLOR, NamedColorsIdentifier());


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

		auto w = isVertical ? tabBar.getWidth() : (tabBar.getWidth() - (hasCutout ? cutoutRect.getWidth() : 0)) * frac;
		auto h = isVertical ? (tabBar.getHeight() - (hasCutout ? cutoutRect.getHeight() : 0)) * frac : tabBar.getHeight();
		auto f = resizeFontToFit(buttonFont.getFont(), w, 0.75 * h, maxText);
		auto ratio = f.getHeight() / h;

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

		float outlineW = universals->tabBorderSize;

		juce::Path p;
		if (isVertical)
		{
			p.addPath(getRoundedRectangleFromCurveBools(juce::Rectangle<float>().leftTopRightBottom(buttons.getFirst()->getRight(), buttons.getFirst()->getY() + outlineW, getWidth() - outlineW, buttons.getLast()->getBottom() - outlineW), corners, universals->roundedRectangleCurve - outlineW));
		}
		else
		{
			p.addPath(getRoundedRectangleFromCurveBools(juce::Rectangle<float>().leftTopRightBottom(buttons.getFirst()->getX() + outlineW, buttons.getFirst()->getBottom(), buttons.getLast()->getRight() - outlineW, getHeight() - outlineW), corners, universals->roundedRectangleCurve - outlineW));
		}

		for (int i = 0; i < buttons.size(); ++i)
		{

			CornerCurves buttonCurves = CornerCurves::all;


			if (buttons[i]->getToggleState())
			{
				auto buttonBounds = buttons[i]->getBounds().toFloat();


				if (isVertical || i == 0)
				{
					buttonBounds = buttonBounds.withTrimmedLeft(outlineW);
				}

				if (!isVertical || i == 0)
				{
					buttonBounds = buttonBounds.withTrimmedTop(outlineW);
				}

				if (isVertical && i == buttons.size() - 1)
				{
					buttonBounds = buttonBounds.withTrimmedBottom(outlineW);
				}

				if (!isVertical && i == buttons.size() - 1)
				{
					buttonBounds = buttonBounds.withTrimmedRight(outlineW);
				}

				if (isVertical)
				{
					buttonBounds = buttonBounds.withTrimmedRight(-2*universals->roundedRectangleCurve);
				}
				else
				{
					buttonBounds = buttonBounds.withTrimmedBottom(-2*universals->roundedRectangleCurve);
				}

				auto buttonPath = getRoundedRectangleFromCurveBools(buttonBounds, buttonCurves, universals->roundedRectangleCurve - outlineW);
				p = ClipperLib::performBoolean(p, buttonPath, ClipperLib::ctUnion);
			}


		}

		g.setColour(getColor(outline));
		g.fillPath(getRoundedRectangleFromCurveBools(getLocalBounds().toFloat(), corners, universals->roundedRectangleCurve));

		g.setColour(getColor(bkgd));
		g.fillPath(p);
	} 






	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& buttonColor, const NamedColorsIdentifier& outlineColor)
	{
		setColors(bkgdColor, buttonColor.withMultipliedAlpha(universals->disabledAlpha), buttonColor, outlineColor);
	}

	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const NamedColorsIdentifier& textUp, const NamedColorsIdentifier& textDown, const NamedColorsIdentifier& outlineColor)
	{
		bkgd = bkgdColor;
		outline = outlineColor;


		for (auto b : buttons)
		{

			b->textUp = textUp;
			b->textDown = textDown;

			b->backgroundDown = NamedColorsIdentifier();
			b->backgroundUp = NamedColorsIdentifier();
		}
		GUI_UniversalsChanged();
	}


	void TabsComponent::setColors(const NamedColorsIdentifier& bkgdColor, const juce::Array<NamedColorsIdentifier>& buttonColors, const NamedColorsIdentifier& outlineColor)
	{

		jassert(buttonColors.size() == buttons.size());


		bkgd = bkgdColor;
		outline = outlineColor;

		for (int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->textUp = buttonColors[i].withMultipliedAlpha(universals->disabledAlpha);
			buttons[i]->textDown = buttonColors[i];

			buttons[i]->backgroundDown = NamedColorsIdentifier();
			buttons[i]->backgroundUp = NamedColorsIdentifier();
		}

		GUI_UniversalsChanged();
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
				dragTimer.start(buttons[i]);
				break;
			}
		}
	}

	void TabsComponent::itemDropped(const SourceDetails& dragSourceDetails)
	{
	}

	void TabsComponent::GUI_UniversalsChanged()
	{
		repaint();
	}





} // namespace bdsp
