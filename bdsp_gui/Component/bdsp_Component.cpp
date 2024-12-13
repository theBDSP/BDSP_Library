#include "bdsp_Component.h"

namespace bdsp
{

	HoverableComponent::HoverableComponent(juce::Component* compToExtend)
		:t(this)
	{
		extendedComponent = compToExtend;
		extendedComponent->addMouseListener(&t, true);
	}

	juce::Component* HoverableComponent::getComponent()
	{
		return extendedComponent;
	}

	void HoverableComponent::setHoverTime(int ms)
	{
		t.setTime(ms);
	}

	void HoverableComponent::setHoverFunc(std::function<void(bool)>& func)
	{
		t.hoverFunc = func;
	}

	std::function<void(bool)>& HoverableComponent::getHoverFunc()
	{
		return t.hoverFunc;
	}


	int HoverableComponent::getHoverTime()
	{
		return t.getTime();
	}

	bool HoverableComponent::isHovering()
	{
		return t.isHovering() && extendedComponent->isEnabled();
	}

	void HoverableComponent::setHovering(bool state, bool notifyListeners, bool notifyPartners, HoverableComponent* excludedPartner)
	{
		t.setHovering(state, notifyListeners, notifyPartners, excludedPartner);
		extendedComponent->repaint();
	}



	bool HoverableComponent::isHoverPartner(HoverableComponent* compToCheck)
	{
		return hoverPartners.contains(compToCheck);
	}

	bool HoverableComponent::isMouseWithin()
	{
		return t.isMouseWithin();
	}


	void HoverableComponent::addHoverListener(Listener* listenerToAdd)
	{
		listeners.addIfNotAlreadyThere(listenerToAdd);
	}

	void HoverableComponent::removeHoverListener(Listener* listenerToRemove)
	{
		listeners.removeAllInstancesOf(listenerToRemove);
	}

	void HoverableComponent::removeAllHoverListeners()
	{
		listeners.clear();
	}

	void HoverableComponent::addHoverPartner(HoverableComponent* otherComp, bool initialAdd)
	{
		hoverPartners.addIfNotAlreadyThere(otherComp);

		if (initialAdd)
		{
			otherComp->addHoverPartner(this, false);
		}
	}

	void HoverableComponent::addHoverPartners(const juce::Array<juce::WeakReference<HoverableComponent>>& otherComps, bool initialAdd)
	{
		for (auto p : otherComps)
		{
			if (p != nullptr)
			{
				hoverPartners.addIfNotAlreadyThere(p);
			}
			else
			{
				removeHoverPartner(p);
			}
		}
		if (initialAdd)
		{

			for (auto p : otherComps)
			{
				if (p != nullptr)
				{
					auto arr = otherComps;
					arr.removeAllInstancesOf(p);
					arr.add(this);
					p->addHoverPartners(arr, false);
				}
			}
		}
	}

	void HoverableComponent::removeHoverPartner(HoverableComponent* otherComp, bool removeFromAllOtherPartners)
	{
		hoverPartners.removeAllInstancesOf(otherComp);

		if (removeFromAllOtherPartners)
		{
			for (auto p : hoverPartners)
			{
				p->removeHoverPartner(otherComp, false);
			}
		}
	}

	void HoverableComponent::removeAllHoverPartners()
	{
		for (auto p : hoverPartners)
		{
			p->hoverPartners.removeAllInstancesOf(this);
		}
		hoverPartners.clear();
	}

	//================================================================================================================================================================================================

	HoverableComponent::HoverTimer::HoverTimer(HoverableComponent* parent)
	{
		p = parent;
	}




	void HoverableComponent::HoverTimer::timerCallback()
	{
		if (hoverFunc.operator bool())
		{
			hoverFunc(hovering);
		}
		//notifyParentsListeners();

		stopTimer();
	}


	void HoverableComponent::HoverTimer::setTime(int ms)
	{
		onDelayMS = ms;
	}


	int HoverableComponent::HoverTimer::getTime()
	{
		return onDelayMS;
	}

	bool HoverableComponent::HoverTimer::isHovering()
	{
		return hovering;
	}

	void HoverableComponent::HoverTimer::setHovering(bool state, bool notifyListeners, bool notifyPartners, HoverableComponent* excludedPartner)
	{
		if (p->extendedComponent->isEnabled() && p->extendedComponent->isShowing())
		{
			hovering = state;
			p->extendedComponent->repaint();
			if (state)
			{
				startTimer(onDelayMS);
			}
			else
			{
				startTimer(offDelayMS);
			}
			if (notifyListeners)
			{
				notifyParentsListeners();
			}
			if (notifyPartners)
			{
				notifyParentsPartners(excludedPartner);
			}
		}
	}

	void HoverableComponent::HoverTimer::notifyParentsListeners()
	{
		for (auto l : p->listeners)
		{
			if (l != nullptr)
			{
				if (l->onHoveringStateChanged.operator bool())
				{
					l->onHoveringStateChanged(p, hovering);
				}
			}
			else
			{
				p->removeHoverListener(l);
			}
		}
	}

	void HoverableComponent::HoverTimer::notifyParentsPartners(HoverableComponent* excludedPartner)
	{
		for (auto h : p->hoverPartners)
		{
			if (h != nullptr)
			{
				if (h != excludedPartner)
				{
					h->setHovering(hovering, false, true, p);
				}
			}
			else
			{
				p->removeHoverPartner(h);
			}
		}
	}


	void HoverableComponent::HoverTimer::mouseEnter(const juce::MouseEvent& e)
	{
		//if (p->extendedComponent->contains(e.position))
		//{
		//}

		mouseWithin = true;
		setHovering(true);

		//hovering = true;
		//p->extendedComponent->repaint();
		//startTimer(onDelayMS);

		//for (auto c : p->hoverPartners)
		//{
		//	c->setHovering(true);
		//}
	}


	void HoverableComponent::HoverTimer::mouseExit(const juce::MouseEvent& e)
	{
		//bool exit = true;
		//for (auto h : p->hoverPartners)
		//{
		//	auto pos = e.getEventRelativeTo(h->extendedComponent);
		//	if (h->extendedComponent->hitTest(pos.x, pos.y))
		//	{
		//		exit = false;
		//		break;
		//	}
		//}

		//if (exit)
		//{
		//	setHovering(false);
		//}
		mouseWithin = false;
		setHovering(false);

		//hovering = false;
		//p->extendedComponent->repaint();
		//stopTimer();
		//if (hoverFunc.operator bool())
		//{
		//	hoverFunc(false);
		//}

		//for (auto c : p->hoverPartners)
		//{
		//	c->setHovering(false);
		//}

	}

	bool HoverableComponent::HoverTimer::isMouseWithin()
	{
		return mouseWithin;
	}


	//================================================================================================================================================================================================


	ComponentCore::ComponentCore(juce::Component* componentToExtend, GUI_Universals* universalsToUse)
		:HoverableComponent(componentToExtend)
	{
		universals = universalsToUse;
		extendedComponent->setPaintingIsUnclipped(false);

		extendedComponent->setLookAndFeel(&universals->lnf);


	}

	ComponentCore::~ComponentCore()
	{
		//extendedComponent->setLookAndFeel(nullptr);
	}





	juce::Rectangle<int> ComponentCore::getBoundsRelativeToDesktopManager()
	{
		return universals->desktopManager.getLocalArea(extendedComponent, extendedComponent->getLocalBounds());
	}

	// forces the aspect ratio as close to specified value as possible given current size and the fact that both height and width must be set to integer values
	void ComponentCore::forceAspectRatio(double aspectRatio, juce::RectanglePlacement position)
	{
		extendedComponent->setBounds(confineToAspectRatio(extendedComponent->getBounds(), aspectRatio, position));
	}



	//void ComponentCore::paintCircularSlider(juce::Graphics& g, BaseSlider* s, juce::Rectangle<int> bounds, bool paintValue, bool paintKnob, bool paintPtr, bool paintTrack)
	//{
	//	g.saveState();


	//	//================================================================================================================================================================================================
	//	// stop that stupid artifact when ptr starts @ exactly halfway w/ contorl influences


	//	//g.reduceClipRegion(bounds.reduced(1));


	//	//================================================================================================================================================================================================



	//	auto rotaryStartAngle = s->getRotaryParameters().startAngleRadians;
	//	auto rotaryEndAngle = s->getRotaryParameters().endAngleRadians;
	//	auto sliderPos = s->valueToProportionOfLength(s->getValue());
	//	auto actualSliderPos = s->valueToProportionOfLength(s->getActualValue());

	//	auto sliderAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	//	auto actualSliderAngle = rotaryStartAngle + actualSliderPos * (rotaryEndAngle - rotaryStartAngle);


	//	auto LNF = dynamic_cast<LookAndFeel*>(&s->getLookAndFeel());


	//	auto alpha = s->isEnabled() ? 1.0f : universals->disabledAlpha;



	//	auto center = bounds.toFloat().getCentre();

	//	auto r = juce::jmin(center.x, center.y);
	//	auto innerR = r * (1 - BDSP_CIRCULAR_SLIDER_TRACK_RATIO);
	//	auto w = (r - innerR);





	//	if (paintTrack)
	//	{
	//		//g.setColour(universals->hoverAdjustmentFunc(s->findColour(juce::Slider::trackColourId), s->isHovering()).withMultipliedAlpha(alpha * 0.5f));
	//		g.setColour(universals->colors.getColor(BDSP_COLOR_PURE_BLACK));
	//		g.fillPath(createCenteredPieSegment(center, innerR, r, rotaryStartAngle, rotaryEndAngle));

	//	}
	//	if (paintKnob)
	//	{






	//		auto strokeW = w / 4;
	//		auto circleSize = w - strokeW / 2;
	//		juce::PathStrokeType ptrStroke(strokeW, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
	//		juce::Path ptr;




	//		if (s->getType() == BaseSlider::CenterMirrored)
	//		{
	//			auto angle1 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 + sliderPos) / 2.0;
	//			auto angle2 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 - sliderPos) / 2.0;



	//			ptr.addEllipse(juce::Rectangle<float>(circleSize, circleSize).withCentre(center.getPointOnCircumference(r - w / 2, angle1)));
	//			ptr.addEllipse(juce::Rectangle<float>(circleSize, circleSize).withCentre(center.getPointOnCircumference(r - w / 2, angle2)));

	//		}
	//		else
	//		{

	//			ptr.addEllipse(juce::Rectangle<float>(circleSize, circleSize).withCentre(center.getPointOnCircumference(r - w / 2, sliderAngle)));



	//		}
	//		ptrStroke.createStrokedPath(ptr, ptr);



	//		g.setColour(universals->hoverAdjustmentFunc(s->findColour(juce::Slider::backgroundColourId), s->isHovering()).withMultipliedAlpha(alpha));


	//		g.fillPath(ptr);

	//	}

	//	if (paintValue)
	//	{



	//		float midAngle = (rotaryStartAngle + rotaryEndAngle) / 2;
	//		juce::Path  p;


	//		switch (s->getType())
	//		{
	//		case BaseSlider::Normal:
	//			p = createCenteredPieSegment(center, innerR, r, rotaryStartAngle, actualSliderAngle);

	//			break;

	//		case BaseSlider::CenterZero:

	//			p = createCenteredPieSegment(center, innerR, r, midAngle, actualSliderAngle);

	//			break;

	//		case BaseSlider::CenterMirrored:
	//			auto angleDelta = (actualSliderAngle - rotaryStartAngle) / 2.0;
	//			p = createCenteredPieSegment(center, innerR, r, midAngle - angleDelta, midAngle + angleDelta);

	//			break;
	//		}


	//		g.setColour(universals->hoverAdjustmentFunc(s->findColour(juce::Slider::trackColourId), s->isHovering()).withMultipliedAlpha(alpha));
	//		g.fillPath(p);

	//	}

	//	if (paintPtr)
	//	{
	//		float midAngle = (rotaryStartAngle + rotaryEndAngle) / 2;


	//		juce::Path  p;

	//		switch (s->getType())
	//		{
	//		case BaseSlider::Normal:
	//			p.addEllipse(juce::Rectangle<float>(w, w).withCentre(center.getPointOnCircumference(r - w / 2, actualSliderAngle)));

	//			break;

	//		case BaseSlider::CenterZero:

	//			p.addEllipse(juce::Rectangle<float>(w, w).withCentre(center.getPointOnCircumference(r - w / 2, actualSliderAngle)));

	//			break;

	//		case BaseSlider::CenterMirrored:
	//			auto angleDelta = (actualSliderAngle - rotaryStartAngle) / 2.0;
	//			p.addEllipse(juce::Rectangle<float>(w, w).withCentre(center.getPointOnCircumference(r - w / 2, midAngle - angleDelta)));
	//			p.addEllipse(juce::Rectangle<float>(w, w).withCentre(center.getPointOnCircumference(r - w / 2, midAngle + angleDelta)));


	//			break;
	//		}

	//		g.setColour(universals->hoverAdjustmentFunc(s->findColour(juce::Slider::trackColourId), s->isHovering()).withMultipliedAlpha(alpha));
	//		g.fillPath(p);

	//	}

	//	g.restoreState();
	//}

	void ComponentCore::paintLinearSlider(juce::Graphics& g, BaseSlider* s, const juce::Rectangle<int>& bounds, bool paintValue, bool paintKnob, bool paintValuePtr, bool paintTrack)
	{
		auto sliderPos = s->getNormalisableRange().convertTo0to1(s->getValue());
		auto actualSliderPos = s->getNormalisableRange().convertTo0to1(s->getActualValue());



		auto alpha = s->isEnabled() ? 1.0f : universals->disabledAlpha;

		auto ptrGap = BDSP_LINEAR_SLIDER_POINTER_SIZE / (2 * (BDSP_LINEAR_SLIDER_POINTER_SIZE + 1)); // need so value ptr at extremes doesn't get clipped out

		bool vertical = s->isVertical();

		auto reduced = vertical ?
			bounds.toFloat().getProportion(juce::Rectangle<float>(0.5 - (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT) / 2, ptrGap, (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT), 1 - 2 * ptrGap)) :
			bounds.toFloat().getProportion(juce::Rectangle<float>(ptrGap, 0.5 - (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT) / 2, 1 - 2 * ptrGap, (1 - BDSP_LINEAR_SLIDER_POINTER_OVERSHOOT)));

		auto ptrSize = (vertical ? reduced.getHeight() : reduced.getWidth()) * BDSP_LINEAR_SLIDER_POINTER_SIZE;
		juce::PathStrokeType ptrStroke(ptrSize, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
		auto& outlineStroke = ptrStroke;
		outlineStroke.setStrokeThickness(ptrStroke.getStrokeThickness() * 0.75);
		ptrSize += outlineStroke.getStrokeThickness();

		juce::Path track, knob, value, valuePTR;
		if (paintValue)
		{
			if (s->getType() == BaseSlider::CenterMirrored)
			{
				if (vertical)
				{
					value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.0, 0.5 - sliderPos / 2 + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2), reduced.getRelativePoint(0.0, 0.5 + sliderPos / 2 - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2)));
				}
				else
				{
					value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.5 - sliderPos / 2 + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0), reduced.getRelativePoint(0.5 + sliderPos / 2 - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 1.0)));
				}
			}
			else if (s->getType() == BaseSlider::CenterZero)
			{

				if (vertical)
				{
					value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(0.0, 1 - sliderPos + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2), reduced.getRelativePoint(1.0, 0.5)));
				}
				else
				{
					value.addRectangle(juce::Rectangle<float>(reduced.getRelativePoint(sliderPos - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0), reduced.getRelativePoint(0.5, 1.0)));
				}
			}
			else
			{
				if (vertical)
				{
					value.addRectangle(reduced.withTop(reduced.getRelativePoint(0.0, 1 - sliderPos + BDSP_LINEAR_SLIDER_POINTER_SIZE / 2).y));
				}
				else
				{
					value.addRectangle(reduced.withX(reduced.getRelativePoint(sliderPos - BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0.0).x));
				}
			}
		}

		if (paintKnob)
		{
			if (s->getType() == BaseSlider::CenterMirrored)
			{
				if (vertical)
				{
					knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - sliderPos / 2).y));
					knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - sliderPos / 2).y));

					knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + sliderPos / 2).y));
					knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + sliderPos / 2).y));

				}
				else
				{
					knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 - sliderPos / 2, 0.0).x, ptrSize / 2));
					knob.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 - sliderPos / 2, 0.0).x, bounds.getCentreY() - ptrSize / 2));

					knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 + sliderPos / 2, 0.0).x, ptrSize / 2));
					knob.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 + sliderPos / 2, 0.0).x, bounds.getCentreY() - ptrSize / 2));
				}
			}
			else
			{
				if (vertical)
				{
					knob.startNewSubPath(juce::Point<float>(ptrSize / 2, reduced.getRelativePoint(0.0, 1 - sliderPos).y));
					knob.lineTo(juce::Point<float>(bounds.getCentreX() - ptrSize / 2, reduced.getRelativePoint(0.0, 1 - sliderPos).y));

				}
				else
				{
					knob.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(sliderPos, 0.0).x, ptrSize / 2));
					knob.lineTo(juce::Point<float>(reduced.getRelativePoint(sliderPos, 0.0).x, bounds.getCentreY() - ptrSize / 2));
				}
			}
			ptrStroke.createStrokedPath(knob, knob);
		}




		if (paintValuePtr)
		{

			if (s->getType() == BaseSlider::CenterMirrored)
			{
				if (vertical)
				{
					valuePTR.startNewSubPath(juce::Point<float>(bounds.getCentreX() + ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - actualSliderPos / 2).y));
					valuePTR.lineTo(juce::Point<float>(bounds.getWidth() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 - actualSliderPos / 2).y));

					valuePTR.startNewSubPath(juce::Point<float>(bounds.getCentreX() + ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + actualSliderPos / 2).y));
					valuePTR.lineTo(juce::Point<float>(bounds.getWidth() - ptrSize / 2, reduced.getRelativePoint(0.0, 0.5 + actualSliderPos / 2).y));

				}
				else
				{
					valuePTR.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 - actualSliderPos / 2, 0.0).x, bounds.getCentreY() + ptrSize / 2));
					valuePTR.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 - actualSliderPos / 2, 0.0).x, bounds.getHeight() - ptrSize / 2));

					valuePTR.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(0.5 + actualSliderPos / 2, 0.0).x, bounds.getCentreY() + ptrSize / 2));
					valuePTR.lineTo(juce::Point<float>(reduced.getRelativePoint(0.5 + actualSliderPos / 2, 0.0).x, bounds.getHeight() - ptrSize / 2));
				}
			}
			else
			{
				if (vertical)
				{
					valuePTR.startNewSubPath(juce::Point<float>(bounds.getCentreX() + ptrSize / 2, reduced.getRelativePoint(0.0, 1 - actualSliderPos).y));
					valuePTR.lineTo(juce::Point<float>(bounds.getWidth() - ptrSize / 2, reduced.getRelativePoint(0.0, 1 - actualSliderPos).y));

				}
				else
				{
					valuePTR.startNewSubPath(juce::Point<float>(reduced.getRelativePoint(actualSliderPos, 0.0).x, bounds.getCentreY() + ptrSize / 2));
					valuePTR.lineTo(juce::Point<float>(reduced.getRelativePoint(actualSliderPos, 0.0).x, bounds.getHeight() - ptrSize / 2));
				}
			}
			ptrStroke.createStrokedPath(valuePTR, valuePTR);

		}

		if (paintTrack)
		{

			if (vertical)
			{
				track.addRectangle(reduced.getProportion(juce::Rectangle<float>(0, BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 1, 1 - BDSP_LINEAR_SLIDER_POINTER_SIZE)));
			}
			else
			{
				track.addRectangle(reduced.getProportion(juce::Rectangle<float>(BDSP_LINEAR_SLIDER_POINTER_SIZE / 2, 0, 1 - BDSP_LINEAR_SLIDER_POINTER_SIZE, 1)));
			}

		}


		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(s->valueTrackColor), s->isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(track);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(s->valueColor), s->isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(value);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(s->valueTrackColor), s->isHovering()).withMultipliedAlpha(alpha));
		g.strokePath(knob, outlineStroke);
		g.strokePath(valuePTR, outlineStroke);

		//================================================================================================================================================================================================
		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(s->valueColor), s->isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(valuePTR);

		//================================================================================================================================================================================================

		g.setColour(universals->hoverAdjustmentFunc(universals->colors.getColor(s->knobColor), s->isHovering()).withMultipliedAlpha(alpha));
		g.fillPath(knob);


	}

	//circular middle knob, with ptr cutout and track outside for value, gap for influence


	void ComponentCore::paintCircularSlider(juce::Graphics& g, BaseSlider* s, const juce::Rectangle<int>& bounds, bool paintValue, bool paintKnob, bool paintActualValue, bool paintTrack)
	{
		g.saveState();


		//================================================================================================================================================================================================
		// stop that stupid artifact when ptr starts @ exactly halfway w/ contorl influences


		//g.reduceClipRegion(bounds.reduced(1));


		//================================================================================================================================================================================================
		auto newBounds = bounds;
		if (dynamic_cast<ControlParameter*>(s->getParameter()) != nullptr)
		{
			newBounds = expandRectangleToInt(bounds.toFloat().getProportion(juce::Rectangle<float>(BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO)));
		}


		auto rotaryStartAngle = s->getRotaryParameters().startAngleRadians;
		auto rotaryEndAngle = s->getRotaryParameters().endAngleRadians;
		auto sliderPos = s->getNormalisableRange().convertTo0to1(s->getValue());
		auto actualSliderPos = s->getNormalisableRange().convertTo0to1(s->getActualValue());

		auto sliderAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
		auto actualSliderAngle = rotaryStartAngle + actualSliderPos * (rotaryEndAngle - rotaryStartAngle);




		auto alpha = s->isEnabled() ? 1.0f : universals->disabledAlpha;



		auto center = newBounds.toFloat().getCentre();

		auto r = newBounds.getWidth() / 2.0;
		auto innerR = r * (1 - BDSP_CIRCULAR_SLIDER_TRACK_RATIO);
		auto w = (r - innerR);
		auto trackWidth = w * BDSP_CIRCULAR_SLIDER_TRACK_WIDTH;
		auto trackWidthInside = trackWidth * (1 - BDSP_CIRCULAR_SLIDER_TRACK_INSIDE_RATIO);

		//auto trackW = s->isHovering() ? trackWidth : trackWidthInside;
		auto trackW = trackWidthInside;
		auto knobR = innerR * BDSP_CIRCULAR_SLIDER_KNOB_RATIO;

		jassert(trackWidth <= w);




		if (paintKnob)
		{
			juce::Path knob;
			knob.addEllipse(expandRectangleToInt(juce::Rectangle<float>(2 * knobR, 2 * knobR).withCentre(center)).toFloat());
			auto color = (universals->hoverAdjustmentFunc(s->findColour(juce::Slider::backgroundColourId), s->isHovering()).withMultipliedAlpha(alpha));
			juce::ColourGradient cg(color.withMultipliedLightness(1.1f).withMultipliedAlpha(alpha), knob.getBounds().getCentre().getX(), knob.getBounds().getY(), color, center.x, center.y, true);
			g.setGradientFill(cg);
			g.fillPath(knob);



			//================================================================================================================================================================================================

			//juce::Image botShadow(juce::Image::PixelFormat::ARGB, knob.getBounds().getWidth(), knob.getBounds().getHeight(), true);
			//juce::ColourGradient shadowGrad(juce::Colour(), 0, 0, juce::Colour(0,0,0), 0, knobR, false);

			//juce::Graphics shadowG(botShadow);
			//shadowG.setGradientFill(shadowGrad);
			//shadowG.strokePath(knob, juce::PathStrokeType(0.075 * knobR), juce::AffineTransform().translated(-knob.getBounds().getPosition()));
			//gin::applyStackBlur(botShadow, juce::jlimit(2, 254, int(knobR * 0.33)));

			//g.drawImageAt(botShadow, knob.getBounds().getX(), knob.getBounds().getY());

			//================================================================================================================================================================================================


			juce::PathStrokeType ptrStroke(knobR * BDSP_CIRCULAR_SLIDER_THUMB_RATIO, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
			juce::Path ptr;


			if (s->getType() == BaseSlider::CenterMirrored)
			{
				auto angle1 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 + sliderPos) / 2.0;
				auto angle2 = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (1 - sliderPos) / 2.0;



				ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, angle1));
				ptr.lineTo(center.getPointOnCircumference(knobR, angle1));

				ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, angle2));
				ptr.lineTo(center.getPointOnCircumference(knobR, angle2));


				ptrStroke.createStrokedPath(ptr, ptr);

			}
			else
			{

				ptr.startNewSubPath(center.getPointOnCircumference(ptrStroke.getStrokeThickness() / 2, sliderAngle));


				ptr.lineTo(center.getPointOnCircumference(knobR, sliderAngle));

				ptrStroke.createStrokedPath(ptr, ptr);
			}




			g.setColour(universals->hoverAdjustmentFunc(s->universals->colors.getColor(BDSP_COLOR_PURE_BLACK), s->isHovering()).withMultipliedAlpha(alpha));
			g.fillPath(ptr);

		}

		if (paintTrack)
		{
			g.setColour(universals->hoverAdjustmentFunc(s->universals->colors.getColor(s->valueTrackColor), s->isHovering()).withMultipliedAlpha(alpha));
			g.fillPath(createCenteredPieSegment(center, r - w / 2 - trackWidth / 2, r - w / 2 + trackWidth / 2, rotaryStartAngle, rotaryEndAngle));

			g.setColour(universals->hoverAdjustmentFunc(s->universals->colors.getColor(s->valueTrackInsideColor), s->isHovering()).withMultipliedAlpha(alpha));
			g.fillPath(createCenteredPieSegment(center, r - w / 2 - trackWidthInside / 2, r - w / 2 + trackWidthInside / 2, rotaryStartAngle, rotaryEndAngle));

		}



		if (paintValue)
		{



			float midAngle = (rotaryStartAngle + rotaryEndAngle) / 2;
			juce::Path  p;


			switch (s->getType())
			{
			case BaseSlider::Normal:
				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, rotaryStartAngle, sliderAngle);

				break;

			case BaseSlider::CenterZero:

				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle, sliderAngle);

				break;

			case BaseSlider::CenterMirrored:
				auto angleDelta = (sliderAngle - rotaryStartAngle) / 2.0;
				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle - angleDelta, midAngle + angleDelta);


				break;
			}


			g.setColour(universals->hoverAdjustmentFunc(s->universals->colors.getColor(s->valueColor), s->isHovering()).withMultipliedAlpha(alpha));
			g.fillPath(p);

		}

		if (paintActualValue)
		{

			float midAngle = (rotaryStartAngle + rotaryEndAngle) / 2;
			juce::Path  p;


			switch (s->getType())
			{
			case BaseSlider::Normal:
				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, rotaryStartAngle, actualSliderAngle);

				break;

			case BaseSlider::CenterZero:

				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle, actualSliderAngle);

				break;

			case BaseSlider::CenterMirrored:
				auto angleDelta = (actualSliderAngle - rotaryStartAngle) / 2.0;
				p = createCenteredPieSegment(center, r - w / 2 - trackW / 2, r - w / 2 + trackW / 2, midAngle - angleDelta, midAngle + angleDelta);


				break;
			}


			g.setColour(universals->hoverAdjustmentFunc(s->universals->colors.getColor(s->valueColor), s->isHovering()).withMultipliedAlpha(alpha));
			g.fillPath(p);

		}


		g.restoreState();
	}

	void ComponentCore::repaintThreadChecked()
	{
		universals->repaintChecker->callRepaint(juce::WeakReference<juce::Component>(extendedComponent));
	}









	void ComponentCore::setHintBarText(const juce::String& text)
	{
		const juce::MessageManagerLock mml;
		extendedComponent->removeMouseListener(hintListener.get());
		hintListener.reset(new HintBar::Listener(universals->hintBar.get(), text));

		extendedComponent->addMouseListener(hintListener.get(), true);

		if (extendedComponent->isMouseOver())
		{
			universals->hintBar->setText(hintListener->getText());
		}


	}

	const juce::AttributedString& ComponentCore::getHintText()
	{
		return hintListener->getText();
	}

	void ComponentCore::setFontIndex(int idx)
	{
		fontIndex = idx;
	}

	int ComponentCore::getFontIndex()
	{
		return fontIndex;
	}

	juce::Colour ComponentCore::getColor(const NamedColorsIdentifier& name)
	{
		return universals->colors.getColor(name);
	}

	juce::Font ComponentCore::getFont(int index)
	{
		if (index < 0)
		{
			index = fontIndex;
		}

		return universals->Fonts[index].getFont();
	}




	void ComponentCore::clearLookAndFeels()
	{
		extendedComponent->setLookAndFeel(nullptr);
	}

	void ComponentCore::resetLookAndFeels(GUI_Universals* universalsToUse)
	{

	}





} // namespace bdsp