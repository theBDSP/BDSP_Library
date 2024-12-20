#include "bdsp_Component.h"

namespace bdsp
{

	HoverableComponent::HoverableComponent(juce::Component* compToExtend)
		:t(this)
	{
		extendedComponent = compToExtend;
		extendedComponent->addMouseListener(&t, true); // adds the hover timer as mouse listener of the managed juce component
	}

	HoverableComponent::~HoverableComponent()
	{
		removeAllHoverPartners(); // we don't want other components notifying this of hovering state changes if this component has been deleted
	}

	juce::Component* HoverableComponent::getComponent()
	{
		return extendedComponent;
	}

	void HoverableComponent::setHoverTime(int ms)
	{
		t.setTime(ms);
	}

	void HoverableComponent::setHoverFunc(std::function<void(bool)>& func)
	{
		t.hoverFunc = func;
	}

	std::function<void(bool)>& HoverableComponent::getHoverFunc()
	{
		return t.hoverFunc;
	}


	int HoverableComponent::getHoverTime()
	{
		return t.getTime();
	}

	bool HoverableComponent::isHovering()
	{
		return t.isHovering() && extendedComponent->isEnabled();
	}

	void HoverableComponent::setHovering(bool state, bool notifyListeners, bool notifyPartners, HoverableComponent* excludedPartner)
	{
		t.setHovering(state, notifyListeners, notifyPartners, excludedPartner);
		extendedComponent->repaint();
	}



	bool HoverableComponent::isHoverPartner(HoverableComponent* compToCheck)
	{
		return hoverPartners.contains(compToCheck);
	}




	void HoverableComponent::addHoverListener(Listener* listenerToAdd)
	{
		listeners.addIfNotAlreadyThere(listenerToAdd);
	}

	void HoverableComponent::removeHoverListener(Listener* listenerToRemove)
	{
		listeners.removeAllInstancesOf(listenerToRemove);
	}

	void HoverableComponent::removeAllHoverListeners()
	{
		listeners.clear();
	}

	void HoverableComponent::addHoverPartner(HoverableComponent* otherComp, bool initialAdd)
	{
		jassert(otherComp != this); // Adding a component to its own list of hover partners is not only pointless but can get you stuck in an infinite loop of notofying itself
		hoverPartners.addIfNotAlreadyThere(otherComp);

		if (initialAdd)
		{
			otherComp->addHoverPartner(this, false);
		}
	}

	void HoverableComponent::addHoverPartners(const juce::Array<juce::WeakReference<HoverableComponent>>& otherComps, bool initialAdd)
	{
		for (auto& p : otherComps)
		{
			jassert(p.get() != this); // Adding a component to its own list of hover partners is not only pointless but can get you stuck in an infinite loop of notofying itself


			if (p.wasObjectDeleted()) // remove this partner if was deleted, just in case it was already added as a hover partner previously
			{
				removeHoverPartner(p);
			}
			else
			{
				hoverPartners.addIfNotAlreadyThere(p);
			}
		}
		if (initialAdd)
		{

			for (auto& p : otherComps) // for each comp added as a hover partner, add the other partners in the set and this component as hover partners
			{
				if (!p.wasObjectDeleted())
				{
					auto arr = otherComps;
					arr.removeAllInstancesOf(p);
					arr.add(this);
					p->addHoverPartners(arr, false);
				}
			}
		}
	}

	void HoverableComponent::removeHoverPartner(HoverableComponent* otherComp, bool removeFromAllOtherPartners)
	{
		hoverPartners.removeAllInstancesOf(otherComp);

		if (removeFromAllOtherPartners)
		{
			for (auto& p : hoverPartners)
			{
				p->removeHoverPartner(otherComp, false);
			}
		}
	}

	void HoverableComponent::removeAllHoverPartners()
	{
		for (auto& p : hoverPartners)
		{
			if (!p.wasObjectDeleted())
			{
				p->hoverPartners.removeAllInstancesOf(this);
			}
		}
		hoverPartners.clear();
	}


	//================================================================================================================================================================================================

	HoverableComponent::HoverTimer::HoverTimer(HoverableComponent* parent)
	{
		p = parent;
	}

	HoverableComponent::HoverTimer::~HoverTimer()
	{
		stopTimer();
	}




	void HoverableComponent::HoverTimer::timerCallback()
	{
		if (hoverFunc.operator bool())
		{
			hoverFunc(hovering);
		}

		stopTimer();
	}


	void HoverableComponent::HoverTimer::setTime(int ms)
	{
		onDelayMS = ms;
	}


	int HoverableComponent::HoverTimer::getTime()
	{
		return onDelayMS;
	}

	bool HoverableComponent::HoverTimer::isHovering()
	{
		return hovering;
	}

	void HoverableComponent::HoverTimer::setHovering(bool state, bool notifyListeners, bool notifyPartners, HoverableComponent* excludedPartner)
	{
		if (p->extendedComponent->isEnabled() && p->extendedComponent->isShowing()) // component should only change hovering state if it is enabled and on screen
		{
			hovering = state;
			p->extendedComponent->repaint();
			if (state)
			{
				startTimer(onDelayMS);
			}
			else
			{
				startTimer(offDelayMS);
			}
			if (notifyListeners)
			{
				notifyParentsListeners();
			}
			if (notifyPartners)
			{
				notifyParentsPartners(excludedPartner);
			}
		}
	}

	void HoverableComponent::HoverTimer::notifyParentsListeners()
	{
		for (auto& l : p->listeners)
		{
			if (l.wasObjectDeleted())
			{
				p->removeHoverListener(l);
			}
			else
			{
				if (l->onHoveringStateChanged.operator bool())
				{
					l->onHoveringStateChanged(p, hovering);
				}
			}
		}
	}

	void HoverableComponent::HoverTimer::notifyParentsPartners(HoverableComponent* excludedPartner)
	{
		for (auto& h : p->hoverPartners)
		{
			if (h.wasObjectDeleted())
			{
				p->removeHoverPartner(h);
			}
			else
			{
				if (h != excludedPartner)
				{
					h->setHovering(hovering, false, true, p);
				}
			}
		}
	}


	void HoverableComponent::HoverTimer::mouseEnter(const juce::MouseEvent& e)
	{
		setHovering(true);
	}


	void HoverableComponent::HoverTimer::mouseExit(const juce::MouseEvent& e)
	{
		setHovering(false);
	}


	//================================================================================================================================================================================================


	ComponentCore::ComponentCore(juce::Component* componentToExtend, GUI_Universals* universalsToUse)
		:HoverableComponent(componentToExtend),
		universals(universalsToUse)
	{
		extendedComponent->setLookAndFeel(&universals->lnf);
	}

	ComponentCore::~ComponentCore()
	{
	}





	juce::Rectangle<int> ComponentCore::getBoundsRelativeToDesktopManager()
	{
		return universals->desktopManager.getLocalArea(extendedComponent, extendedComponent->getLocalBounds());
	}

	void ComponentCore::forceAspectRatio(double aspectRatio, juce::RectanglePlacement position)
	{
		extendedComponent->setBounds(confineToAspectRatio(extendedComponent->getBounds(), aspectRatio, position));
	}



	void ComponentCore::repaintThreadChecked()
	{
		universals->repaintChecker->callRepaint(juce::WeakReference<juce::Component>(extendedComponent));
	}









	void ComponentCore::setHintBarText(const juce::String& text)
	{
		const juce::MessageManagerLock mml;
		extendedComponent->removeMouseListener(hintListener.get());// remove the old hint listener as a mouse listener before it gets deleted
		hintListener.reset(new HintBar::Listener(universals->hintBar.get(), text));

		extendedComponent->addMouseListener(hintListener.get(), true);

		if (extendedComponent->isMouseOver())// if this is being done while the mouse is over the extended component, we should show the new text in the hint bar
		{
			universals->hintBar->setText(hintListener->getText());
		}


	}

	const juce::AttributedString& ComponentCore::getHintText()
	{
		return hintListener->getText();
	}

	void ComponentCore::setFontIndex(int idx)
	{
		fontIndex = idx;
	}

	int ComponentCore::getFontIndex()
	{
		return fontIndex;
	}

	juce::Colour ComponentCore::getColor(const NamedColorsIdentifier& name)
	{
		return universals->colors.getColor(name);
	}

	juce::Font ComponentCore::getFont(int index)
	{
		if (index < 0)
		{
			index = fontIndex;
		}

		return universals->Fonts[index].getFont();
	}


} // namespace bdsp