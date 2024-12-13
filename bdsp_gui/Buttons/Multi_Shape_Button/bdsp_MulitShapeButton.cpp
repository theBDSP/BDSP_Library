#include "bdsp_MulitShapeButton.h"


namespace bdsp
{
	MultiShapeButton::MultiShapeButton(GUI_Universals* universalsToUse, bool toggle, bool on)
		: Button(universalsToUse)
	{
		setClickingTogglesState(toggle);

		setToggleState(on, juce::dontSendNotification);
		setWantsKeyboardFocus(false);


	}

	MultiShapeButton::~MultiShapeButton()
	{

	}

	MultiShapeButton& MultiShapeButton::operator = (const MultiShapeButton& oldButton)
	{
		upShapes.clear();
		addShapes(oldButton.upShapes);




		return *this;
	}


	void MultiShapeButton::addShapes(const juce::Array<Shape>& newShapes)
	{
		upShapes.addArray(newShapes);
		for (int i = 0; i < upShapes.size(); ++i)
		{
			upPaths.add(new juce::Path(upShapes[i].path));
		}



	}

	void MultiShapeButton::addShapes(const juce::Array<Shape>& newUpShapes, const juce::Array<Shape>& newDownShapes)
	{
		addShapes(newUpShapes);
		downShapes.addArray(newDownShapes);
		for (int i = 0; i < downShapes.size(); ++i)
		{
			downPaths.add(new juce::Path(downShapes[i].path));

		}
	}

	void MultiShapeButton::clearShapes()
	{
		upShapes.clear();
		downShapes.clear();
		upPaths.clear();
		downPaths.clear();

	}


	void MultiShapeButton::resized()
	{
		//repaint();
	}

	void MultiShapeButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
	{
		bool isOver = shouldDrawButtonAsHighlighted || isHovering(); //  other comps can control mouse state of button
		for (int i = 0; i < upShapes.size(); ++i)
		{
			auto currentShape = downShapes.isEmpty() ? upShapes[i] : (getToggleState() ? downShapes[i] : upShapes[i]);
			auto currentPath = downShapes.isEmpty() ? upPaths[i] : (getToggleState() ? downPaths[i] : upPaths[i]);
			juce::Identifier fill;
			auto lnf = dynamic_cast<LookAndFeel*>(&getLookAndFeel());
			float alpha = isEnabled() ? 1.0 : 0.5;

			if (lnf != nullptr && !isEnabled())
			{
				alpha = lnf->universals->disabledAlpha;
			}
			/*		if (!getClickingTogglesState())
					{
						fill = currentShape.fillOn;
						if (fill.isColour())
						{
							fill.setColour(currentShape.fillOn.colour.withMultipliedBrightness(1 + (int)isOver * brightnessDelta + (int)shouldDrawButtonAsDown * brightnessDelta).withMultipliedAlpha(alpha));
						}
						else if (fill.isGradient())
						{
							fill.colour = juce::Colour().withAlpha(alpha);
							for (int i = 0; i < fill.gradient->getNumColours(); ++i)
							{
								fill.gradient->setColour(i, fill.gradient->getColour(i).withMultipliedBrightness(1 + (int)isOver * brightnessDelta + (int)shouldDrawButtonAsDown * brightnessDelta));
							}
						}
						else
						{
							jassertfalse;
						}


					}*/

			juce::Colour fillColor;

			auto outlineColor = universals->hoverAdjustmentFunc(getColor(currentShape.outline), isOver || shouldDrawButtonAsDown).withMultipliedAlpha(alpha);
			if (getColor(currentShape.fillOn) != getColor(currentShape.fillOff) && !currentShape.fillOn.isEmpty() && !currentShape.fillOff.isEmpty())
			{
				fillColor = getHoverColor(getColor(currentShape.fillOn), getColor(currentShape.fillOff), shouldDrawButtonAsDown || getToggleState(), isOver, universals->hoverMixAmt).withMultipliedAlpha(alpha);
			}
			else
			{
				fill = (shouldDrawButtonAsDown || getToggleState() ? currentShape.fillOn : currentShape.fillOff);

				fillColor = universals->hoverAdjustmentFunc(getColor(fill), isOver).withMultipliedAlpha(alpha);

			}



			auto rect = getLocalBounds().toFloat().getProportion(currentShape.relativeBounds);

			currentPath->scaleToFit(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), true);


			auto thicc = currentShape.outlineProportionOfWidth * getWidth();

			if (currentShape.fillUnder)
			{
				g.setFillType(fillColor);
				g.fillPath(*currentPath);

				g.setColour(outlineColor);
				g.strokePath(*currentPath, juce::PathStrokeType(thicc));
			}
			else
			{
				g.setColour(outlineColor);
				g.strokePath(*currentPath, juce::PathStrokeType(thicc));

				g.setFillType(fillColor);
				g.fillPath(*currentPath);
			}
		}



	}

	void MultiShapeButton::setAllColors(const NamedColorsIdentifier& colorToReplaceWith)
	{
		for (int i = 0; i < upShapes.size(); ++i)
		{
			auto s = upShapes[i];
			if (s.fillOff.ID.isValid())
			{
				s.fillOff = colorToReplaceWith;
			}
			if (s.fillOn.ID.isValid())
			{
				s.fillOn = colorToReplaceWith;
			}
			if (s.outline.ID.isValid())
			{
				s.outline = colorToReplaceWith;
			}
			upShapes.set(i, s);
		}

		for (int i = 0; i < downShapes.size(); ++i)
		{
			auto s = downShapes[i];
			if (s.fillOff.ID.isValid())
			{
				s.fillOff = colorToReplaceWith;
			}
			if (s.fillOn.ID.isValid())
			{
				s.fillOn = colorToReplaceWith;
			}
			if (s.outline.ID.isValid())
			{
				s.outline = colorToReplaceWith;
			}
			downShapes.set(i, s);
		}


	}

	void MultiShapeButton::replaceColor(const NamedColorsIdentifier& colorToReplace, const NamedColorsIdentifier& colorToReplaceWith)
	{
		for (int i = 0; i < upShapes.size(); ++i)
		{
			auto s = upShapes[i];
			if (getColor(s.fillOff) == getColor(colorToReplace))
			{
				s.fillOff = colorToReplaceWith;
			}
			if (getColor(s.fillOn) == getColor(colorToReplace))
			{
				s.fillOn = colorToReplaceWith;
			}
			if (getColor(s.outline) == getColor(colorToReplace))
			{
				s.outline = colorToReplaceWith;
			}
			upShapes.set(i, s);
		}

		for (int i = 0; i < downShapes.size(); ++i)
		{
			auto s = downShapes[i];
			if (getColor(s.fillOff) == getColor(colorToReplace))
			{
				s.fillOff = colorToReplaceWith;
			}
			if (getColor(s.fillOn) == getColor(colorToReplace))
			{
				s.fillOn = colorToReplaceWith;
			}
			if (getColor(s.outline) == getColor(colorToReplace))
			{
				s.outline = colorToReplaceWith;
			}
			downShapes.set(i, s);
		}


	}





	//bool MultiShapeButton::hitTest(int x, int y)
	//{
	//    bool out = false;
	//    for (int i = 0; i < upShapes.size(); ++i)
	//    {
	//        
	//            auto rect = getLocalBounds().toFloat().getProportion(upShapes[i].relativeBounds);
	//            auto transform = upShapes[i].path.getTransformToScaleToFit(rect, true);
	//            auto p = juce::Point<float>(x, y);
	//            p.applyTransform(transform.inverted());
	//            if (upShapes[i].path.contains(p))
	//            {
	//                out = true;
	//                break;
	//            }
	//      
	//     
	//    }
	//    return out;
	//}


}// namnepace bdsp
