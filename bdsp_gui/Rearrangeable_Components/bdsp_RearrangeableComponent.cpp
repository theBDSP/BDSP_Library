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
		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragText = s;

			dragBoxes[i]->dragger.repaint();
		}

		longestTitle = s;
	}

	void RearrangableComponentManagerBase::setDragHandlePath(const juce::Path& p)
	{
		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragHandlePath.clear();
			dragBoxes[i]->dragger.dragHandlePath.addPath(p);

			dragBoxes[i]->repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandleColors(const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor)
	{

		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragColor = normalColor;
			dragBoxes[i]->dragger.dragColorDown = downColor;

			dragBoxes[i]->setColors(slotColor);

			dragBoxes[i]->repaint();
			draggerSlots[i]->repaint();

		}
	}

	void RearrangableComponentManagerBase::setSingleDragHandleText(int idx, const juce::String& s)
	{
		int i = getPositionOfDragBox(idx);

		dragBoxes[i]->dragger.dragText = s;

		dragBoxes[i]->dragger.repaint();

		calculateLongestTitle();
	}

	void RearrangableComponentManagerBase::setSingleDragHandlePath(int idx, const juce::Path& p)
	{
		int i = getPositionOfDragBox(idx);

		dragBoxes[i]->dragger.dragHandlePath.clear();
		dragBoxes[i]->dragger.dragHandlePath.addPath(p);

		dragBoxes[i]->dragger.repaint();
	}

	void RearrangableComponentManagerBase::setSingleDragHandleColors(int idx, const NamedColorsIdentifier& normalColor, const NamedColorsIdentifier& downColor, const NamedColorsIdentifier& slotColor)
	{
		int i = getPositionOfDragBox(idx);
		dragBoxes[i]->dragger.dragColor = normalColor;
		dragBoxes[i]->dragger.dragColorDown = downColor;

		dragBoxes[i]->setColors(slotColor);

		dragBoxes[i]->repaint();
		draggerSlots[i]->repaint();
	}

	void RearrangableComponentManagerBase::setDragHandleTexts(juce::StringArray strings)
	{
		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragText = strings[i];

			dragBoxes[i]->dragger.repaint();
		}
		calculateLongestTitle();
	}

	void RearrangableComponentManagerBase::setDragHandlePaths(juce::Array<juce::Path> paths)
	{
		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragHandlePath.clear();
			dragBoxes[i]->dragger.dragHandlePath.addPath(paths[i]);

			dragBoxes[i]->dragger.repaint();
		}
	}

	void RearrangableComponentManagerBase::setDragHandleColors(juce::Array<NamedColorsIdentifier> normalColors, juce::Array<NamedColorsIdentifier> downColors, juce::Array<NamedColorsIdentifier> slotColors)
	{

		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			dragBoxes[i]->dragger.dragColor = normalColors[i];
			dragBoxes[i]->dragger.dragColorDown = downColors[i];

			dragBoxes[i]->setColors(slotColors[i]);

			dragBoxes[i]->repaint();
			draggerSlots[i]->repaint();
		}
	}

	int RearrangableComponentManagerBase::getPositionOfDragBox(int dragBoxIndex)
	{
		return orderedDragBoxes.indexOf(dragBoxes[dragBoxIndex]);
	}

	void RearrangableComponentManagerBase::attach(OrderedListParameter* param)
	{
		attachment = std::make_unique<RearrangableComponentParameterAttachment>(*param, *this, universals->undoManager);
	}


	void RearrangableComponentManagerBase::addListener(Listener* listenerToAdd)
	{
		listeners.add(listenerToAdd);
	}

	void RearrangableComponentManagerBase::removeListener(Listener* listenerToRemove)
	{
		listeners.remove(listenerToRemove);
	}

	void RearrangableComponentManagerBase::addComponent(juce::Component* newComp)
	{

		comps.add(newComp);
		addAndMakeVisible(comps.getLast());


		draggerSlots.add(new Component(universals));
		addAndMakeVisible(draggerSlots.getLast(), 0);

		dragBoxes.add(new DragBox(this, dragBoxes.size()));
		addAndMakeVisible(dragBoxes.getLast());
		orderedDragBoxes.add(dragBoxes.getLast());

		dragBoxes.getLast()->setAlwaysOnTop(true);

	}



	void RearrangableComponentManagerBase::reorderComponents(const juce::Array<int>& newOrder, bool notifyListeners)
	{
		for (int i = 0; i < newOrder.size(); ++i)
		{
			orderedDragBoxes.set(i, dragBoxes[newOrder[i]]);
		}
		reorderComponents(0, 0, notifyListeners);
	}

	void RearrangableComponentManagerBase::reorderComponents(DragBox* boxMoved, int indexMovedTo, bool notifyListeners)
	{
		reorderComponents(orderedDragBoxes.indexOf(boxMoved), indexMovedTo, notifyListeners);
	}

	void RearrangableComponentManagerBase::reorderComponents(int indexMoved, int indexMovedTo, bool notifyListeners)
	{
		orderedDragBoxes.move(indexMoved, indexMovedTo);

		for (int i = 0; i < orderedDragBoxes.size(); ++i)
		{
			if (!orderedDragBoxes[i]->dragger.isDragging)
			{
				orderedDragBoxes[i]->setBounds(draggerSlots[i]->getBounds());
			}
			//orderedDragBoxes[i]->toFront(false);
		}

		if (notifyListeners)
		{
			listeners.call([=](Listener& l) {l.componentOrderChanged(this); });

			if (onComponentOrderChanged.operator bool())
			{
				onComponentOrderChanged(indexMoved, indexMovedTo);
			}

		}

		resized();
	}


	juce::Component* RearrangableComponentManagerBase::getComp(int idx) const
	{
		return comps.getUnchecked(idx);
	}

	int RearrangableComponentManagerBase::getNumComps() const
	{
		return comps.size();
	}

	juce::Array<int> RearrangableComponentManagerBase::getOrder()
	{
		juce::Array<int> out;

		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			out.add(dragBoxes.indexOf(orderedDragBoxes[i]));
		}
		return out;
	}

	void RearrangableComponentManagerBase::calculateLongestTitle()
	{
		juce::StringArray titles;

		for (auto* d : dragBoxes)
		{
			titles.add(d->dragger.dragText);
		}
		longestTitle = universals->Fonts[fontIndex].getWidestString(titles);
		repaint();
	}




	//================================================================================================================================================================================================

	RearrangableComponentManagerBase::DragBox::Dragger::Dragger(DragBox* parent)
		:Component(parent->universals)
	{
		p = parent;
		//p->toFront(false);

	}


	void RearrangableComponentManagerBase::DragBox::Dragger::mouseEnter(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::DraggingHandCursor);
	}

	void RearrangableComponentManagerBase::DragBox::Dragger::mouseExit(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	void RearrangableComponentManagerBase::DragBox::Dragger::mouseDown(const juce::MouseEvent& e)
	{
		p->toFront(false);
		p->universals->undoManager->beginNewTransaction();
		startDraggingComponent(p, e);
		isDragging = true;

		for (int i = 0; i < p->p->dragBoxes.size(); ++i)
		{
			p->p->baselineDragOrder.set(i, p->p->dragBoxes[i]->getIndex());
		}


		repaint();
	}
	void RearrangableComponentManagerBase::DragBox::Dragger::mouseDrag(const juce::MouseEvent& e)
	{

		dragComponent(p, e, &p->p->dragBoundsConstrainer);

		for (int i = 0; i < p->p->draggerSlots.size(); ++i)
		{
			int dragPos = p->p->getPositionOfDragBox(p->idx);
			if (i != dragPos && p->p->draggerSlots[i]->isShowing())
			{
				int l, r, t, b;
				l = p->p->draggerSlots[i]->getX();
				r = p->p->draggerSlots[i]->getRight();
				t = p->p->draggerSlots[i]->getY();
				b = p->p->draggerSlots[i]->getBottom();



				if ((p->p->horizontal && isBetweenInclusive(p->getBounds().getCentreX(), l, r)) || (!p->p->horizontal && isBetweenInclusive(p->getBounds().getCentreY(), t, b)))
				{
					p->p->reorderComponents(p, i, p->p->instantSwap);
					break;
				}
			}

		}

		if (p->p->onDrag.operator bool())
		{
			p->p->onDrag(p->p->getPositionOfDragBox(p->idx), e.getEventRelativeTo(p->p).getPosition());
		}


	}
	void RearrangableComponentManagerBase::DragBox::Dragger::mouseUp(const juce::MouseEvent& e)
	{
		isDragging = false;

		if (!p->p->instantSwap)
		{
			for (int i = 0; i < p->p->draggerSlots.size(); ++i)
			{
				if (p->p->draggerSlots[i]->getBounds().contains(getBounds().getCentre()))
				{
					p->p->reorderComponents(p, i);
				}

			}
		}


		p->setBounds(p->p->draggerSlots[p->p->getPositionOfDragBox(p->idx)]->getBounds());
		repaint();
	}

	void RearrangableComponentManagerBase::DragBox::Dragger::paint(juce::Graphics& g)
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
			auto f = resizeFontToFit(getFont(), getWidth() * 0.9, getHeight() * 0.9, p->p->longestTitle);
			drawText(g, f, dragText, getLocalBounds(), false, juce::Justification::centredLeft);
		}

	}
	void RearrangableComponentManagerBase::DragBox::Dragger::resized()
	{
	}


	int RearrangableComponentManagerBase::DragBox::getIndex()
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
		vpComp.setBounds(getLocalBounds());

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
		for (int i = 0; i < dragBoxes.size(); ++i)
		{
			int idx = dragBoxes[i]->getIndex();
			dragBoxes[i]->setBounds(draggerSlots[idx]->getBounds());
		}

	}


	void StationaryRearrangableComponentManager::setDragHandleBoundsRelativeToComp(juce::Rectangle<float> newBounds)
	{
		dragHandleBoundsRelativeToComp = newBounds;
		resized();
	}

	void StationaryRearrangableComponentManager::addComponent(juce::Component* newComp)
	{
		RearrangableComponentManagerBase::addComponent(newComp);

		vpComp.addAndMakeVisible(comps.getLast());
		vpComp.addAndMakeVisible(dragBoxes.getLast());
		vpComp.addAndMakeVisible(draggerSlots.getLast(), 0);

	}



	void StationaryRearrangableComponentManager::reorderComponents(int indexMoved, int indexMovedTo, bool notifyListeners)
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



		RearrangableComponentManagerBase::reorderComponents(indexMoved, indexMovedTo, notifyListeners);


	}





	RearrangableComponentManagerBase::DragBox::DragBox(RearrangableComponentManagerBase* parent, int index)
		:BasicContainerComponent(parent->universals, NamedColorsIdentifier()),
		dragger(this)
	{
		p = parent;
		idx = index;

		setInterceptsMouseClicks(false, true);
		addAndMakeVisible(dragger);
	}

	void RearrangableComponentManagerBase::DragBox::paint(juce::Graphics& g)
	{
		if (p->dragBoxPaintFunc.operator bool())
		{
			p->dragBoxPaintFunc(g, this);
		}
		else
		{
			BasicContainerComponent::paint(g);
		}
	}


	RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::OrderedListParameterAttachment(OrderedListParameter& parameter, RearrangableComponentManagerBase& comp, juce::UndoManager* undoManager)
		:rearrangeComp(comp),
		storedParameter(parameter),
		um(undoManager),
		OrderedListParameter::Listener(&parameter)
	{
		storedParameter.addListener(this);
		sendInitialUpdate();

	}



	RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::~OrderedListParameterAttachment()
	{
		storedParameter.removeListener(this);

	}

	void RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::sendInitialUpdate()
	{
		orderChanged(storedParameter.getOrder());
	}

	void RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::setValue(juce::Array<int> newOrder)
	{
		if (storedParameter.getOrder() != newOrder)
		{
			if (um != nullptr)
			{
				//um->beginNewTransaction();
			}
			storedParameter.setNewOrder(newOrder);
		}
	}



	void RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::orderChanged(const juce::Array<int> newOrder)
	{
		if (juce::MessageManager::getInstance()->isThisTheMessageThread())
		{
			cancelPendingUpdate();
			handleAsyncUpdate();
		}
		else
		{
			triggerAsyncUpdate();
		}
	}

	void RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::OrderedListParameterAttachment::handleAsyncUpdate()
	{
		rearrangeComp.reorderComponents(storedParameter.getOrder());
	}


	RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::RearrangableComponentParameterAttachment(OrderedListParameter& parameter, RearrangableComponentManagerBase& comp, juce::UndoManager* undoManager)
		:parameterAttachment(parameter, comp, undoManager),
		listComp(comp),
		listParameter(parameter)
	{
		listComp.addListener(this);
	}

	void RearrangableComponentManagerBase::RearrangableComponentParameterAttachment::componentOrderChanged(RearrangableComponentManagerBase* rearrangableCompThatChanged)
	{
		parameterAttachment.setValue(listComp.getOrder());
	}

} // namespace bdsp