#pragma once



namespace bdsp
{
	// gradient that changed color based on angle from a center point
	// starts at 12 o'clock and goes clockwise
	class ConicGradient
	{
	public:
		ConicGradient();
		ConicGradient(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed = false);
		ConicGradient(const juce::Colour& c1, const juce::Colour& c2, const juce::Rectangle<float>& bounds, bool reversed = false);
		~ConicGradient();

		void reset(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed = false);
		void reset(const juce::ColourGradient& cg, const juce::Point<float>& newCenter, bool reversed = false);

		void reverse();

		void fillPath(juce::Graphics& g, const juce::Path& p, float rotationInRadians = 0.0f);

	private:
		void init(const juce::ColourGradient& cg, const juce::Rectangle<float>& bounds, bool reversed = false);

		juce::Image img;
		juce::HeapBlock<juce::PixelARGB> lookup;
		juce::Rectangle<float> b;
	};

}// namnepace bdsp






