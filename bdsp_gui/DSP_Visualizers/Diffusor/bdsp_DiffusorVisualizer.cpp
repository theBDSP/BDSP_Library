#include "bdsp_DiffusorVisualizer.h"

namespace bdsp
{


	DiffusorVisualizer::DiffusorVisualizerInternal::DiffusorVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* timeSlider, BaseSlider* mixSlider)
		: OpenGLCirclePlotter(universalsToUse, 25 * 25 * 2),
		color(universalsToUse, this)
	{
		time = timeSlider;
		mix = mixSlider;

		auto xBounds = 0.75f;
		auto yBounds = 0.75f;
		auto spacing = 0.8f;
		pointW = xBounds / cols * spacing;
		pointH = yBounds / rows * spacing;
		juce::Random rand;
		float dispScale = 0.2;
		for (int r = 0; r < rows; ++r)
		{
			float x = (2 * r / (rows - 1.0f)) - 1;
			x *= xBounds;
			for (int c = 0; c < cols; ++c)
			{

				float y = (2 * c / (cols - 1.0f)) - 1;
				y *= yBounds;
				baseVerticies.add(juce::Point<float>(x, y));
				displacementVectors.add(juce::Point<float>((rand.nextBool() ? -1 : 1) * rand.nextFloat() * dispScale, (rand.nextBool() ? -1 : 1) * rand.nextFloat() * dispScale));

			}
		}

		universals->contextHolder->registerOpenGlRenderer(this);
	}



	DiffusorVisualizer::DiffusorVisualizerInternal::~DiffusorVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}







	void DiffusorVisualizer::DiffusorVisualizerInternal::setColor(const NamedColorsIdentifier& newColor)
	{
		color.setColors(newColor, background.getColorID(false).mixedWith(newColor, universals->disabledAlpha));
	}










	void DiffusorVisualizer::DiffusorVisualizerInternal::generateVertexBuffer()
	{
		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());
		timeVal = juce::jmap(time->getNormalisableRange().convertTo0to1(time->getActualValue()), 0.05, 1.0);


		float r, g, b, a;
		color.getComponents(r, g, b, a);

		juce::Point<float> c;
		for (int i = 0; i < baseVerticies.size(); ++i)
		{
			auto p = baseVerticies[i];
			juce::Point<float> delta = displacementVectors[i] * timeVal;

			c = p + delta;

			circleData.set(2 * i,
				{
					c.x,c.y,
					r,g,b,mixVal
				});

			circleData.set(2 * i + 1,
				{
					p.x,p.y,
					r,g,b,1 - mixVal
				});
		}

		generateCircleVerticies();

	}








	//================================================================================================================================================================================================



	DiffusorVisualizer::DiffusorVisualizer(GUI_Universals* universalsToUse, BaseSlider* timeSlider, BaseSlider* mixSlider)
		:Component(universalsToUse),
		vis(universalsToUse, timeSlider, mixSlider)
	{
		addAndMakeVisible(vis);
	}

	void DiffusorVisualizer::resized()
	{
		vis.setRadius(juce::jmax(universals->roundedRectangleCurve / 6, 3.0f));
		vis.setBounds(getLocalBounds().reduced(universals->roundedRectangleCurve));
	}

	void DiffusorVisualizer::paint(juce::Graphics& g)
	{
		g.setColour(vis.getBackgroundColor());
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
	}

	void DiffusorVisualizer::setColor(const NamedColorsIdentifier& newColor)
	{
		vis.setColor(newColor);
	}

} // namespace bdsp