#include "bdsp_SequencerVisualizer.h"

namespace bdsp
{
	SequencerVisualizerInternal::SequencerVisualizerInternal(GUI_Universals* universalsToUse, SequencerComponent* parentComp)
		:OpenGLFunctionVisualizer(universalsToUse, false, true, 25 * BDSP_SEQUENCER_STEPS + 1),
		GUI_Universals::Listener(universalsToUse)
	{
		controller = dynamic_cast<SequencerControllerObject*>(parentComp->control);
		setBipolar(false);
		setJointType(0, JointType::Rounded);
		setCapType(0, CapType::Round);

		parentComponent = parentComp;

		setDividerColor(BDSP_COLOR_BLACK);

		vertexBuffer.init(4 * BDSP_SEQUENCER_STEPS / 2);

		for (int i = 0; i < BDSP_SEQUENCER_STEPS / 2; ++i)
		{
			indexBuffer.addArray({ 4 * i, 4 * i + 1, 4 * i + 2, 4 * i + 1, 4 * i + 2, 4 * i + 3 });
		}

		fontIndex = 1;

		vertsPerStep = (lineVertexBuffer.getFirst()->getNumVerticies() - 1) / BDSP_SEQUENCER_STEPS;


	}

	SequencerVisualizerInternal::~SequencerVisualizerInternal()
	{
		//universals->contextHolder->unregisterOpenGlRenderer(this);
		openGLContextClosing();
	}


	inline float SequencerVisualizerInternal::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float p = normX * BDSP_SEQUENCER_STEPS;

		int step = static_cast<int>(p);
		float prop = p - step;
		float v;
		if (step >= BDSP_SEQUENCER_STEPS)
		{
			v = controller->getValueForShape(SequencerShapes::Empty, 0);
		}
		else
		{
			v = controller->getAmtForStep(step) * controller->getValueForShape(controller->getShapeForStep(step), prop);
		}


		return juce::jmap(v, 0.0f, 1.0f, 2.0f * (numbersProportion)-1.0f, 1.0f);
	}

	inline void SequencerVisualizerInternal::newFrameInit()
	{
		if (fillShaderProgram.operator bool())
		{
			progress = controller->getProgress() / BDSP_SEQUENCER_STEPS;
			setFillPos(progress);

			progressIncrement = controller->getProgressIncrement();
			auto posWidth = 0.4f * powf(progressIncrement / 0.4f, 0.25f);
			width->set(posWidth);

			int steps = controller->getNumStep();

			if (steps == BDSP_SEQUENCER_STEPS || !shapeStartsVertical(controller->getShapeForStep(steps)))
			{
				setEndpoint((float)steps / BDSP_SEQUENCER_STEPS);
			}
			else
			{
				setEndpoint((float)steps / BDSP_SEQUENCER_STEPS - scalingX * lineScreenThickness.getFirst() / getWidth());
			}

		}
	}

	void SequencerVisualizerInternal::mouseEnter(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::PointingHandCursor);
	}

	void SequencerVisualizerInternal::mouseExit(const juce::MouseEvent& e)
	{
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}

	void SequencerVisualizerInternal::mouseDown(const juce::MouseEvent& e)
	{
		int step = floor(BDSP_SEQUENCER_STEPS * e.position.x / getWidth());
		step = juce::jlimit(0, BDSP_SEQUENCER_STEPS - 1, step);
		auto* shapeParam = controller->getShapeParameter(step);
		shapeParam->beginChangeGesture();
		if (e.mods.testFlags(universals->bindings[SingleClickReset]) || e.mods.isRightButtonDown())
		{
			shapeParam->setValueNotifyingHost(0);
		}
		else
		{
			shapeParam->setValueNotifyingHost(shapeParam->convertTo0to1(parentComponent->getParentSection()->getSelectedShape()));
		}

		shapeParam->endChangeGesture();

		auto* amtParam = controller->getAmtParameter(step);
		amtParam->beginChangeGesture();
		amtParam->setValueNotifyingHost(juce::jmap(1.0f - e.position.y / getHeight(), 1.0f - numbersProportion, 1.0f, 0.0f, 1.0f));
		amtParam->endChangeGesture();


		lastMousePos = e.position;
	}

	void SequencerVisualizerInternal::mouseDrag(const juce::MouseEvent& e)
	{
		int dir = e.position.x > lastMousePos.x ? 1 : -1;
		float stepW = (float)getWidth() / BDSP_SEQUENCER_STEPS;

		float x = lastMousePos.x;
		while (isBetweenInclusive(x, lastMousePos.x, e.position.x))
		{
			juce::MouseEvent me = e.withNewPosition(juce::Point<float>(x, e.position.y));
			mouseDown(me);

			x += stepW * dir;
		}
	}

	void SequencerVisualizerInternal::setDividerColor(const NamedColorsIdentifier& color)
	{
		dividerColor = color;

		auto c = getColor(dividerColor);

		float r = c.getFloatRed();
		float g = c.getFloatGreen();
		float b = c.getFloatBlue();
		float a = c.getFloatAlpha();
		float bot = -1 + 2 * numbersProportion;

		for (int i = 0; i < BDSP_SEQUENCER_STEPS / 2; ++i)
		{
			float left = scalingX * (2.0f * (2.0f * i) / BDSP_SEQUENCER_STEPS - 1.0f);
			float right = scalingX * (2.0f * (2.0f * i + 1.0f) / BDSP_SEQUENCER_STEPS - 1.0f);
			vertexBuffer.set(4 * i, { left,1,r,g,b,a });
			vertexBuffer.set(4 * i + 1, { left,bot,r,g,b,a });
			vertexBuffer.set(4 * i + 2, { right,1,r,g,b,a });
			vertexBuffer.set(4 * i + 3, { right,bot,r,g,b,a });
		}
	}

	void SequencerVisualizerInternal::renderWithoutGenerating()
	{
		juce::gl::glStencilMask(0x00); // disable writing to the stencil buffer
		OpenGLComponent::renderWithoutGenerating();

		OpenGLFunctionVisualizer::renderWithoutGenerating();
	}

	void SequencerVisualizerInternal::generateVertexBuffer()
	{

		setDividerColor(dividerColor);
		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		fillShaderProgram->use();
		newFrameInit();
		generateZeroLine();

		auto& p = *lineVertexBuffer[0];
		for (int i = 0; i < p.getNumVerticies(); ++i)
		{

			float normX = (float)i / (BDSP_SEQUENCER_STEPS * vertsPerStep);
			int offset = i / vertsPerStep;

			float x = scalingX * (2.0f * normX - 1.0f);
			float y = scalingY * calculateFunctionSample(i, x, normX);
			p.set(i,
				{
					x, y,
					r,g,b, a
				}
			);

		}
	}

	bool SequencerVisualizerInternal::shapeStartsVertical(SequencerShapes shape)
	{
		return shape == SequencerShapes::SawDown || shape == SequencerShapes::SinDown || shape == SequencerShapes::SquareFull || shape == SequencerShapes::SquareHalf;
	}



	//================================================================================================================================================================================================

	SequencerVisualizer::SequencerVisualizer(GUI_Universals* universalsToUse, SequencerComponent* parentComp)
		:OpenGlComponentWrapper<SequencerVisualizerInternal>(universalsToUse, parentComp)
	{
	}

	void SequencerVisualizer::resized()
	{
		vis->setBounds(getLocalBounds().reduced(universals->roundedRectangleCurve, 0).withTrimmedTop(universals->roundedRectangleCurve).withTrimmedBottom(1));
	}

	void SequencerVisualizer::paint(juce::Graphics& g)
	{
		g.setColour(getColor(vis->cutoutColor));
		g.fillRect(getLocalBounds());

		auto visRect = getLocalBounds().toFloat().withHeight(vis->getHeight() * vis->numbersProportion + universals->roundedRectangleCurve);

		drawOutlinedRoundedRectangle(g, visRect, CornerCurves::top, universals->roundedRectangleCurve, universals->dividerSize, vis->getBackgroundColor().getColor(isEnabled()), getColor(outlineColor));

	}




} // namespace bdsp
