#include "bdsp_PitchShifterVisualizer.h"

namespace bdsp
{
	PitchShifterVisualizerInternal::PitchShifterVisualizerInternal(GUI_Universals* universalsToUse, Parameter* leftAmt, Parameter* rightAmt, Parameter* mixAmt)
		:OpenGLCompositeComponent(universalsToUse),
		color(universalsToUse, this),
		scaleColor(universalsToUse, this)
	{
		left = leftAmt;
		right = rightAmt;
		mix = mixAmt;

		subClasses.add(new OpenGLLineRenderer(universals, { 2,2,2,2,2,2,2,2,2,2 })); //scale
		subClasses.add(new OpenGLLineRenderer(universals, { numPoints,numPoints,numPoints,numPoints })); // trigTails
		subClasses.add(new OpenGLCircleRenderer(universals, 2)); // thumbs
		subClasses.add(new OpenGLCircleRenderer(universals, 2)); // dryThumbs

		scale = dynamic_cast<OpenGLLineRenderer*>(subClasses[0]);
		trigTails = dynamic_cast<OpenGLLineRenderer*>(subClasses[1]);
		thumbs = dynamic_cast<OpenGLCircleRenderer*>(subClasses[2]);
		dryThumbs = dynamic_cast<OpenGLCircleRenderer*>(subClasses[3]);

		initSubClasses();

		trigTails->setThicknessRamp(-1, 0);
		trigTails->setCapType(-1, OpenGLLineRenderer::CapType::Round);
		trigTails->setInterLineOverdraw(false);
	}


	PitchShifterVisualizerInternal::~PitchShifterVisualizerInternal()
	{
	}

	void PitchShifterVisualizerInternal::renderOpenGL()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		juce::OpenGLHelpers::clear(background);

		generateVertexBuffer();

		scale->renderWithoutGenerating();
		trigTails->renderWithoutGenerating();

        juce::gl::glClear(juce::gl::GL_STENCIL_BUFFER_BIT); // clear the stencil buffer of the scale's data
        
		// stencil test the thumbs to prevent overdraw with the tails
		juce::gl::glEnable(juce::gl::GL_STENCIL_TEST);
		juce::gl::glStencilFunc(juce::gl::GL_NOTEQUAL, 1, 0xFF); // only render fragments not rendered by the previous shader pass
		thumbs->renderWithoutGenerating();

		juce::gl::glDisable(juce::gl::GL_STENCIL_TEST);
		dryThumbs->renderWithoutGenerating();
	}

	void PitchShifterVisualizerInternal::paintOverChildren(juce::Graphics& g)
	{
		g.setColour(scaleColor.getColor(isEnabled()));
		float sideLength = 0.8*getWidth() * gap;
		juce::Rectangle<float> bounds = juce::Rectangle<float>(sideLength, sideLength);
		float centerX = getWidth() / 2.0f;

		auto font = universals->Fonts[getFontIndex()].getFont().withHeight(sideLength);
		drawText(g, font, "+12", bounds.withCentre({ centerX,getHeight() * (-scaleY + 1) / 2.0f }));
		drawText(g, font, "+7", bounds.withCentre({ centerX,getHeight() * (-scaleY * 7.0f / 12.0f + 1) / 2.0f }));

		drawText(g, font, "0", bounds.withCentre({ centerX, getHeight() / 2.0f }));

		drawText(g, font, "-7", bounds.withCentre({ centerX,getHeight() * (scaleY * 7.0f / 12.0f + 1) / 2.0f }));
		drawText(g, font, "-12", bounds.withCentre({ centerX,getHeight() * (scaleY + 1) / 2.0f }));
	}



	void PitchShifterVisualizerInternal::setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newScaleColor)
	{
		color.setColors(newColor, newColor.withMultipliedAlpha(universals->disabledAlpha));
		scaleColor.setColors(newScaleColor, newScaleColor.withMultipliedAlpha(universals->disabledAlpha));
	}

	void PitchShifterVisualizerInternal::linkAmounts(bool shouldLink)
	{
		amountsLinked = shouldLink;
	}

	void PitchShifterVisualizerInternal::resized()
	{
		scale->setThickness(-1, universals->visualizerLineThickness / 4.0);
		trigTails->setThickness(-1, universals->visualizerLineThickness);
		OpenGLCompositeComponent::resized();
	}


	void PitchShifterVisualizerInternal::generateVertexBuffer()
	{
		time = fmodf(time + 0.025, 2 * PI);
		shaderProgram->use();

		//frequencies multipliers for each stereo channel
		float fL = juce::mapToLog10((double)left->convertTo0to1(left->getActualValue()), 0.5, 2.0);
		float fR = juce::mapToLog10((double)right->convertTo0to1(right->getActualValue()), 0.5, 2.0);

		leftVal = juce::jmap((double)left->convertTo0to1(left->getActualValue()), -1.0, 1.0);
		if (amountsLinked)
		{
			rightVal = leftVal;
			fR = fL;
		}
		else
		{
			rightVal = juce::jmap((double)right->convertTo0to1(right->getActualValue()), -1.0, 1.0);
		}

		mixVal = mix->getActualValue();


		float r, g, b, a;
		color.getComponents(r, g, b, a);
		float dryA = (1 - mixVal) * a;
		a *= mixVal;

		float mid = (scaleX + gap) / 2.0; // x-midpoint of the right channel scale

		float w = (1.0f - scaleY) / 3.0f;
		leftVal *= scaleY;
		rightVal *= scaleY;
		float wL = w * leftVal;
		float wR = w * rightVal;
		float dim = juce::jmin(getWidth(), getHeight());
		float radL = abs(wL * dim);
		float radR = abs(wR * dim);
		float radDry = w * (1 - mixVal) * dim*scaleY;

		thumbs->circleVertexBuffer.set(0, { -mid,leftVal,radL,radL,r,g,b,a });
		thumbs->circleVertexBuffer.set(1, { mid,rightVal,radR,radR,r,g,b,a });

		float gray = (r + g + b) / 3.0;
		dryThumbs->circleVertexBuffer.set(0, { -mid,0,radDry,radDry,gray,gray,gray,dryA });
		dryThumbs->circleVertexBuffer.set(1, { mid,0,radDry,radDry,gray,gray,gray,dryA });
		//================================================================================================================================================================================================
		for (int i = 0; i < numPoints; ++i)
		{
			float prop = i / (numPoints - 1.0f);
			float alpha = a * prop;
			float yL = prop * leftVal;
			float yR = prop * rightVal;
			float xL = wL/2 * sin(2 * fL * PI * yL + fL * time);
			float xR = wR/2 * sin(2 * fR * PI * yR + fR * time);
			trigTails->lineVertexBuffer[0]->set(i, { -mid + xL,yL,r,g,b,alpha });
			trigTails->lineVertexBuffer[1]->set(i, { -mid - xL,yL,r,g,b,alpha });
			trigTails->lineVertexBuffer[2]->set(i, { mid + xR,yR,r,g,b,alpha });
			trigTails->lineVertexBuffer[3]->set(i, { mid - xR,yR,r,g,b,alpha });
		}
		//================================================================================================================================================================================================
		float fifthY = scaleY * 7.0f / 12.0f;
		scaleColor.getComponents(r, g, b, a);
		scale->lineVertexBuffer[0]->set(0, { -scaleX,scaleY,r,g,b,a });
		scale->lineVertexBuffer[0]->set(1, { -gap,scaleY,r,g,b,a });

		scale->lineVertexBuffer[1]->set(0, { gap,scaleY,r,g,b,a });
		scale->lineVertexBuffer[1]->set(1, { scaleX,scaleY,r,g,b,a });

		scale->lineVertexBuffer[2]->set(0, { -scaleX,fifthY,r,g,b,a });
		scale->lineVertexBuffer[2]->set(1, { -gap,fifthY,r,g,b,a });

		scale->lineVertexBuffer[3]->set(0, { gap,fifthY,r,g,b,a });
		scale->lineVertexBuffer[3]->set(1, { scaleX,fifthY,r,g,b,a });


		scale->lineVertexBuffer[4]->set(0, { -scaleX,0,r,g,b,a });
		scale->lineVertexBuffer[4]->set(1, { -gap,0,r,g,b,a });

		scale->lineVertexBuffer[5]->set(0, { gap,0,r,g,b,a });
		scale->lineVertexBuffer[5]->set(1, { scaleX,0,r,g,b,a });


		scale->lineVertexBuffer[6]->set(0, { -scaleX,-fifthY,r,g,b,a });
		scale->lineVertexBuffer[6]->set(1, { -gap,-fifthY,r,g,b,a });

		scale->lineVertexBuffer[7]->set(0, { gap,-fifthY,r,g,b,a });
		scale->lineVertexBuffer[7]->set(1, { scaleX,-fifthY,r,g,b,a });

		scale->lineVertexBuffer[8]->set(0, { -scaleX,-scaleY,r,g,b,a });
		scale->lineVertexBuffer[8]->set(1, { -gap,-scaleY,r,g,b,a });

		scale->lineVertexBuffer[9]->set(0, { gap,-scaleY,r,g,b,a });
		scale->lineVertexBuffer[9]->set(1, { scaleX,-scaleY,r,g,b,a });
	}

} // namespace bdsp
