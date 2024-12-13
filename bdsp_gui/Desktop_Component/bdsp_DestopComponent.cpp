#include "bdsp_DestopComponent.h"

namespace bdsp
{
	DesktopComponent::DesktopComponent(GUI_Universals* universalsToUse, bool shouldBeDraggable, bool needsToBeOriginalComp)
		:Component(universalsToUse)
	{
		manager = &universals->desktopManager;
		manager->addComponent(this);
		setWantsKeyboardFocus(true);
		setMouseClickGrabsKeyboardFocus(false);

		setDraggable(shouldBeDraggable, needsToBeOriginalComp);


		setAlwaysOnTop(true); // ensure each desktop comp is above the reset of the GUI

	}





	DesktopComponent::~DesktopComponent()
	{
		manager->removeComponent(this);
	}

	void DesktopComponent::show()
	{
		if (closesOthersOnOpen)
		{
			manager->hideAllComponents();
		}

		setVisible(true);
		grabKeyboardFocus();

		if (onShow.operator bool())
		{
			onShow();
		}
	}

	void DesktopComponent::hide()
	{
		giveAwayKeyboardFocus();
		setVisible(false);

		if (onHide.operator bool())
		{
			onHide();
		}
	}






	bool DesktopComponent::isMouseInside(const juce::MouseEvent& e)
	{
		return reallyContains(e.getEventRelativeTo(this).position.toInt(), true);
	}

	void DesktopComponent::focusLost(FocusChangeType change)
	{
		if (lofCloses)
		{

			if (change == focusChangedDirectly)
			{
				//check whether comp grabbing focus is a child of this comp
					//if so need to reamin open to kep in focus comp visible
				auto currentFocusedComp = getCurrentlyFocusedComponent();
				if (currentFocusedComp == nullptr)
				{
					hide();
				}
				else if (lofExclusion.contains(currentFocusedComp))
				{
					return;
				}
				else
				{

					juce::Component* nextParent = currentFocusedComp->getParentComponent();
					while (nextParent != nullptr && nextParent != this) // climbs top hierarchy until this component is reached or is at top level component
					{
						nextParent = nextParent->getParentComponent();
					}

					if (nextParent == nullptr) // if focused comp is not a child
					{
						hide();
					}
				}
			}
			else if (change == focusChangedByMouseClick && !lofExclusion.contains(getTopLevelComponent()->getComponentAt(getTopLevelComponent()->getMouseXYRelative())))
			{
				hide();
			}
			else if (juce::Desktop::getInstance().findComponentAt(juce::Desktop::getInstance().getMousePosition()) == nullptr) // essentially if the user clicks outside the window
			{
				hide();
			}
		}

	}

	void DesktopComponent::setDarken(bool state)
	{
		darken = state;
		repaint();
	}

	void DesktopComponent::paintOverChildren(juce::Graphics& g)
	{
		if (darken)
		{
			g.setColour(getColor(BDSP_COLOR_PURE_BLACK).withAlpha(BDSP_OVERLAY_ALPHA));
			g.fillAll();
		}

	}



	void DesktopComponent::setLossOfFocusClosesWindow(bool b, const juce::Array<juce::Component*>& excludedComps)
	{
		lofCloses = b;
		for (auto c : excludedComps)
		{
			lofExclusion.addIfNotAlreadyThere(c);
		}
	}

	bool DesktopComponent::lossOfFocusClosesWindow()
	{
		return lofCloses;
	}

	void DesktopComponent::setDraggable(bool shouldBeDraggable, bool needsToBeOriginalComp)
	{
		needsToBeOriginalCompToDrag = needsToBeOriginalComp;
		if (shouldBeDraggable && !dragListener.operator bool())
		{
			dragListener = std::make_unique<DragListener>(this);
		}
		else
		{
			dragListener.reset();
		}
	}

	void DesktopComponent::setClosesOthersOnShow(bool closeOthersOnShow)
	{
		closesOthersOnOpen = closeOthersOnShow;
	}






}// namnepace bdsp
//==============================================================================
