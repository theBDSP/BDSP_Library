#include "bdsp_RearrangeableTabsComponent.h"
namespace bdsp
{
	RearrangeableTabsComponent::RearrangeableTabsComponent(GUI_Universals* universalsToUse, const juce::StringArray& tabNames)
		:TabsComponent(universalsToUse, tabNames)
	{
		rearrangeComp = std::make_unique<RearrangableComponentManagerBase>(universals);
		//addAndMakeVisible(rearrangeComp.get());
		setNames(tabNames);
	}
	void RearrangeableTabsComponent::setVertical(bool shouldBeVertical)
	{
		TabsComponent::setVertical(shouldBeVertical);
		for (auto b : buttons)
		{
			b->setJustification(isVertical ? juce::Justification::centredLeft : juce::Justification::centredTop);
		}
	}
	void RearrangeableTabsComponent::setNames(const juce::StringArray& newNames)
	{
		TabsComponent::setNames(newNames);

		for (int i = 0; i < buttons.size(); ++i)
		{
			buttons[i]->setJustification(isVertical ? juce::Justification::centredLeft : juce::Justification::centredTop);
			buttons[i]->setInterceptsMouseClicks(true, true);
		}
	}
	void RearrangeableTabsComponent::setNumCompsPerTab(int num)
	{
		numPerTab = num;
		int j = 0;
		for (int t = 0; t < buttons.size(); t++)
		{
			for (int i = 0; i < num; ++i)
			{
				buttons[t]->addAndMakeVisible(rearrangeComp->draggerSlots[++j]);
			}
		}
	}
	void RearrangeableTabsComponent::resized()
	{
		TabsComponent::resized();
		auto dragBounds = getRelativeRearrangeBounds().toFloat();
		rearrangeComp->setBounds(getLocalBounds());
		juce::Rectangle<float> dragHandleBoundsZeroOrigin;
		float offset;
		if (isVertical)
		{
			dragHandleBoundsZeroOrigin = dragBounds.getProportion(juce::Rectangle<float>(0, 0, 1, 1.0 / numPerTab));
			offset = dragHandleBoundsZeroOrigin.getHeight();
		}
		else
		{
			dragHandleBoundsZeroOrigin = dragBounds.getProportion(juce::Rectangle<float>(0, 0, 1.0 / numPerTab, 1));
			offset = dragHandleBoundsZeroOrigin.getWidth();
		}

		for (int i = 0; i < rearrangeComp->draggerSlots.size(); ++i)
		{
			int t = i / numPerTab;
			int j = i % numPerTab;
			if (isVertical)
			{
				rearrangeComp->draggerSlots[i]->setBounds(shrinkRectangleToInt(dragHandleBoundsZeroOrigin.translated(0, buttons[t]->getY() + j * offset)).reduced(universals->rectThicc));
			}
			else
			{
				rearrangeComp->draggerSlots[i]->setBounds(shrinkRectangleToInt(dragHandleBoundsZeroOrigin.translated(buttons[t]->getX() + j * offset, 0)).reduced(universals->rectThicc));
			}
			rearrangeComp->draggers[rearrangeComp->findDraggerIndex(i)]->setBounds(rearrangeComp->draggerSlots[i]->getBounds());
		}
		rearrangeComp->dragHandleBounds = juce::Rectangle<int>(rearrangeComp->draggerSlots.getFirst()->getPosition(), rearrangeComp->draggerSlots.getLast()->getBounds().getBottomRight()).toFloat();
	}
	//void RearrangeableTabsComponent::paint(juce::Graphics& g)
	//{
	//	g.setColour(getColor(bkgd));
	//	g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
	//}

	void RearrangeableTabsComponent::paintOverChildren(juce::Graphics& g)
	{
	}


	juce::Rectangle<int> RearrangeableTabsComponent::getRelativeRearrangeBounds()
	{
		auto ratio = buttons.getFirst()->getTextHeightRatio();
		auto b = buttons.getFirst()->getLocalBounds();
		if (isVertical)
		{
			return b.getProportion(juce::Rectangle<float>(ratio, 0, 1 - ratio, 1));
		}
		else
		{
			return b.getProportion(juce::Rectangle<float>(0, ratio, 1, 1 - ratio));
		}
	}
	juce::Rectangle<int> RearrangeableTabsComponent::getRelativeTabTitleBounds()
	{
		auto ratio = buttons.getFirst()->getTextHeightRatio();
		auto b = buttons.getFirst()->getLocalBounds();

		if (isVertical)
		{
			return b.getProportion(juce::Rectangle<float>(0, 0, ratio, 1));
		}
		else
		{
			return b.getProportion(juce::Rectangle<float>(0, 0, 1, ratio));
		}
	}
	void RearrangeableTabsComponent::addComponent(juce::Component* newComp, int page)
	{
		rearrangeComp->addComponent(newComp);

		pages[page]->addAndMakeVisible(rearrangeComp->comps.getLast());

		addAndMakeVisible(rearrangeComp->draggerSlots.getLast());
		addAndMakeVisible(rearrangeComp->draggers.getLast());

	}
	void RearrangeableTabsComponent::setTabTitleHeightRatio(float newRatio)
	{
		for (auto b : buttons)
		{
			b->setTextHeightRatio(newRatio);
		}
	}

	RearrangableComponentManagerBase* RearrangeableTabsComponent::getRearrangeComp()
	{
		return rearrangeComp.get();
	}


} // namespace bdsp
