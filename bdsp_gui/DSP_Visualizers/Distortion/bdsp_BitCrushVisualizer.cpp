#include "bdsp_BitCrushVisualizer.h"

namespace bdsp
{


	BitCrushVisualizerInternal::BitCrushVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider)
		: OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this),
		gridColor(universalsToUse, this)
	{
		bitDepth = bitDepthSlider;
		sampleRate = sampleRateSlider;
		mix = mixSlider;

		auto lightColor = NamedColorsIdentifier(BDSP_COLOR_LIGHT);
		gridColor.setColors(BDSP_COLOR_LIGHT, background.getColorID(false).mixedWith(&lightColor, universals->disabledAlpha));







		subClasses.add(new OpenGLCirclePlotter(universals, 29 * 33));
		subClasses.add(new OpenGLLineRenderer(universals, 2 * 29));
		subClasses.add(new OpenGLLineRenderer(universals, 50));
		initSubClasses();

		grid = dynamic_cast<OpenGLCirclePlotter*>(subClasses[0]);
		line = dynamic_cast<OpenGLLineRenderer*>(subClasses[1]);
		dry = dynamic_cast<OpenGLLineRenderer*>(subClasses[2]);

		dry->setHasAlpha(true);

		int num = dry->linePoints.getFirst()->getNumVerticies();
		for (int i = 0; i < num; ++i)
		{
			float x = 2 * i / (num - 1.0f) - 1.0f;
			dryPoints.add({ x, sin(PI * x) });
		}

		universals->contextHolder->registerOpenGlRenderer(this);
	}



	BitCrushVisualizerInternal::~BitCrushVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}




	void BitCrushVisualizerInternal::resized()
	{
		line->setThickness(0, universals->visualizerLineThickness);
		dry->setThickness(0, universals->visualizerLineThickness);
		//grid->setRadius(juce::jmax(universals->roundedRectangleCurve / 4, 2.0f));
		grid->setRadius(universals->visualizerLineThickness);
		OpenGLCompositeComponent::resized();

	}

	void BitCrushVisualizerInternal::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity)
	{
		color.setColors(c, background.getColorID(false).mixedWith(&c, universals->disabledAlpha));
	}

	void BitCrushVisualizerInternal::setScaling(float x, float y)
	{
		xScaling = x;
		yScaling = y;
		resized();
	}










	void BitCrushVisualizerInternal::generateVertexBuffer()
	{


		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());
		depthVal = 1.0f / pow(2.0f, juce::jmap(bitDepth->getActualValue(), 4.0, 2.0));
		rateVal = 1.0f / (juce::jmap(sampleRate->getActualValue(), 1.0, 32.0, 6.0, 1.0) + 8.0f);



		//================================================================================================================================================================================================
		if ((mixVal < 0.5 && line == subClasses[2]) || (mixVal >= 0.5 && line == subClasses[1]))
		{
			subClasses.swap(1, 2);
		}

		//================================================================================================================================================================================================
		float r, g, b, a;
		gridColor.getComponents(r, g, b, a);

		float x = 0;
		float y = 0;
		int n = 0;
		float xBounds;
		int xSamps = 0;
		juce::Array<juce::Point<float>> samples;

		while (x <= 1.0 + grid->pointW)
		{
			samples.add({ x * xScaling,yScaling * truncf(sin(PI * x) / depthVal) * depthVal });
			xBounds = x;
			xSamps++;
			y = 0;
			while (y <= 1.0 + grid->pointH)
			{
				grid->circleData.set(++n, {
					xScaling * x,yScaling * y,
					r,g,b, mixVal
					});

				if (x > 0)
				{
					grid->circleData.set(++n, {
						-xScaling * x,yScaling * y,
						r,g,b, mixVal
						});


					if (y > 0)
					{
						grid->circleData.set(++n, {
							-xScaling * x,-yScaling * y,
							r,g,b, mixVal
							});
					}
				}
				if (y > 0)
				{
					grid->circleData.set(++n, {
						xScaling * x,-yScaling * y,
						r,g,b, mixVal
						});
				}

				y += depthVal;
			}
			x += rateVal;
		}



		//================================================================================================================================================================================================
		color.getInterpolatedComponents(r, g, b, a, background, 1 - mixVal);


		auto p = line->linePoints.getFirst();
		int mid = 2 * xSamps - 1;
		for (int i = 0; i < xSamps; ++i)
		{
			x = samples[i].x;
			y = samples[i].y;
			p->set(mid + 2 * i, {
				x, y,
				r,g,b, 1.0
				});
			p->set(mid - 1 - 2 * i, {
				-x, -y,
				r,g,b, 1.0
				});

			if (i + 1 < xSamps)
			{
				y = samples[i + 1].y;
				p->set(mid + 2 * i + 1, {
					x,y,
					r,g,b, 1.0
					});
				p->set(mid - 1 - 2 * i - 1, {
					-x,-y,
					r,g,b, 1.0
					});
			}

		}

		p->resizeByVertex(4 * (xSamps - 1) + 2);


		//================================================================================================================================================================================================

		color.getComponents(r, g, b, a);

		p = dry->linePoints.getFirst();
		int num = p->getNumVerticies();
		for (int i = 0; i < num; ++i)
		{
			p->set(i, {
				dryPoints[i].x, yScaling * dryPoints[i].y,
				r,g,b, 1 - mixVal
				});
		}



		grid->circleData.resize(n);
		grid->generateCircleVerticies();

		line->generateLineTriangles();

		dry->generateLineTriangles();



		//OpenGLComponent::generateVertexBuffer();
	}





	//================================================================================================================================================================================================



	BitCrushVisualizer::BitCrushVisualizer(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider)
		:OpenGlComponentWrapper<BitCrushVisualizerInternal>(universalsToUse, bitDepthSlider, sampleRateSlider, mixSlider)
	{
	}





} // namespace bdsp
