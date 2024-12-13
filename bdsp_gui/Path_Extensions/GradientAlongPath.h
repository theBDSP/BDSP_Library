#pragma once




namespace bdsp
{
	class GradientAlongPath
	{
	public:
		GradientAlongPath(juce::ColourGradient& gradient);
		~GradientAlongPath() = default;

		void drawPath(juce::Graphics& g, juce::Path& p, juce::PathStrokeType& stroke, bool smoothOutGaps = true);

		juce::ColourGradient& getGradient();
	private:
		juce::ColourGradient cg;
	};
} // namespace bdsp
