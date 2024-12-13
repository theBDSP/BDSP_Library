#include "bdsp_DragHandle.h"

namespace bdsp
{

	DragHandle::DragHandle(GUI_Universals* universalsToUse, juce::Component* componentToDrag, Shape shapeToUse)
		:Component(universalsToUse)
	{
		shape = shapeToUse;
		dragee = componentToDrag;

	}



	void DragHandle::paint(juce::Graphics& g)
	{


		float alpha = isEnabled() ? 1.0 : universals->disabledAlpha;






		auto outlineColor = universals->hoverAdjustmentFunc(getColor(shape.outline), isMouseOverOrDragging()).withMultipliedAlpha(alpha);
		auto fillColor = universals->hoverAdjustmentFunc(getColor(shape.fillOn), isMouseOverOrDragging()).withMultipliedAlpha(alpha);



		auto rect = getLocalBounds().toFloat().getProportion(shape.relativeBounds);

		scaleToFit(shape.path, rect);


		auto thicc = shape.outlineProportionOfWidth * rect.getWidth();

		if (shape.fillUnder)
		{
			g.setColour(fillColor);
			g.fillPath(shape.path);

			g.setColour(outlineColor);
			g.strokePath(shape.path, juce::PathStrokeType(thicc));
		}
		else
		{
			g.setColour(outlineColor);
			g.strokePath(shape.path, juce::PathStrokeType(thicc));

			g.setColour(fillColor);
			g.fillPath(shape.path);
		}

	}


	void DragHandle::setLegalBounds(juce::Rectangle<int> newLegalBounds)
	{
		legalBounds = newLegalBounds;
	}

	void DragHandle::resized()
	{
	}

	void DragHandle::mouseDown(const juce::MouseEvent& e)
	{
		dragee->toFront(false);
		dragger.startDraggingComponent(dragee, e);
	}

	void DragHandle::mouseDrag(const juce::MouseEvent& e)
	{
		if (snapMovement)
		{
			if (snapVertically)
			{
				juce::MouseEvent snappedE(e.withNewPosition(juce::Point<float>(e.mouseDownPosition.x, e.position.y)));
				dragger.dragComponent(dragee, snappedE, nullptr);
			}
			else
			{
				juce::MouseEvent snappedE(e.withNewPosition(juce::Point<float>(e.position.x, e.mouseDownPosition.y)));
				dragger.dragComponent(dragee, snappedE, nullptr);
			}
		}
		else
		{
			dragger.dragComponent(dragee, e, nullptr);
		}

		//constrain dragee to legal bounds
		dragee->setBounds(dragee->getBounds().constrainedWithin(legalBounds));

	}


	void DragHandle::mouseUp(const juce::MouseEvent& e)
	{
		if (onDragEnd.operator bool())
		{
			onDragEnd(e);
		}
	}

	void DragHandle::setNewShape(Shape newShape)
	{
		shape = newShape;
	}

	Shape& DragHandle::getShape()
	{
		return shape;
	}

	void DragHandle::setMovementSnapping(bool shouldSnap, bool verticalSnap)
	{
		snapMovement = shouldSnap;
		snapVertically = verticalSnap;
	}




} // namespace bdsp