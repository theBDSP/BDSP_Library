#include "bdsp_RearrangeableComponent.h"
namespace bdsp
{
	RearrangableComponentManagerBase::RearrangableComponentManagerBase(GUI_Universals* universalsToUse)
		:Component(universalsToUse),
		dragBoundsConstrainer(this)
	{
	}

	void RearrangableComponentManagerBase::setDragHandleText(const juce::String& s)
	{
		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragText = s;

			draggers[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandlePath(const juce::Path& p)
	{
		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragHandlePath.clear();
			draggers[i]->dragHandlePath.addPath(p);

			draggers[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandleColors(const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor)
	{

		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragColor = normalColor;
			draggers[i]->dragColorDown = downColor;

			draggerSlots[i]->setColors(slotColor);

			draggers[i]->repaint();
			draggerSlots[i]->repaint();

		}
	}

	void RearrangableComponentManagerBase::setSingleDragHandleText(int idx, const juce::String& s)
	{
		draggers[idx]->dragText = s;

		draggers[idx]->repaint();
	}

	void RearrangableComponentManagerBase::setSingleDragHandlePath(int idx, const juce::Path& p)
	{
		draggers[idx]->dragHandlePath.clear();
		draggers[idx]->dragHandlePath.addPath(p);

		draggers[idx]->repaint();
	}

	void RearrangableComponentManagerBase::setSingleDragHandleColors(int idx, const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor)
	{

		draggers[idx]->dragColor = normalColor;
		draggers[idx]->dragColorDown = downColor;

		draggerSlots[idx]->setColors(slotColor);

		draggers[idx]->repaint();
		draggerSlots[idx]->repaint();
	}

	void RearrangableComponentManagerBase::setDragHandleTexts(juce::StringArray strings)
	{
		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragText = strings[i];

			draggers[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandlePaths(juce::Array<juce::Path> paths)
	{
		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragHandlePath.clear();
			draggers[i]->dragHandlePath.addPath(paths[i]);

			draggers[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandleColors(juce::Array<NamedColorsIdentifier> normalColors, juce::Array<NamedColorsIdentifier> downColors, juce::Array<NamedColorsIdentifier> slotColors)
	{

		for (int i = 0; i < draggers.size(); ++i)
		{
			draggers[i]->dragColor = normalColors[i];
			draggers[i]->dragColorDown = downColors[i];

			draggerSlots[i]->setColors(slotColors[i]);

			draggers[i]->repaint();
			draggerSlots[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::addComponent(juce::Component* newComp)
	{
		comps.add(newComp);
		addAndMakeVisible(comps.getLast());



		draggerSlots.add(new BasicContainerComponent(universals, NamedColorsIdentifier()));
		addAndMakeVisible(draggerSlots.getLast(), 0);

		draggers.add(new Dragger(this, comps.size() - 1));
		addAndMakeVisible(draggers.getLast());

	}

	void RearrangableComponentManagerBase::reorderComponents(int indexMoved, int indexMovedTo)
	{
		int i1 = findDraggerIndex(indexMoved);
		int i2 = findDraggerIndex(indexMovedTo);

		draggers[i1]->setIndex(indexMovedTo);
		draggers[i2]->setIndex(indexMoved);

		draggers[i1]->toFront(false);
		draggers[i2]->toFront(false);

		if (onComponentOrderChanged.operator bool())
		{
			onComponentOrderChanged(indexMoved, indexMovedTo);
		}
	}

	void RearrangableComponentManagerBase::previewNewOrder(int indexMoved, int indexMovedTo)
	{
		int i1 = findDraggerIndex(indexMoved);
		int i2 = findDraggerIndex(indexMovedTo);

		draggers[i1]->toFront(false);
		draggers[i2]->toFront(false);


		if (!draggers[i1]->isDragging)
		{
			draggers[i1]->setBounds(draggerSlots[indexMovedTo]->getBounds());
		}
		if (!draggers[i2]->isDragging)
		{
			draggers[i2]->setBounds(draggerSlots[indexMoved]->getBounds());
		}

		for (int i = 0; i < draggers.size(); ++i)
		{


			if (i != i1 && i != i2)
			{
				draggers[i]->setBounds(draggerSlots[baselineDragOrder[i]]->getBounds());
			}
		}

	}


	juce::Component* RearrangableComponentManagerBase::getComp(int idx) const
	{
		return comps.getUnchecked(idx);
	}

	int RearrangableComponentManagerBase::getNumComps() const
	{
		return comps.size();
	}

	int RearrangableComponentManagerBase::findDraggerIndex(int compIndex) const
	{
		int out = -1;
		for (int i = 0; i < draggers.size(); ++i)
		{
			if (draggers[i]->getIndex() == compIndex)
			{
				out = i;
				break;
			}
		}
		return out;
	}


	//================================================================================================================================================================================================

	RearrangableComponentManagerBase::Dragger::Dragger(RearrangableComponentManagerBase* parent, int index)
		:Component(parent->universals)
	{
		p = parent;
		idx = index;
		toFront(false);

	}

	void RearrangableComponentManagerBase::Dragger::mouseEnter(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::DraggingHandCursor);
	}

	void RearrangableComponentManagerBase::Dragger::mouseExit(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	void RearrangableComponentManagerBase::Dragger::mouseDown(const juce::MouseEvent& e)
	{
		toFront(false);
		startDraggingComponent(this, e);
		isDragging = true;

		for (int i = 0; i < p->draggers.size(); ++i)
		{
			p->baselineDragOrder.set(i, p->draggers[i]->getIndex());
		}


		repaint();
	}
	void RearrangableComponentManagerBase::Dragger::mouseDrag(const juce::MouseEvent& e)
	{

		dragComponent(this, e, &p->dragBoundsConstrainer);

		for (int i = 0; i < p->draggerSlots.size(); ++i)
		{
			if ((i != idx || !p->instantSwap) && p->draggerSlots[i]->isShowing())
			{
				int l, r, t, b;
				l = p->draggerSlots[i]->getX();
				r = p->draggerSlots[i]->getRight();
				t = p->draggerSlots[i]->getY();
				b = p->draggerSlots[i]->getBottom();



				if ((p->horizontal && isBetweenInclusive(getBounds().getCentreX(), l, r)) || (!p->horizontal && isBetweenInclusive(getBounds().getCentreY(), t, b)))
				{
					if (p->instantSwap)
					{
						p->reorderComponents(idx, i);
					}
					else
					{
						p->previewNewOrder(idx, i);
					}

					break;
				}
			}

		}

		if (p->onDrag.operator bool())
		{
			p->onDrag(idx, e.getEventRelativeTo(p).getPosition());
		}


	}
	void RearrangableComponentManagerBase::Dragger::mouseUp(const juce::MouseEvent& e)
	{
		isDragging = false;

		if (!p->instantSwap)
		{
			for (int i = 0; i < p->draggerSlots.size(); ++i)
			{
				if (p->draggerSlots[i]->getBounds().contains(getBounds().getCentre()))
				{
					p->reorderComponents(idx, i);
				}		

			}
		}


		setBounds(p->draggerSlots[idx]->getBounds());
		repaint();
	}
	void RearrangableComponentManagerBase::Dragger::paint(juce::Graphics& g)
	{
		auto color = getHoverColor(getColor(dragColorDown), getColor(dragColor), isDragging, isMouseOver(), universals->hoverMixAmt);

		g.setColour(color);

		if (!dragHandlePath.isEmpty())
		{
			scaleToFit(dragHandlePath, getLocalBounds());

			g.fillPath(dragHandlePath);
		}
		if (dragText.isNotEmpty())
		{
			drawText(g, universals->Fonts[getFontIndex()].getFont().withHeight(getHeight() * 0.9), dragText, getLocalBounds());
		}

	}
	void RearrangableComponentManagerBase::Dragger::resized()
	{
	}
	void RearrangableComponentManagerBase::Dragger::setIndex(int newIndex)
	{
		idx = newIndex;
		//repaint();
	}

	int RearrangableComponentManagerBase::Dragger::getIndex()
	{
		return idx;
	}



	//================================================================================================================================================================================================



	StationaryRearrangableComponentManager::StationaryRearrangableComponentManager(GUI_Universals* universalsToUse)
		:RearrangableComponentManagerBase(universalsToUse)
	{
		addAndMakeVisible(vp);
		vp.setViewedComponent(&vpComp);

		vp.setScrollBarsShown(!horizontal, horizontal, !horizontal, horizontal);

		onDrag = [=](int idx, juce::Point<int> p)
		{
			auto pos = p + getPosition() - vp.getViewPosition();
			bool edge = vp.autoScroll(pos.x, pos.y, horizontal ? comps[0]->getX() : comps[0]->getY(), (horizontal ? comps[0]->getWidth() : comps[0]->getHeight()) / 6);

			beginDragAutoRepeat(edge ? 100 : 0);
		};
	}


	StationaryRearrangableComponentManager::~StationaryRearrangableComponentManager()
	{
	}

	void StationaryRearrangableComponentManager::resized()
	{
		vp.setBounds(getLocalBounds());

		int num = comps.size();

		auto w = horizontal ? (1.0 - (num - 1) * borderRatio) / num : 1 - 2 * borderRatio;
		auto h = horizontal ? 1 - 2 * borderRatio : (1.0 - (num - 1) * borderRatio) / num;
		auto localBounds = getLocalBounds().toFloat();
		juce::Rectangle<int> bounds;
		for (int i = 0; i < comps.size(); ++i)
		{
			if (horizontal)
			{
				bounds = bdsp::shrinkRectangleToInt(localBounds.getProportion(juce::Rectangle<float>(i * (borderRatio + w), borderRatio, w, h)));
			}
			else
			{
				bounds = bdsp::shrinkRectangleToInt(localBounds.getProportion(juce::Rectangle<float>(borderRatio, i * (borderRatio + h), w, h)));
			}
			comps[i]->setBounds(bounds);
			draggerSlots[i]->setBounds(shrinkRectangleToInt(dragHandleBoundsRelativeToComp.translated(comps[i]->getX(), comps[i]->getY())));
		}
		for (int i = 0; i < draggers.size(); ++i)
		{
			int idx = draggers[i]->getIndex();
			draggers[i]->setBounds(draggerSlots[idx]->getBounds());
		}

	}

	void StationaryRearrangableComponentManager::setDragHandleBoundsRelativeToComp(juce::Rectangle<float> newBounds)
	{
		dragHandleBoundsRelativeToComp = newBounds;
		resized();
	}

	void StationaryRearrangableComponentManager::addComponent(juce::Component* newComp)
	{
		comps.add(newComp);
		vpComp.addAndMakeVisible(comps.getLast());


		draggers.add(new Dragger(this, comps.size() - 1));
		vpComp.addAndMakeVisible(draggers.getLast());

	}



	void StationaryRearrangableComponentManager::reorderComponents(int indexMoved, int indexMovedTo)
	{


		//for (int i = 0; i < comps.size(); ++i)
		//{
		//	int idx = draggers[i]->getIndex();
		//	if (idx != indexMovedTo)
		//	{
		//		draggers[i]->setBounds(shrinkRectangleToInt(dragHandleBoundsRelativeToComp.translated(comps[idx]->getX(), comps[idx]->getY())));
		//	}
		//	//draggers[i]->repaint();
		//}
		resized();



		RearrangableComponentManagerBase::reorderComponents(indexMoved, indexMovedTo);


	}





} // namespace bdsp