#include "bdsp_EQVisualizer.h"

namespace bdsp
{


	bdsp::EQVisualizerInternal::EQFuncitonVisualizer::EQFuncitonVisualizer(EQVisualizerInternal* parent, dsp::ParametricEQ<float>* EQToUse, int numSamples)
		:OpenGLFunctionVisualizer(parent->universals, false, true, numSamples)
	{
		p = parent;
		setBipolar(false);
		eq = EQToUse;


		newFrameInit();

	}


	EQVisualizerInternal::EQFuncitonVisualizer::~EQFuncitonVisualizer()
	{
	}

	void EQVisualizerInternal::EQFuncitonVisualizer::resized()
	{
		if (getWidth() > 0)
		{
			OpenGLFunctionVisualizer::resized();
			newFrameInit();
		}
	}

	void EQVisualizerInternal::EQFuncitonVisualizer::newFrameInit()
	{
		for (int i = 0; i < eq->getNumProcessors(); ++i)
		{
			eq->getBand(i)->visualizerParameters->frequency = freqSliders[i] == nullptr ? 20.0f : freqSliders[i]->getActualValue();
			eq->getBand(i)->visualizerParameters->q = qSliders[i] == nullptr ? BDSP_FILTER_DEFAULT_Q : qSliders[i]->getActualValue();
			eq->getBand(i)->visualizerParameters->gain = gainSliders[i] == nullptr ? 0.0f : gainSliders[i]->getActualValue();
		}

		mixVal = mixSlider == nullptr ? 1.0f : mixSlider->getActualValue();
		gainVal = globalGain == nullptr ? 1.0f : globalGain->getActualValue();

	}

	inline float EQVisualizerInternal::EQFuncitonVisualizer::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		float mag = 1;


		for (int i = 0; i < eq->getNumProcessors(); ++i)
		{
			auto band = eq->getBand(i);
			mag *= band->getMagnitudeForFrequency(band->visualizerParameters.get(), freqSliders.getFirst()->getNormalisableRange().convertFrom0to1(normX));
		}
		float decibels = juce::Decibels::gainToDecibels(mag);
		return juce::jmap(decibels * mixVal * gainVal, -db_scale, db_scale, -1.0f, 1.0f);

	}

	void EQVisualizerInternal::EQFuncitonVisualizer::calculateZeroLine()
	{
		zeroY = -1.0f;
	}


	//================================================================================================================================================================================================
	//================================================================================================================================================================================================






	EQVisualizerInternal::EQVisualizerHandle::EQVisualizerHandle(EQVisualizerInternal* parent)
		:OpenGLCirclePlotter(parent->universals, 1),
		normColor(parent->universals, this),
		downColor(parent->universals, this)
	{

		p = parent;


		mouseComp = std::make_unique<MouseComp>(this);


		featherRatio = 1.25;

	}



	void EQVisualizerInternal::EQVisualizerHandle::setSliders(BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* gainSlider)
	{
		freq = freqSlider;
		q = qSlider;
		gain = gainSlider;

		freq->addListener(this);
		q->addListener(this);
		gain->addListener(this);


		sliderValueChanged(nullptr);
	}

	void EQVisualizerInternal::EQVisualizerHandle::setToDefault()
	{
		freq->setValue(freq->getDefaultValue());
		q->setValue(q->getDefaultValue());
		gain->setValue(gain->getDefaultValue());
	}

	void EQVisualizerInternal::EQVisualizerHandle::resized()
	{
		//screenRadius = 15 * universals->visualizerLineThickness;


		OpenGLCirclePlotter::resized();
		mouseComp->setBounds(bdsp::shrinkRectangleToInt(getHandleBounds()));
	}

	void EQVisualizerInternal::EQVisualizerHandle::generateVertexBuffer()
	{
		if (freq != nullptr && gain != nullptr)
		{

			int border = p->constrainer.getMinimumWhenOffTheTop();

			float borderX = 2 * (border / (float)getWidth());
			float borderY = 2 * (border / (float)getHeight());


			glCenter.x = juce::jmap((float)freq->getNormalisableRange().convertTo0to1(freq->getActualValue()), -1 + borderX, 1 - borderX);
			glCenter.y = juce::jmap((float)gain->getNormalisableRange().convertTo0to1(gain->getActualValue()), -1 + borderY, 1 - borderY);
		}



		float r, g, b, a;

		if (mouseComp->isDown || mouseComp->isDragging)
		{
			downColor.getComponents(r, g, b, a);
		}
		else if (mouseComp->isOver)
		{
			normColor.getInterpolatedComponents(r, g, b, a, downColor.getColor(isEnabled()), 0.25);
		}
		else
		{
			normColor.getComponents(r, g, b, a);
		}






		circleData.set(0, {
			glCenter.x, glCenter.y,
			r,g,b,1
			});


		generateCircleVerticies();
	}


	void EQVisualizerInternal::EQVisualizerHandle::setColor(const NamedColorsIdentifier& norm, const NamedColorsIdentifier& down, const OpenGLColor& bkgd)
	{
		normColor.setColors(norm, bkgd.getColorID(false).mixedWith(norm, universals->disabledAlpha));
		downColor.setColors(down, bkgd.getColorID(false).mixedWith(down, universals->disabledAlpha));
	}

	juce::Point<float> EQVisualizerInternal::EQVisualizerHandle::getGLCenter()
	{
		return glCenter;
	}

	juce::Point<float> EQVisualizerInternal::EQVisualizerHandle::compPointToGLPoint(juce::Point<float> compPoint)
	{
		float x = 2 * ((compPoint.x - getX()) / getWidth()) - 1;
		float y = -(2 * ((compPoint.y - getY()) / getHeight()) - 1);

		return juce::Point<float>(x, y);
	}

	juce::Point<float> EQVisualizerInternal::EQVisualizerHandle::GLPointToCompPoint(juce::Point<float> glPoint)
	{
		float x = getX() + (glPoint.x + 1) / 2 * getWidth();
		float y = getY() + (1 - (glPoint.y + 1) / 2) * getHeight();
		return juce::Point<float>(x, y);
	}

	juce::Rectangle<float> EQVisualizerInternal::EQVisualizerHandle::compBoundsToGLBounds(juce::Rectangle<float> compBounds)
	{
		return juce::Rectangle<float>(compPointToGLPoint(compBounds.getPosition()), compPointToGLPoint(compBounds.getBottomRight()));
	}

	juce::Rectangle<float> EQVisualizerInternal::EQVisualizerHandle::GLBoundsToCompBounds(juce::Rectangle<float> glBounds)
	{
		return juce::Rectangle<float>(GLPointToCompPoint(glBounds.getPosition()), GLPointToCompPoint(glBounds.getBottomRight()));
	}

	juce::Rectangle<float> EQVisualizerInternal::EQVisualizerHandle::getHandleBounds()
	{
		auto glBounds = juce::Rectangle<float>(4 * featherW, 4 * featherH).withCentre(glCenter);
		return GLBoundsToCompBounds(glBounds);
	}

	void EQVisualizerInternal::EQVisualizerHandle::setParamsFromGLCenter()
	{

		float f = juce::jlimit(0.0f, 1.0f, juce::jmap(glCenter.x, -1 + p->borderX, 1 - p->borderX, 0.0f, 1.0f));
		float g = juce::jlimit(0.0f, 1.0f, juce::jmap(glCenter.y, -1 + p->borderY, 1 - p->borderY, 0.0f, 1.0f));
		//float f = (glCenter.x + 1) / 2;
		//float g = (glCenter.y + 1) / 2;

		freq->setValue(freq->getNormalisableRange().convertFrom0to1(f));
		gain->setValue(gain->getNormalisableRange().convertFrom0to1(g));

	}

	void EQVisualizerInternal::EQVisualizerHandle::selected()
	{
		p->handleSelected(this);
		if (p->onHandleSelected.operator bool())
		{
			int idx = p->subClasses.indexOf(this) - 1;
			p->onHandleSelected(idx);
		}
	}


	void EQVisualizerInternal::EQVisualizerHandle::sliderValueChanged(juce::Slider* slider)
	{

		juce::MessageManagerLock mml;
		int border = p->constrainer.getMinimumWhenOffTheTop();

		//borderColor = borderColor == 0 ? screenRadius : borderColor;
		float borderX = 2 * (border / (float)getWidth());


		glCenter.x = juce::jmap((float)freq->getNormalisableRange().convertTo0to1(freq->getActualValue()), -1 + borderX, 1 - borderX);
		glCenter.y = juce::jmap((float)gain->getNormalisableRange().convertTo0to1(gain->getActualValue()), -1 + borderX, 1 - borderX);

		mouseComp->setBounds(bdsp::shrinkRectangleToInt(getHandleBounds()));


	}



	//================================================================================================================================================================================================



	EQVisualizerInternal::EQVisualizerInternal(GUI_Universals* universalsToUse, dsp::ParametricEQ<float>* EQToUse, int numSamples)
		:OpenGLCompositeComponent(universalsToUse)
	{
		subClasses.add(new EQFuncitonVisualizer(this, EQToUse, numSamples));
		for (int i = 0; i < EQToUse->getNumProcessors(); ++i)
		{
			subClasses.add(new EQVisualizerHandle(this));

			handlePointers.add(dynamic_cast<EQVisualizerHandle*>(subClasses.getLast()));
		}

		subClasses.add(new OpenGLRing(universals));
		ringPointer = dynamic_cast<OpenGLRing*>(subClasses.getLast());

		resized();
		initSubClasses();
	}

	void EQVisualizerInternal::setSliders(int band, BaseSlider* freqSlider, BaseSlider* qSlider, BaseSlider* gainSlider)
	{
		auto cast = dynamic_cast<EQFuncitonVisualizer*>(subClasses.getFirst());

		cast->freqSliders.set(band, freqSlider);
		cast->qSliders.set(band, qSlider);
		cast->gainSliders.set(band, gainSlider);

		cast->db_scale = 1.5 * juce::Decibels::gainToDecibels(gainSlider->getNormalisableRange().convertFrom0to1(1));

		//resized();
		handlePointers[band]->setSliders(freqSlider, qSlider, gainSlider);
	}

	void EQVisualizerInternal::setGlobalSliders(BaseSlider* gainSlider, BaseSlider* mixSlider)
	{
		auto cast = dynamic_cast<EQFuncitonVisualizer*>(subClasses.getFirst());
		cast->globalGain = gainSlider;
		cast->mixSlider = mixSlider;
	}



	void EQVisualizerInternal::generateVertexBuffer()
	{


		ringPointer->setCenter(handlePointers[selectedHandle]->getGLCenter());


		for (auto c : subClasses)
		{
			c->generateVertexBuffer();
		}
	}

	void EQVisualizerInternal::setScaling(float x, float y)
	{
		dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getFirst())->setScaling(x, y);
	}

	void EQVisualizerInternal::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float top, float bot)
	{
		auto cast = dynamic_cast<EQFuncitonVisualizer*>(subClasses.getFirst());
		cast->setColor(c, line, top, bot);

		for (auto* h : handlePointers)
		{
			h->setColor(c.withMultipliedSaturation(1 - universals->buttonDesaturation), c, cast->getBackgroundColor());
		}

		ringPointer->setColor(c);
	}

	void EQVisualizerInternal::resized()
	{


		auto handleSize = 15 * universals->visualizerLineThickness;
		constrainer.setMinimumOnscreenAmounts(handleSize, handleSize, handleSize, handleSize);

		borderX = 2 * (handleSize / (float)getWidth());
		borderY = 2 * (handleSize / (float)getHeight());

		ringPointer->setRadius(2 * handleSize);
		ringPointer->setThickness(universals->visualizerLineThickness);

		auto cast = dynamic_cast<OpenGLFunctionVisualizer*>(subClasses.getFirst());

		for (auto h : handlePointers)
		{
			h->setRadius(handleSize);
		}
		OpenGLCompositeComponent::resized();
		setScaling(1 - borderX + cast->lineFeatherX[0] + cast->lineThicknessX[0], 1);


	}

	void EQVisualizerInternal::handleSelected(EQVisualizerHandle* handleSelected)
	{
		selectedHandle = handlePointers.indexOf(handleSelected);

	}









	EQVisualizerInternal::EQVisualizerHandle::MouseComp::MouseComp(EQVisualizerHandle* p)
		:Component(p->universals)
	{
		parent = p;
		p->p->addAndMakeVisible(this);
		setAlwaysOnTop(true);
		/*	p->p->addMouseListener(this, true);*/
	}

	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseEnter(const juce::MouseEvent& e)
	{

		isOver = true;
		setMouseCursor(juce::MouseCursor::DraggingHandCursor);

	}


	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseExit(const juce::MouseEvent& e)
	{

		isOver = false;
		setMouseCursor(juce::MouseCursor::NormalCursor);

	}


	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseDown(const juce::MouseEvent& e)
	{
		if (e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::SingleClickReset] || e.mods.isMiddleButtonDown())
		{
			mouseDoubleClick(e);
		}
		else
		{
			isDown = true;
			parent->selected();
			startDraggingComponent(this, e);
		}
	}

	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseDoubleClick(const juce::MouseEvent& e)
	{

		parent->setToDefault();
		parent->selected();

	}


	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseUp(const juce::MouseEvent& e)
	{
		isDown = false;
		isDragging = false;
	}

	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseDrag(const juce::MouseEvent& e)
	{
		if (isDown)
		{


			dragComponent(this, e, &parent->p->constrainer);

			isDragging = true;

			parent->glCenter = parent->compBoundsToGLBounds(getBounds().toFloat()).getCentre();
			parent->setParamsFromGLCenter();

		}
	}

	void EQVisualizerInternal::EQVisualizerHandle::MouseComp::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& mw)
	{

		parent->selected();

		float mwDelta = mw.deltaY;
		if (e.mods.withoutMouseButtons().getRawFlags() == universals->bindings[SliderKeyBindings::SingleClickFineControl])
		{
			mwDelta /= BDSP_FINE_MOUSE_WHEEL_REDUCTION_FACTOR;
		}
		float delta = (mw.isReversed ? -1 : 1) * mwDelta;
		auto q = parent->q;


		q->setValue(q->getNormalisableRange().convertFrom0to1(juce::jlimit(0.0,1.0,q->getNormalisableRange().convertTo0to1(q->getValue()) + delta)));
	}



} // namespace bdsp
