#include "bdsp_PitchShifterVisualizer.h"

namespace bdsp
{


	PitchShifterVisualizer::PitchShifterVisualizer::PitchShifterVisualizerInternal::PitchShifterVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* leftAmt, BaseSlider* rightAmt, BaseSlider* mixAmt)
		:OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this),
		scaleColor(universalsToUse, this),
		textColor(universalsToUse, this)
	{
		left = leftAmt;
		right = rightAmt;
		mix = mixAmt;

		subClasses.add(new OpenGLLineRenderer(universals, { 2,2,2,2,2,2,2,2,2,2 }));
		subClasses.add(new OpenGLLineRenderer(universals, { 2,2 }));
		subClasses.add(new OpenGLCirclePlotter(universals, 4));

		initSubClasses();

		scale = dynamic_cast<OpenGLLineRenderer*>(subClasses[0]);
		bars = dynamic_cast<OpenGLLineRenderer*>(subClasses[1]);
		dots = dynamic_cast<OpenGLCirclePlotter*>(subClasses[2]);



	}

	PitchShifterVisualizer::PitchShifterVisualizerInternal::~PitchShifterVisualizerInternal()
	{
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::resized()
	{
		auto barW = barSize > 0 ? barSize * getWidth() : universals->visualizerLineThickness;
		auto scaleW = scaleSize > 0 ? scaleSize * getWidth() : universals->visualizerLineThickness;
		auto dotW = dotSize > 0 ? dotSize * getWidth() : 2 * universals->visualizerLineThickness;

		dots->setRadius(dotW);

		for (int i = 0; i < scale->lineVertexBuffer.size(); i++)
		{
			scale->setThickness(i, scaleW);
		}
		for (int i = 0; i < bars->lineVertexBuffer.size(); i++)
		{
			bars->setThickness(i, barW);
		}

		OpenGLCompositeComponent::resized();

		scaleY = (getHeight() - 2 * dotW) / getHeight();
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newScaleColor, const NamedColorsIdentifier& newTextColor)
	{

		color.setColors(newColor, newColor.mixedWith(background.getColorID(false), 1 - universals->disabledAlpha));
		scaleColor.setColors(newScaleColor, newScaleColor.mixedWith(background.getColorID(false), 1 - universals->disabledAlpha));
		textColor.setColors(newTextColor, newTextColor.mixedWith(background.getColorID(false), 1 - universals->disabledAlpha));
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::linkAmounts(bool shouldLink)
	{
		amountsLinked = shouldLink;
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::setBarWidth(float barRatio, float scaleRatio)
	{
		barSize = barRatio;
		scaleSize = scaleRatio;
		resized();
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::setDotSize(float dotRatio)
	{
		dotSize = dotRatio;
		resized();
	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::paintOverChildren(juce::Graphics& g)
	{
		g.setColour(textColor.getColor(isEnabled()));
		juce::Rectangle<float> bounds = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0, 0, 1, 1.75 * dotSize));


		drawText(g, universals->Fonts[getFontIndex()].getFont(), "+12", bounds.withCentre({ bounds.getCentreX(),getHeight() * (-scaleY + 1) / 2.0f }), true);
		drawText(g, universals->Fonts[getFontIndex()].getFont(), "+7", bounds.withCentre({ bounds.getCentreX(),getHeight() * (-scaleY * 7.0f / 12.0f + 1) / 2.0f }), true);

		drawText(g, universals->Fonts[getFontIndex()].getFont(), "0", bounds.withCentre({ bounds.getCentreX(), getHeight() / 2.0f }), true);

		drawText(g, universals->Fonts[getFontIndex()].getFont(), "-7", bounds.withCentre({ bounds.getCentreX(),getHeight() * (scaleY * 7.0f / 12.0f + 1) / 2.0f }), true);
		drawText(g, universals->Fonts[getFontIndex()].getFont(), "-12", bounds.withCentre({ bounds.getCentreX(),getHeight() * (scaleY + 1) / 2.0f }), true);

	}


	void PitchShifterVisualizer::PitchShifterVisualizerInternal::generateVertexBuffer()
	{
		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());
		leftVal = juce::jmap((float)left->getNormalisableRange().convertTo0to1(left->getActualValue()), -scaleY, scaleY);

		if (amountsLinked)
		{
			rightVal = leftVal;
		}
		else
		{
			rightVal = juce::jmap((float)right->getNormalisableRange().convertTo0to1(right->getActualValue()), -scaleY, scaleY);
		}


		float red, green, blue, a;

		scaleColor.getComponents(red, green, blue, a);


		float y;
		float in = gap / 2;

		for (int i = 0; i < 2; i++)
		{
			y = scaleY;
			float l = i % 2 == 0 ? -scaleX : in;
			float r = i % 2 == 0 ? -in : scaleX;
			auto p = scale->lineVertexBuffer[5 * i];
			p->set(0, {
				l,0,
				red, green, blue,1.0
				});
			p->set(1, {
				r,0,
				red, green, blue,1.0
				});

			//================================================================================================================================================================================================
			p = scale->lineVertexBuffer[5 * i + 1];
			p->set(0, {
				l,y,
				red, green, blue,1.0
				});
			p->set(1, {
				r,y,
				red, green, blue,1.0
				});

			//================================================================================================================================================================================================
			p = scale->lineVertexBuffer[5 * i + 2];
			p->set(0, {
				l,-y,
				red, green, blue,1.0
				});
			p->set(1, {
				r,-y,
				red, green, blue,1.0
				});

			//================================================================================================================================================================================================
			y *= 7.0 / 12.0;
			p = scale->lineVertexBuffer[5 * i + 3];
			p->set(0, {
				l,y,
				red, green, blue,1.0
				});
			p->set(1, {
				r,y,
				red, green, blue,1.0
				});

			//================================================================================================================================================================================================
			p = scale->lineVertexBuffer[5 * i + 4];
			p->set(0, {
				l,-y,
				red, green, blue,1.0
				});
			p->set(1, {
				r,-y,
				red, green, blue,1.0
				});

		}

		//================================================================================================================================================================================================
		float out = scaleX;

		//float rectAlphaL = mixVal * (abs(leftVal) > 2 * dots->pointH ? 1.0f : 0.0f);
		//float rectAlphaR = mixVal * (abs(rightVal) > 2 * dots->pointH ? 1.0f : 0.0f);

		float x = (in + out) / 2;


		color.getComponents(red, green, blue, a);

		float backR, backG, backB;
		background.getComponents(backR, backG, backB, a);

		float dryRed = juce::jmap(1 - mixVal, backR, red);
		float dryGreen = juce::jmap(1 - mixVal, backG, green);
		float dryBlue = juce::jmap(1 - mixVal, backB, blue);


		red = juce::jmap(mixVal, backR, red);
		green = juce::jmap(mixVal, backG, green);
		blue = juce::jmap(mixVal, backB, blue);

		//================================================================================================================================================================================================

		auto& cd = dots->circleData;
		if (mixVal >= 0.5f)
		{

			cd.set(0, {
				-x,0,
				dryRed,dryGreen,dryBlue,1,
				});

			cd.set(1, {
				x,0,
				dryRed,dryGreen,dryBlue,1,
				});


			cd.set(2, {
				-x,leftVal,
				red, green, blue, 1
				});

			cd.set(3, {
				x,rightVal,
				red, green, blue, 1
				});
		}
		else
		{
			cd.set(0, {
				-x,leftVal,
				red, green, blue, 1
				});

			cd.set(1, {
				x,rightVal,
				red, green, blue, 1
				});

			cd.set(2, {
				-x,0,
				dryRed,dryGreen,dryBlue,1,
				});

			cd.set(3, {
				x,0,
				dryRed,dryGreen,dryBlue,1,
				});


		}



		//================================================================================================================================================================================================


		auto p = bars->lineVertexBuffer[0];
		p->set(0, {
			-x, 0,
			0, 0, 0, 0
			});

		p->set(1, {
			-x, leftVal  ,
			red, green, blue, 1
			});



		//================================================================================================================================================================================================

		p = bars->lineVertexBuffer[1];

		p->set(0, {
			x, 0,
			0, 0, 0, 0
			});

		p->set(1, {
			x, rightVal  ,
			red, green, blue, 1
			});





		//================================================================================================================================================================================================
		dots->generateCircleVerticies();

	}

	void PitchShifterVisualizer::PitchShifterVisualizerInternal::onShaderCreation()
	{
	}





	//================================================================================================================================================================================================


	PitchShifterVisualizer::PitchShifterVisualizer(GUI_Universals* universalsToUse, BaseSlider* leftAmt, BaseSlider* rightAmt, BaseSlider* mixAmt)
		:Component(universalsToUse),
		vis(universalsToUse, leftAmt, rightAmt, mixAmt)
	{
		addAndMakeVisible(vis);
	}

	void PitchShifterVisualizer::resized()
	{
		vis.setBounds(getLocalBounds().reduced(universals->roundedRectangleCurve));
	}

	void PitchShifterVisualizer::paint(juce::Graphics& g)
	{
		g.setColour(vis.getBackgroundColor());
		g.fillRoundedRectangle(getLocalBounds().toFloat(), universals->roundedRectangleCurve);
	}

	void PitchShifterVisualizer::setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newDryColor)
	{
		vis.setColor(newColor, newDryColor);
	}

	void PitchShifterVisualizer::linkAmounts(bool shouldLink)
	{
		vis.linkAmounts(shouldLink);
	}

	void PitchShifterVisualizer::setBarWidth(float barRatio, float scaleRatio)
	{
		vis.setBarWidth(barRatio, scaleRatio);
	}

	void PitchShifterVisualizer::setDotSize(float dotRatio)
	{
		vis.setDotSize(dotRatio);
	}

	void PitchShifterVisualizer::setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind)
	{
		vis.setBackgroundColor(bkgd, bkgdBehind.mixedWith(&bkgd, universals->disabledAlpha));
	}

} // namespace bdsp