#include "ConicGradient.h"
namespace bdsp
{
	ConicGradient::ConicGradient()
	{
	}
	ConicGradient::ConicGradient(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed)
	{
		init(cg, bounds, reversed);
	}
	ConicGradient::ConicGradient(const juce::Colour& c1, const juce::Colour& c2, const juce::Rectangle<float>& bounds, bool reversed)
	{
		auto l = juce::jmax(bounds.getWidth(), bounds.getHeight());
		juce::ColourGradient cg(c1, 0, 0, c2, l, l, false);
		init(cg, bounds, reversed);
	}
	ConicGradient::~ConicGradient()
	{
	}
	void ConicGradient::reset(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed)
	{
		init(cg, bounds, reversed);
	}
	void ConicGradient::reset(const juce::ColourGradient& cg, const juce::Point<float>& newCenter, bool reversed)
	{
		init(cg, b.withCentre(newCenter), reversed);
	}


	void ConicGradient::reverse()
	{
		const auto temp = img.createCopy();
		img.clear(img.getBounds());

		juce::Graphics g(img);
		g.addTransform(juce::AffineTransform(-1, 0, temp.getWidth(), 0, 1, 0));
		g.drawImage(temp,temp.getBounds().toFloat());
	}

	void ConicGradient::fillPath(juce::Graphics& g, const juce::Path& p, float rotationInRadians)
	{

		auto transform = juce::AffineTransform().rotated(rotationInRadians, img.getBounds().toFloat().getCentreX(), img.getBounds().toFloat().getCentreY()).translated(b.getPosition());
		g.saveState();
		g.reduceClipRegion(p);

		g.drawImageTransformed(img, transform);

		g.restoreState();

	}

	void ConicGradient::init(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed)
	{

		const float cx = bounds.getCentreX();
		const float cy = bounds.getCentreY();

		// algorithm from
		//https://stackoverflow.com/questions/3506404/fast-hypotenuse-algorithm-for-embedded-processor
		auto min = bounds.getAspectRatio() > 1 ? bounds.getHeight() : bounds.getWidth();
		auto max = bounds.getAspectRatio() > 1 ? bounds.getWidth() : bounds.getHeight();
		auto d = ((juce::MathConstants<float>::sqrt2 - 1) * min) + max;

		b = juce::Rectangle<float>(d, d).withCentre(bounds.getCentre());

		img = juce::Image(juce::Image::ARGB, ceil(b.getWidth()), ceil(b.getHeight()), false);
		const juce::Image::BitmapData destData(img, juce::Image::BitmapData::writeOnly);

		const float scale = (1.0f / (2.0f * PI));


		const int numEntries = cg.createLookupTable(juce::AffineTransform(), lookup);

		for (int y = 0; y < b.getHeight(); ++y)
		{
			for (int x = 0; x < b.getWidth(); ++x)
			{
				const float angleRadians = atan2(b.getY() + y - cy, b.getX() + x - cx);
				const float a = scale * (angleRadians + PI / 2);
				const int index = juce::roundToInt(a * (numEntries - 1) + numEntries) % numEntries;
				//jassert(juce::isPositiveAndNotGreaterThan(a, 1.0f));
				jassert(juce::isPositiveAndBelow(index, numEntries));

				destData.setPixelColour(reversed ? b.getWidth() - 1 - x : x, y, lookup[index]);
			}
		}
	}
}// namnepace bdsp
