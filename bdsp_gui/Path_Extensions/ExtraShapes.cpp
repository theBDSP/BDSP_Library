#include "ExtraShapes.h"
namespace bdsp
{
	juce::Path createCenteredPieSegment(const juce::Point<float>& center, float innerR, float outterR, float startAngle, float endAngle, bool roundedEnds)
	{
		juce::Path p;
		if (abs(endAngle - startAngle) >= (3 * PI - PI)) // it's a circle (can't be 2PI b/c weird rounding errors with mulitplication)
		{
			juce::Path c1, c2;
			c1.addEllipse(juce::Rectangle<float>(2 * outterR, 2 * outterR).withCentre(center));
			c2.addEllipse(juce::Rectangle<float>(2 * innerR, 2 * innerR).withCentre(center));

			p = ClipperLib::performBoolean(c1, c2, ClipperLib::ctDifference);
		}
		else
		{
			auto w = outterR - innerR;
			auto r = innerR + w / 2;
			p.addCentredArc(center.getX(), center.getY(), r, r, 0, startAngle, endAngle, true);

			juce::PathStrokeType stroke(w);
			stroke.setEndStyle(roundedEnds ? juce::PathStrokeType::EndCapStyle::rounded : juce::PathStrokeType::EndCapStyle::butt);
			stroke.createStrokedPath(p, p);
		}


		return p;
	}

	juce::Path createCenteredPieTriangle(const juce::Point<float>& center, float startInnerR, float startOutterR, float endInnerR, float endOutterR, float startAngle, float endAngle)
	{
		juce::Path p;

		auto angleDelta = startAngle < endAngle ? 0.05 : -0.05;
		auto num = ceil(abs((endAngle - startAngle) / angleDelta));

		auto rDelta = (endInnerR - startInnerR) / num;

		p.startNewSubPath(center.getPointOnCircumference(startInnerR, startAngle));

		for (int i = 1; i < num; ++i)
		{
			p.lineTo(center.getPointOnCircumference(startInnerR + i * rDelta, startAngle + i * angleDelta));
		}

		p.lineTo(center.getPointOnCircumference(endInnerR, endAngle));

		//=====================================================================================================================


		p.lineTo(center.getPointOnCircumference(endOutterR, endAngle));
		rDelta = (startOutterR - endOutterR) / num;


		for (int i = 1; i < num; ++i)
		{
			p.lineTo(center.getPointOnCircumference(endOutterR + i * rDelta, endAngle - i * angleDelta));
		}

		p.closeSubPath();




		return p;

	}

	juce::Path createCenteredPieTriangle(const juce::Point<float>& center, float centerR, float startW, float endW, float startAngle, float endAngle)
	{
		return createCenteredPieTriangle(center, centerR - startW / 2.0, centerR + startW / 2.0, centerR - endW / 2.0, centerR + endW / 2.0, startAngle, endAngle);
	}

	juce::Path createGear(const juce::Point<float>& center, float outterRadius, float innerRadius, int numTeeth, float toothHeight, float toothAngle)
	{
		jassert(numTeeth > 1);

		juce::Path p;

		p.startNewSubPath(0, outterRadius);

		auto angle = PI / numTeeth;

		auto toothW = toothHeight * tan(toothAngle);

		auto toothRadius = sqrt(toothW * toothW + pow(outterRadius + toothHeight, 2));

		auto angleDelta = atan(toothW / (outterRadius + toothHeight));



		for (int i = 0; i < numTeeth; ++i)
		{

			//	_
			// / \
				//=====================================================================================================================
			p.applyTransform(juce::AffineTransform().rotated(angleDelta));
			p.lineTo(0, toothRadius);

			p.applyTransform(juce::AffineTransform().rotated(angle - 2 * angleDelta));
			p.lineTo(0, toothRadius);

			p.applyTransform(juce::AffineTransform().rotated(angleDelta));
			p.lineTo(0, outterRadius);

			//=====================================================================================================================
			// __


			if (i < numTeeth - 1)
			{
				p.applyTransform(juce::AffineTransform().rotated(angle));
				p.lineTo(0, outterRadius);
			}

		}
		p.closeSubPath();

		p.applyTransform(juce::AffineTransform().rotated(angle / 2).translated(center));

		p.addEllipse(center.getX() - innerRadius, center.getY() - innerRadius, 2 * innerRadius, 2 * innerRadius);
		return p;

	}

	juce::Path createCross(const juce::Point<float>& center, float legRatio, float rotaion)
	{
		juce::Path p;
		auto w = legRatio / 2.0;
		//=====================================================================================================================
		//right arm
		p.startNewSubPath(w, w);
		p.lineTo(1, w);
		p.lineTo(1, -w);
		p.lineTo(w, -w);


		//=====================================================================================================================
		//bottom arm
		p.lineTo(w, -1);
		p.lineTo(-w, -1);
		p.lineTo(-w, -w);

		//=====================================================================================================================
		//left arm
		p.lineTo(-1, -w);
		p.lineTo(-1, w);
		p.lineTo(-w, w);


		//=====================================================================================================================
		//top arm
		p.lineTo(-w, 1);
		p.lineTo(w, 1);
		p.closeSubPath();

		p.applyTransform(juce::AffineTransform().rotated(rotaion).translated(center));
		return p;
	}

	juce::Path createPowerSymbol(const juce::Point<float>& center, float size, bool roundedEnds, float relativeLineWidth, float openingAngle)
	{
		juce::Path p = createCenteredPieSegment(juce::Point<float>(0, 0), 1 - relativeLineWidth, 1, openingAngle / 2, 2 * PI - openingAngle / 2, roundedEnds);

		juce::PathStrokeType stroke(relativeLineWidth, juce::PathStrokeType::curved, roundedEnds ? juce::PathStrokeType::rounded : juce::PathStrokeType::butt);
		juce::Path line;

		line.startNewSubPath(0, 0);
		line.lineTo(0, -1);
		stroke.createStrokedPath(line, line);

		p.addPath(line);
		p.applyTransform(juce::AffineTransform().scaled(size).translated(center));

		return p;

	}

	juce::Path createEquilateralTriangle(const juce::Point<float>& center, float sideLength, float rotation)
	{
		juce::Path p;

		auto h = sqrt(3) / 2.0 * sideLength;
		p.addTriangle(-sideLength / 2, h / 2, sideLength / 2, h / 2, 0.0, -h / 2);
		jassert(p.getBounds().getCentre() == juce::Point<float>(0, 0));
		p.applyTransform(juce::AffineTransform().rotated(rotation).translated(center));

		return p;
	}

	juce::Path createEquilateralTriangle(const juce::Rectangle<float>& bounds, float rotation)
	{
		auto p = createEquilateralTriangle(bounds.getCentre(), 1, rotation);
		scaleToFit(p, bounds);
		return p;
	}

	juce::Path createX(const juce::Rectangle<float>& bounds, float legWidthRatio, bool rounded)
	{

		juce::Path x;
		x.startNewSubPath(0, 0);
		x.lineTo(1, 1);

		x.startNewSubPath(0, 1);
		x.lineTo(1, 0);

		juce::PathStrokeType(legWidthRatio, juce::PathStrokeType::beveled, rounded ? juce::PathStrokeType::rounded : juce::PathStrokeType::butt).createStrokedPath(x, x);

		scaleToFit(x, bounds);

		return x;
	}



}// namnepace bdsp
