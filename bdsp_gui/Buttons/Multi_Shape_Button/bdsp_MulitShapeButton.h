#pragma once


namespace bdsp
{
	struct Shape
	{
		Shape()
		{

		}
		//defaults off color to same as normal fill color
		// gradient fills will be scaled by path bounds
		Shape(juce::Path p, const NamedColorsIdentifier& fill, const juce::Rectangle<float>& relBounds, const  NamedColorsIdentifier& outlineColor = NamedColorsIdentifier(), float outlineProportion = 0, bool fillunderStroke = true)
		{
			path = p;
			fillOn = fill;
			fillOff = fill;
			relativeBounds = relBounds;
			outline = outlineColor;
			outlineProportionOfWidth = outlineProportion;
			fillUnder = fillunderStroke;
		}


		Shape(const Shape& other)
			:Shape()
		{
			path = other.path;
			fillOn = other.fillOn;
			fillOff = other.fillOff;
			relativeBounds = other.relativeBounds;
			outline = other.outline;
			outlineProportionOfWidth = other.outlineProportionOfWidth;
			fillUnder = other.fillUnder;
		}

		void draw(juce::Graphics& g, juce::Rectangle<float> bounds, bool isOn, bool isHover, GUI_Universals* universals, float alpha = 1.0f)
		{
			auto getColor = [=](NamedColorsIdentifier& c)
			{
				return universals->colors.getColor(c);
			};




			juce::Colour fillColor;

			auto outlineColor = universals->hoverAdjustmentFunc(getColor(outline), isHover || isOn).withMultipliedAlpha(alpha);
			if (getColor(fillOn) != getColor(fillOff) && !fillOn.isEmpty() && !fillOff.isEmpty())
			{
				fillColor = getHoverColor(getColor(fillOn), getColor(fillOff), isOn, isHover, universals->hoverMixAmt).withMultipliedAlpha(alpha);
			}
			else
			{
				auto fill = (isOn ? fillOn : fillOff);

				fillColor = universals->hoverAdjustmentFunc(getColor(fill), isHover).withMultipliedAlpha(alpha);

			}



			auto rect = bounds.getProportion(relativeBounds);

			path.scaleToFit(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), true);


			auto thicc = outlineProportionOfWidth * bounds.getWidth();

			if (fillUnder)
			{
				g.setFillType(fillColor);
				g.fillPath(path);

				g.setColour(outlineColor);
				g.strokePath(path, juce::PathStrokeType(thicc));
			}
			else
			{
				g.setColour(outlineColor);
				g.strokePath(path, juce::PathStrokeType(thicc));

				g.setFillType(fillColor);
				g.fillPath(path);
			}
		}


		juce::Path path;
		NamedColorsIdentifier fillOn;
		NamedColorsIdentifier fillOff;
		NamedColorsIdentifier outline;
		float outlineProportionOfWidth = 0;
		juce::Rectangle<float> relativeBounds;
		bool fillUnder = false;
	};

	class MultiShapeButton : public Button
	{
	public:

		MultiShapeButton(GUI_Universals* universalsToUse, bool toggle = false, bool on = false);

		~MultiShapeButton();

		MultiShapeButton& operator = (const MultiShapeButton& oldButton);


		void addShapes(const juce::Array<Shape>& newShapes);

		void addShapes(const juce::Array<Shape>& newUpShapes, const juce::Array<Shape>& newDownShapes);

		void clearShapes();

		void resized() override;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

		void setAllColors(const NamedColorsIdentifier& colorToReplaceWith);
		void replaceColor(const NamedColorsIdentifier& colorToReplace, const NamedColorsIdentifier& colorToReplaceWith);


		// if I want more precise mouse detection
		//bool hitTest(int x, int y)override;



	private:

		juce::Array<Shape> upShapes, downShapes;
		juce::OwnedArray<juce::Path> upPaths, downPaths;






	};
}// namnepace bdsp


