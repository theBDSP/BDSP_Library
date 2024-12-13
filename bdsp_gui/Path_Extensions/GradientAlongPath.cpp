#include "GradientAlongPath.h"
namespace bdsp
{
	GradientAlongPath::GradientAlongPath(juce::ColourGradient& gradient)
		:cg(gradient)
	{

	}

	void GradientAlongPath::drawPath(juce::Graphics& g, juce::Path& path, juce::PathStrokeType& stroke, bool smoothOutGaps)
	{



		juce::PathFlatteningIterator it(path, juce::AffineTransform(), 1.0f);
		auto length = 0;

		juce::Array<juce::Line<float>> lines, topLines, botLines;



		while (it.next())
		{
			auto line = juce::Line<float>(it.x1, it.y1, it.x2, it.y2);
			lines.add(line);

			auto angle = line.getAngle();

			/*		auto topLine = line;
					topLine.applyTransform(juce::AffineTransform().translated(juce::Point<float>(0,0).getPointOnCircumference(stroke.getStrokeThickness() / 2.0, angle - PI / 2.0)));
					topLines.add(topLine);

					auto botLine = line;
					botLine.applyTransform(juce::AffineTransform().translated(juce::Point<float>(0,0).getPointOnCircumference(stroke.getStrokeThickness() / 2.0, angle + PI / 2.0)));
					botLines.add(botLine);*/

			auto topLine = juce::Line<float>().fromStartAndAngle(line.getPointAlongLine(0, stroke.getStrokeThickness() / 2.0f), line.getLength(), angle);
			topLines.add(topLine);

			auto botLine = juce::Line<float>().fromStartAndAngle(line.getPointAlongLine(0, -stroke.getStrokeThickness() / 2.0f), line.getLength(), angle);
			botLines.add(botLine);

			length += line.getLength();
		}


		double pos = 0;
		jassert(lines.size() > 1);

		juce::Point<float> startTop, startBot, endTop, endBot;

		juce::Image img(juce::Image::ARGB, g.getClipBounds().getWidth(), g.getClipBounds().getHeight(), true);
		juce::Graphics gImg(img);
		for (int i = 0; i < lines.size(); ++i)
		{


			auto dist = lines[i].getLength();

			auto pos1 = pos;
			auto pos2 = pos1 + dist / length;
			pos = pos2;





			if (i == 0) // first
			{
				startTop = topLines[i].getStart();
				startBot = botLines[i].getStart();

				endTop = topLines[i].getIntersection(topLines[i + 1]);
				endBot = botLines[i].getIntersection(botLines[i + 1]);

			}
			else if (i == lines.size() - 1) // last
			{
				//startTop = topLines[i].getIntersection(topLines[i - 1]);
				//startBot = botLines[i].getIntersection(botLines[i - 1]);

				startTop = endTop;
				startBot = endBot;

				endTop = topLines[i].getEnd();
				endBot = botLines[i].getEnd();

			}
			else
			{

				//startTop = topLines[i].getIntersection(topLines[i - 1]);
				//startBot = botLines[i].getIntersection(botLines[i - 1]);


				startTop = endTop;
				startBot = endBot;

				endTop = topLines[i].getIntersection(topLines[i + 1]);
				endBot = botLines[i].getIntersection(botLines[i + 1]);

			}

			juce::Path p;

			p.startNewSubPath(startTop);
			p.lineTo(endTop);
			p.lineTo(endBot);
			p.lineTo(startBot);
			p.closeSubPath();


			juce::ColourGradient grad(cg.getColourAtPosition(pos1), lines[i].getStart(), cg.getColourAtPosition(pos2), lines[i].getEnd(), false);

			gImg.setGradientFill(grad);
			//gImg.setColour(juce::Colours::white);
			gImg.fillPath(p);



		}

		if (smoothOutGaps)
		{
			melatonin::CachedBlur blur(juce::jlimit(2, 254, juce::jmax(img.getWidth(), img.getHeight()) / 100));
			blur.render(img);
			g.saveState();

			juce::Path stroked;
			stroke.createStrokedPath(stroked, path);
			g.reduceClipRegion(stroked);
			g.drawImage(img, g.getClipBounds().withZeroOrigin().toFloat());

			g.restoreState();
		}
		else
		{
			g.drawImage(img, g.getClipBounds().withZeroOrigin().toFloat());
		}
	}

	juce::ColourGradient& GradientAlongPath::getGradient()
	{
		return cg;
	}

} // namespace bdsp
