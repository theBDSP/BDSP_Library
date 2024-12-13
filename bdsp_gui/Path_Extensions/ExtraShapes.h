#pragma once




namespace bdsp
{

	juce::Path createCenteredPieSegment(const juce::Point<float>& center, float innerR, float outterR, float startAngle, float endAngle, bool roundedEnd = false);

	juce::Path createCenteredPieTriangle(const juce::Point<float>& center, float startInnerR, float startOutterR, float endInnerR, float endOutterR, float startAngle, float endAngle);
	juce::Path createCenteredPieTriangle(const juce::Point<float>& center, float centerR, float startW, float endW, float startAngle, float endAngle);

	juce::Path createGear(const juce::Point<float>& center, float outterRadius, float innerRadius, int numTeeth, float toothHeight, float toothAngle);
	juce::Path createCross(const juce::Point<float>& center, float legRatio, float rotaion = 0); // creates a cross or + sign that's centered around the origin with width and height of 2
	juce::Path createPowerSymbol(const juce::Point<float>& center, float size, bool roundedEnd=true, float relativeLineWidth = 0.2, float openingAngle = PI / 3);

	juce::Path createEquilateralTriangle(const juce::Point<float>& center, float sideLength, float rotation = 0); // creates equilateral triangle, angle defines angle of a vertex
	juce::Path createEquilateralTriangle(const juce::Rectangle<float>& bounds, float rotation = 0); // creates equilateral triangle, angle defines angle of a vertex

	juce::Path createX(const juce::Rectangle<float>& bounds, float legWidthRatio, bool rounded);

}// namnepace bdsp
