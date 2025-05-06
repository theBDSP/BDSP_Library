#include "bdsp_RangedSlider.h"

namespace bdsp
{
	RangedSlider::RangedSlider(GUI_Universals* universalsToUse)
		:AnimatedComponent(universalsToUse, &universalsToUse->animationTimer),
		RangedComponentCore()
	{


		std::function<void(bool)> func = [=](bool state)
		{
			if (hoverMenu.operator bool())
			{
				if (state && hoverMenu->canShow())
				{
					if (isShowing())
					{
						hoverMenu->show();
					}
				}
				else if (hoverMenu->canHide())
				{
					hoverMenu->hide();
				}
				else
				{
					hoverMenu->setHovering(true);
				}
			}

		};



		setHoverFunc(func);
	}

	RangedSlider::~RangedSlider()
	{
		for (int i = 0; i < BDSP_NUMBER_OF_MACROS; ++i)
		{
			dynamic_cast<MacroControllerObject*>(universals->controls[i])->getMacroParameter()->removeListener(this);
		}


	}

	void RangedSlider::swap(RangedSlider* other)
	{

		auto otherVal = other->parent->getValue();
		juce::Array<double> otherInfluences;
		for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
		{
			otherInfluences.set(i, other->influences[i]->getValue());
		}

		other->parent->setValue(parent->getValue(), juce::sendNotification);


		for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
		{
			other->influences[i]->setValue(influences[i]->getValue());
		}

		parent->setValue(otherVal, juce::sendNotification);

		for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
		{
			influences[i]->setValue(otherInfluences[i]);
		}
	}


	void RangedSlider::setParent(BaseSlider* p, bool setLookAndFeel)
	{

		parent = p;

		hoverMenu = std::make_unique<InfluenceHoverMenu>(this);




		//parent->addComponentListener(static_cast<AnimatedComponent*>(this));

		addHoverPartner(parent);

		if (setLookAndFeel)
		{
			//			parent->setLookAndFeel(&universals->MasterRangedSliderLNF);
		}




		parent->onAttach = [=]()
		{
			controlParam = static_cast<ControlParameter*>(parent->getParameter());

			parent->getValueFunction = [=]()
			{
				//double out = parent->getNormalisableRange().convertTo0to1(parent->getValue());

				//for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
				//{
				//	out += universals->linkables->controls[i]->control->value * influences[i]->getValue();
				//}

				//return parent->getNormalisableRange().convertFrom0to1(juce::jlimit(0.0, 1.0, out));
				return controlParam->getActualValue();
			};

			influences.clear();
			displays.clear();
			for (int i = 0; i < BDSP_NUMBER_OF_LINKABLE_CONTROLS; ++i)
			{

				if (i < BDSP_NUMBER_OF_MACROS)
				{
					dynamic_cast<MacroControllerObject*>(universals->controls[i])->getMacroParameter()->addListener(this);
				}
				influences.add(new RangedSliderInfluence(this, i));

				displays.add(new InfluenceDisplay(this, i));
				addChildComponent(displays.getLast());



				influences[i]->onHoveringStateChanged = [=](HoverableComponent*, bool state) //when link control component is hovering
				{
					displays[i]->setVisible(state);
				};
				//				influences[i]->setLookAndFeel(&universals->MasterRangedSliderLNF);
				influences[i]->setColor(influences[i]->getLinkedControlColor());// findColour(juce::Slider::trackColourId));

				//influences[i]->setHoverFunc(hover);
				influences[i]->setHovering(false);

				auto& prevValueFunc = influences[i]->onValueChange;

				influences[i]->onValueChange = [=]()
				{
					prevValueFunc();
					displays[i]->repaint();
				};



			}


			//controlParam->sendInitialUpdate();
		};


		//top->setRepaintsOnMouseActivity(false);


		parentSet = true;

	}


	juce::String RangedSlider::getID()
	{
		return  parent->getComponentID();
	}



	BaseSlider* RangedSlider::getParent()
	{
		return parent;
	}

	ControlParameter* RangedSlider::getControlParamter()
	{
		return controlParam;
	}





	// sets which range to highlight
	void RangedSlider::setActiveControl(int i)
	{
		ActiveIndex = i;
		parent->repaint();
	}





	// gets proportion from a value garunteed to be between 0 and 1, with correct sign based on input value
	double RangedSlider::getConstrainedProportion(double val)
	{
		return parent->getNormalisableRange().convertTo0to1(juce::jlimit(parent->getMinimum(), parent->getMaximum(), val));// *signum(label);
	}




	bool RangedSlider::isLinked()
	{
		bool out = false;

		for (auto i : influences)
		{
			if (i->isLinked())
			{
				out = true;
				break;
			}
		}
		return out;
	}

	bool RangedSlider::isLinkedToControl(int idx)
	{
		return influences[idx]->isLinked();
	}

	bool RangedSlider::isLinkedToAnimatedControl()
	{
		bool out = false;
		for (int i = BDSP_NUMBER_OF_MACROS; i < influences.size(); ++i)
		{
			if (isLinkedToControl(i))
			{
				out = true;
				break;
			}
		}
		return out;
	}



	//void RangedSlider::visibilityChanged()
	//{
	//	//for (auto i : influences)
	//	//{
	//	//	i->setVisible(isVisible());
	//	//}
	//}

	bool RangedSlider::hitTest(int x, int y)
	{
		return juce::Component::hitTest(x, y) || (parentSet ? parent->hitTest(x, y) : true) || hoverMenu->container.hitTest(x, y - getHeight());
	}

	void RangedSlider::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
	{
		hoverMenu->onShow();
	}

	ComponentCore* RangedSlider::getHoverMenuAnchor()
	{
		return this;
	}




	void RangedSlider::handleAsyncUpdate()
	{
		parent->repaint();
	}

	bool RangedSlider::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
	{
		auto cast = dynamic_cast<bdsp::LinkableControlComponent::DragComp*>(dragSourceDetails.sourceComponent.get());


		return cast != nullptr && isEnabled();// && !isLinkedToControl(cast->parent->getIndex());
	}

	void RangedSlider::itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
	{
		auto cast = dynamic_cast<bdsp::LinkableControlComponent::DragComp*>(dragSourceDetails.sourceComponent.get());
		//hoverMenu->show();


		// not already linked to this control
		if (!isLinkedToControl(cast->parent->getIndex()))
		{
			influences[cast->parent->getIndex()]->createLink();
			hoverMenu->enableAllInfluences();
		}


	}

	void RangedSlider::itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
	{
		auto cast = dynamic_cast<bdsp::LinkableControlComponent::DragComp*>(dragSourceDetails.sourceComponent.get());

		if (!isLinkedToControl(cast->parent->getIndex()))
		{
			hoverMenu->disableAllInfluences();
			hoverMenu->addInfluence(influences[cast->parent->getIndex()]);
		}

		hoverMenu->show();
	}

	void RangedSlider::itemDragExit(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails)
	{

		hoverMenu->enableAllInfluences();
		hoverMenu->hide();
	}




	void RangedSlider::parameterValueChanged(int parameterIndex, float newValue)
	{
		parent->repaintThreadChecked();
	}

	void RangedSlider::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
	{
	}

	void RangedSlider::update()
	{
		parent->repaintThreadChecked();
	}

	void RangedSlider::valueChanged(juce::Value& value)
	{
	}

	ComponentCore* RangedSlider::getComponent()
	{
		return nullptr;
	}








	//================================================================================================================================================================================================




	RangedSlider::InfluenceDisplay::InfluenceDisplay(RangedSlider* parent, int idx)
		:ComponentCore(this, parent->universals)
	{
		p = parent;
		inf = p->influences[idx];
		setInterceptsMouseClicks(false, false);


	}

	void RangedSlider::InfluenceDisplay::paint(juce::Graphics& g)
	{
		bool polarity = inf->getControlComp()->control->getPolarity();

		if (p->parent->rotary)
		{
			paintCircularDisplay(g, inf->getValue());

			if (polarity)
			{
				paintCircularDisplay(g, -inf->getValue());
			}
		}
		else
		{

			auto container = dynamic_cast<ContainerSlider*>(p->parent);

			if (container != nullptr)
			{
				paintContainerDisplay(g, inf->getValue());
				if (polarity)
				{
					paintContainerDisplay(g, -inf->getValue());
				}
			}
			else
			{
				paintLinearDisplay(g, inf->getValue());
				if (polarity)
				{
					paintLinearDisplay(g, -inf->getValue());
				}
			}
		}
	}

	void RangedSlider::InfluenceDisplay::paintContainerDisplay(juce::Graphics& g, double influenceValue)
	{

		auto cast = dynamic_cast<ContainerSlider*>(p->parent);

		auto c = universals->colors.getColor(inf->getColor());


		bool isVertical = cast->getIsVertical();

		auto baseProp = cast->valueToProportionOfLength(cast->getValue());

		auto floatBounds = getLocalBounds().toFloat();

		juce::PathStrokeType stroke(isVertical ? getWidth() / 2 : getHeight() / 2, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::rounded);

		if (!inf->isEnabled())
		{
			c = c.withMultipliedAlpha(universals->disabledAlpha);
		}
		juce::ColourGradient cg(c.withAlpha(0.0f), 0, 0, c, getWidth(), getHeight(), false);


		juce::Path line;
		if (p->getParent()->getType() == BaseSlider::Normal || p->getParent()->getType() == BaseSlider::CenterZero)
		{
			if (isVertical)
			{

				cg.point1 = floatBounds.getRelativePoint(0.5, 1 - baseProp);
				cg.point2 = floatBounds.getRelativePoint(0.5, juce::jlimit(0.0, 1.0, 1 - baseProp - influenceValue));

				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);
			}
			else
			{
				cg.point1 = floatBounds.getRelativePoint(baseProp, 0.5);
				cg.point2 = floatBounds.getRelativePoint(juce::jlimit(0.0, 1.0, baseProp + influenceValue), 0.5);

				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);
			}
		}
		else if (p->getParent()->getType() == BaseSlider::CenterMirrored)
		{

			if (isVertical)
			{
				cg.point1 = floatBounds.getRelativePoint(0.5, 0.5 - baseProp / 2);
				cg.point2 = floatBounds.getRelativePoint(0.5, juce::jlimit(0.0, 1.0, 0.5 - (baseProp + influenceValue) / 2));

				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);

				g.setGradientFill(cg);
				g.strokePath(line, stroke);

				line.clear();


				cg.point1 = floatBounds.getRelativePoint(0.5, 0.5 + baseProp / 2);
				cg.point2 = floatBounds.getRelativePoint(0.5, juce::jlimit(0.0, 1.0, 0.5 + (baseProp + influenceValue) / 2));

				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);
			}
			else
			{


				cg.point1 = floatBounds.getRelativePoint(0.5 - baseProp / 2, 0.5);
				cg.point2 = floatBounds.getRelativePoint(juce::jlimit(0.0, 1.0, 0.5 - (baseProp + influenceValue) / 2), 0.5);


				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);

				g.setGradientFill(cg);
				g.strokePath(line, stroke);

				line.clear();


				cg.point1 = floatBounds.getRelativePoint(0.5 + baseProp / 2, 0.5);
				cg.point2 = floatBounds.getRelativePoint(juce::jlimit(0.0, 1.0, 0.5 + (baseProp + influenceValue) / 2), 0.5);


				line.startNewSubPath(cg.point1);
				line.lineTo(cg.point2);
			}

		}

		g.setGradientFill(cg);
		g.strokePath(line, stroke);
	}

	void RangedSlider::InfluenceDisplay::paintLinearDisplay(juce::Graphics& g, double influenceValue)
	{
	}

	void RangedSlider::InfluenceDisplay::paintCircularDisplay(juce::Graphics& g, double influenceValue)
	{
		auto bounds = expandRectangleToInt(getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO, 1 - 2 * BDSP_CIRCULAR_SLIDER_INFLUENCE_DISPLAY_RATIO)));

		auto outR = getWidth() / 2.0;
		auto r = bounds.getWidth() / 2.0;

		auto displayIn = r;
		auto displayOut = outR;


		auto c = universals->colors.getColor(inf->getColor());

		auto params = p->getParent()->getRotaryParameters();
		auto base = params.startAngleRadians + p->getParent()->getNormalisableRange().convertTo0to1(p->getParent()->getValue()) * (params.endAngleRadians - params.startAngleRadians);
		auto alpha = p->universals->midOpacity;
		//auto alpha = 1.0f;//p->universals->lowOpacity;
		auto paramStart = juce::jmin(params.startAngleRadians, params.endAngleRadians);
		auto paramEnd = juce::jmax(params.startAngleRadians, params.endAngleRadians);


		if (!inf->isEnabled())
		{
			c = c.withMultipliedAlpha(universals->disabledAlpha);
		}
		juce::ColourGradient cg(c.withAlpha(alpha), 0, 0, c.withAlpha(alpha), getWidth(), getHeight(), false);

		if (p->getParent()->getType() == BaseSlider::CenterMirrored)
		{


			auto midAngle = (params.endAngleRadians + params.startAngleRadians) / 2.0;
			auto baseMid = p->getParent()->getNormalisableRange().convertTo0to1(p->getParent()->getValue()) * (paramEnd - paramStart) / 2.0;
			auto base1 = midAngle - baseMid;
			auto base2 = midAngle + baseMid;


			if (influenceValue < 0)
			{

				auto start = base1;
				auto end = start - influenceValue * (midAngle - paramStart);
				auto endLimited = juce::jlimit(start, midAngle, end);

				cg.addColour((end - start) / (2 * PI), c);

				auto path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, start, endLimited);
				ConicGradient conic(cg, getLocalBounds().toFloat());
				conic.fillPath(g, path, start);

				//================================================================================================================================================================================================

				conic.reverse();

				end = base2;
				start = end + influenceValue * (paramEnd - midAngle);
				auto startLimited = juce::jlimit(midAngle, end, start);

				path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, startLimited, end);

				conic.fillPath(g, path, end);
			}
			else
			{
				auto end = base1;
				auto start = end - influenceValue * (midAngle - paramStart);
				auto startLimited = juce::jlimit(double(paramStart), end, start);

				cg.addColour((end - start) / (2 * PI), c);
				ConicGradient conic(cg, getLocalBounds().toFloat());


				auto path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, startLimited, end);
				conic.fillPath(g, path, end);



				//================================================================================================================================================================================================

				conic.reverse();

				start = base2;
				end = start + influenceValue * (paramEnd - midAngle);
				auto endLimited = juce::jlimit(start, (double)paramEnd, end);

				path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, start, endLimited);
				conic.fillPath(g, path, start);
			}

			//auto start = midAngle - inf->getSlider()->getValue() * (midAngle - params.startAngleRadians);
			//auto end = midAngle + inf->getSlider()->getValue() * (params.endAngleRadians - midAngle);


			//juce::ColourGradient cg(c, 0, 0, c, getWidth(), getWidth(), false);
			//cg.addColour((end - start) / (2 * 2 * PI), c.withAlpha(alpha));
			//cg.addColour((end - start) / (2 * PI), c);

			//if (!inf->getSlider()->isEnabled())
			//{
			//	cg.multiplyOpacity(p->universals->midOpacity);
			//}

			//ConicGradient conic(cg, getLocalBounds().toFloat());
			//auto path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), innerR * 0.75, r - w / 2 - trackWidth / 2, start, end);
			//conic.fillPath(g, path, start);


		}
		else
		{
			if (influenceValue < 0)
			{
				auto end = base;
				auto start = end + influenceValue * (paramEnd - paramStart);
				start = juce::jlimit((double)paramStart, end, start);

				cg.addColour((end - start) / (2 * PI), c);
				ConicGradient conic(cg, getLocalBounds().toFloat(), true);

				auto path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, start, end);
				conic.fillPath(g, path, end);

			}
			else
			{
				auto start = base;
				auto end = start + influenceValue * (paramEnd - paramStart);
				end = juce::jlimit(start, (double)paramEnd, end);

				cg.addColour((end - start) / (2 * PI), c);
				ConicGradient conic(cg, getLocalBounds().toFloat());


				auto path = createCenteredPieSegment(getLocalBounds().toFloat().getCentre(), displayIn, displayOut, start, end);
				conic.fillPath(g, path, start);

			}
		}
	}

	void RangedSlider::InfluenceDisplay::visibilityChanged()
	{
		//p->setHovering(isVisible());
	}




	//================================================================================================================================================================================================


	RangedSlider::InfluenceHoverMenu::InfluenceHoverMenu(RangedSlider* sliderToAttachTo)
		:DesktopComponent(sliderToAttachTo->universals),
		container(sliderToAttachTo->universals)
	{
		parent = sliderToAttachTo;
		//p->universals->desktopManager.addComponent(this);

		setLossOfFocusClosesWindow(false);
		setClosesOthersOnShow(false);


		addAndMakeVisible(container);

		onHoveringStateChanged = [=](HoverableComponent* c, bool state)
		{
			if (state && canShow())
			{
				show();
			}
			else if (canHide())
			{
				hide();
			}
			else
			{
				setHovering(true); // if cant hide set it to hover 
			}
		};
		//parent->addHoverListener(this);


		std::function<void(bool)> hoverFunc = [=](bool state)
		{
			if (!state && canHide())
			{
				hide();
			}
			else
			{
				setHovering(true);
			}

			//parent->setHovering(state);
		};
		container.setHoverFunc(hoverFunc);


		influences.resize(BDSP_NUMBER_OF_LINKABLE_CONTROLS);

		onShow = [=]()
		{
			setHovering(true);


			resizeMenu();

		};


		onHide = [=]()
		{
			for (auto i : influences)
			{
				if (i != nullptr)
				{
					if (i->getValue() == 0)
					{
						i->removeLink();
					}
				}
			}
		};


		//std::function<void(bool)> func = [=](bool state)
		//{
		//	if (!state && canHide())
		//	{
		//		hide();
		//	}
		//};
		//container.setHoverFunc(func);


	}

	void RangedSlider::InfluenceHoverMenu::addInfluence(RangedSliderInfluence* influenceToAdd)
	{
		numInfluences += influences[influenceToAdd->getIndex()] == nullptr ? 1 : 0; // only increment if adding an influence that wasn't there before
		container.addAndMakeVisible(influenceToAdd);
		influences.set(influenceToAdd->getIndex(), influenceToAdd);

		resized();
		//show();
	}

	void RangedSlider::InfluenceHoverMenu::removeInfluence(RangedSliderInfluence* influenceToRemove)
	{
		numInfluences -= influences[influenceToRemove->getIndex()] != nullptr ? 1 : 0; // only decrement if removing an influence that was there before

		container.removeChildComponent(influenceToRemove);

		influences.set(influenceToRemove->getIndex(), nullptr);

		if (numInfluences > 0)
		{
			resized();
		}
		else
		{
			hide();
		}
	}

	void RangedSlider::InfluenceHoverMenu::resized()
	{
		int border = juce::jmax(universals->roundedRectangleCurve / 5, 1.0f);

		auto localBounds = bounds.withZeroOrigin();

		if (pointerLocation.y > 0)
		{
			container.setBounds(shrinkRectangleToInt(localBounds.withTrimmedBottom(universals->roundedRectangleCurve).reduced(border)));
		}
		else
		{
			container.setBounds(shrinkRectangleToInt(localBounds.withTrimmedTop(universals->roundedRectangleCurve).reduced(border)));
		}

		auto reduced = container.getLocalBounds().toFloat().withSizeKeepingCentre(numInfluences / (1 + BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO + BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO) * container.getHeight(), container.getHeight());

		float j = 0;
		for (int i = 0; i < influences.size(); ++i)
		{
			if (influences[i] != nullptr)
			{
				influences[i]->setBoundsToIncludeLabel(shrinkRectangleToInt(reduced.getProportion(juce::Rectangle<float>((j + 0.05) / numInfluences, 0.05, 0.9 / numInfluences, 0.9))));
				++j;
			}
		}


	}

	void RangedSlider::InfluenceHoverMenu::resizeMenu()
	{
		auto pBounds = parent->getHoverMenuAnchor()->getBoundsRelativeToDesktopManager();
		juce::Point<float> center;
		if (pBounds.getBottom() + universals->influenceHoverMenuHeight > universals->desktopManager.getBottom())
		{
			pointerLocation = juce::Point<float>(0.0f, universals->influenceHoverMenuHeight);
			center = juce::Point<float>(pBounds.getCentreX(), pBounds.getY() - universals->influenceHoverMenuHeight / 2.0);
		}
		else
		{
			pointerLocation = { 0.0f, 0.0f };
			center = juce::Point<float>(pBounds.getCentreX(), pBounds.getBottom() + universals->influenceHoverMenuHeight / 2.0);
		}

		auto desired = universals->influenceHoverMenuHeight * (numInfluences + 0.5) / (1 + BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO + BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO);
		bounds = juce::Rectangle<float>(desired, universals->influenceHoverMenuHeight).withCentre(center);


		auto rightOverHang = bounds.getRight() - manager->getWidth();
		auto leftOverHang = bounds.getX();
		auto border = universals->roundedRectangleCurve;

		pointerLocation = pointerLocation.withX(desired / 2);
		if (rightOverHang > -border) // extends past right edge
		{
			bounds.translate(-rightOverHang - border, 0);
			pointerLocation = pointerLocation.translated(rightOverHang + border, 0);

		}
		else if (leftOverHang < border) // extends past left edge
		{
			bounds.translate(-leftOverHang + border, 0);
			pointerLocation = pointerLocation.translated(leftOverHang - border, 0);
		}
		setBounds(shrinkRectangleToInt(bounds));


		resized();
	}

	void RangedSlider::InfluenceHoverMenu::paint(juce::Graphics& g)
	{
		juce::Path rect, tri;
		rect.addRoundedRectangle(container.getBounds().toFloat(), universals->roundedRectangleCurve);
		float centerX = pointerLocation.x;
		float w = universals->roundedRectangleCurve * 2;
		if (pointerLocation.y > 0) // filpped
		{
			float h = container.getBottom();
			//tri = createEquilateralTriangle(juce::Rectangle<float>(universals->roundedRectangleCurve, universals->roundedRectangleCurve).withCentre({ getWidth() / 2.0f, getHeight() - universals->roundedRectangleCurve / 2.0f }), PI);
			tri.startNewSubPath(centerX - w, h);
			tri.lineTo(centerX + w, h);
			tri.lineTo(centerX, h + universals->roundedRectangleCurve);
			tri.closeSubPath();
		}
		else
		{
			float h = container.getY();

			tri.startNewSubPath(centerX - w, h);
			tri.lineTo(centerX + w, h);
			tri.lineTo(centerX, h - universals->roundedRectangleCurve);
			tri.closeSubPath();
			//tri = createEquilateralTriangle(juce::Rectangle<float>(universals->roundedRectangleCurve, universals->roundedRectangleCurve).withCentre({ getWidth() / 2.0f, universals->roundedRectangleCurve / 2.0f }));
		}
		auto p = ClipperLib::performBoolean(rect, tri, ClipperLib::ctUnion);
		g.setColour(universals->colors.getColor(universals->sliderHoverMenuBGKD));
		g.fillPath(p);


		g.setColour(universals->colors.getColor(universals->sliderHoverMenuOutline));
		g.strokePath(p, juce::PathStrokeType(juce::jmax(universals->dividerSize, 1.0f)));



	}

	void RangedSlider::InfluenceHoverMenu::disableAllInfluences()
	{
		for (auto i : influences)
		{
			if (i != nullptr)
			{
				i->setEnabled(false);
			}
		}
	}

	void RangedSlider::InfluenceHoverMenu::enableAllInfluences()
	{
		for (auto i : influences)
		{
			if (i != nullptr)
			{
				i->setEnabled(true);
			}
		}
	}

	bool RangedSlider::InfluenceHoverMenu::canShow()
	{
		bool lambda = true;

		if (canShowLambda.operator bool())
		{
			lambda = canShowLambda();
		}
		return lambda && !isEmpty() && !universals->sliderPopup->isVisible();


	}

	bool RangedSlider::InfluenceHoverMenu::canHide()
	{
		bool editorOpen = false;
		bool dragging = false;
		bool popupOpen = false;
		for (auto i : influences)
		{
			if (i != nullptr)
			{
				if (i->label.getCurrentTextEditor() != nullptr)
				{
					editorOpen = true;

					i->label.onEditorHide = [=]()
					{
						container.setHovering(false);
					};
					break;
				}
				else if (i->isMouseOverOrDragging(true))
				{
					dragging = true;
					break;
				}
				else if (universals->sliderPopup->isVisible())
				{
					popupOpen = true;
					break;
				}
			}
		}

		bool lambda = true;

		if (canHideLambda.operator bool())
		{
			lambda = canHideLambda();
		}
		//return !(isMouseButtonDown() || isHovering() || container.isHovering() || p->isHovering() || editorOpen || dragging);
		return lambda && !(isMouseButtonDownAnywhere() || container.isHovering() || parent->isHovering() || parent->contains(parent->getMouseXYRelative()) || isMouseOver() || editorOpen || dragging || popupOpen);
	}

	bool RangedSlider::InfluenceHoverMenu::isEmpty()
	{
		return numInfluences == 0;
	}




}// namnepace bdsp

