#include "bdsp_RangedSliderInfluence.h"
namespace bdsp
{
	RangedSliderInfluence::RangedSliderInfluence(RangedSlider* p,  int idx)
		: CircleSlider(p->parent->universals, juce::String(), true)
	{
		parent = p;


		controlComp = p->parent->universals->controlComps->controls[idx];
		controlComp->addColorListener(this);
		controlCompID = controlComp->getID();
		ID = parent->getControlParamter()->getBaseID() + controlCompID;


		universals = parent->parent->universals;
		index = idx;



		label.setTitle(controlComp->getDisplayName());
		initText();


		setType(BaseSlider::CenterZero);
		setRotaryParameters(PI, 3 * PI, true);

		//influence.setSliderColors(juce::Colour(),universals->Color,universals->Color.withAlpha(universals->midOpacity));
		//setColor(parent->getParent()->findColour(juce::Slider::trackColourId));


		//influence.onValueChange = [=]()
		//{
		//	//if (influence.isVisible())
		//	//{
		//	//	repaint();
		//	//}

		//};



		//textFromValueFunction = [=](double value)
		//{
		//	auto out = juce::String(value * 100);
		//	auto endPos = out.indexOfChar('.');
		//	if (endPos != -1)
		//	{
		//		out = out.substring(0, endPos);
		//	}
		//	out.append("%", 1);


		//	return out;
		//};

		//valueFromTextFunction = [=](juce::String s)
		//{
		//	return s.retainCharacters("0123456789.-").getFloatValue() / 100;
		//};

		onValueChange = [=]()
		{
			bool nonZero = abs(getValue()) > getInterval();
			if (nonZero && !linked)
			{
				createLink();
			}
			else if (!nonZero && linked && !isMouseOverOrDragging())
			{
				removeLink();
			}

			p->repaint();
		};

		setInterceptsMouseClicks(true, true);





		setEnabled(parent->isEnabled());



		attach(*universals->APVTS, ID);
		onValueChange();

		popup->setClosesOthersOnShow(false);
		popup->setLossOfFocusClosesWindow(true, p->hoverMenu.get());
		//popup->addHoverPartner(p->hoverMenu.get());
		popup->onHide = [=]()
		{
			juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(p->getScreenBounds().toFloat().getCentre());

		};

		parent->addHoverPartner(popup.get());

		auto prevFunc = getHoverFunc();

		std::function<void(bool)> hoverFunc = [=](bool state)
		{
			prevFunc(state);
			p->displays[index]->setVisible(state);
		};
		setHoverFunc(hoverFunc);
	}
	RangedSliderInfluence::~RangedSliderInfluence()
	{


	}

	bool RangedSliderInfluence::isLinked()
	{
		//return !!(parent->getControlParamter()->getLinkParam(index)->getValue());
		//return parent->getControlParamter()->getInfluenceParameter(index)->getValue() != 0.5;
		return linked;
	}

	void RangedSliderInfluence::setColor(NamedColorsIdentifier col)
	{
		c = col;

		setSliderColor(c, NamedColorsIdentifier());
	}

	NamedColorsIdentifier RangedSliderInfluence::getColor()
	{
		return c;
	}

	void RangedSliderInfluence::resized()
	{
		CircleSlider::resized();
		label.setBounds(label.getBounds().withBottom(getParentHeight()));
	}

	void RangedSliderInfluence::paint(juce::Graphics& g)
	{
		auto alpha = isEnabled() ? 1.0f :universals->disabledAlpha;
		auto reduced = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(0.1, 0.1, 0.8, 0.8));

		g.setColour(universals->colors.getColor(c).withAlpha(alpha));
		auto outlineW = reduced.getWidth() * 0.075;
		g.drawEllipse(reduced, outlineW);
		g.fillPath(createCenteredPieSegment(reduced.getCentre(), 0, reduced.getWidth() * 0.5 - 1.5 * outlineW, 2 * PI, 2 * PI * getNormalisableRange().convertTo0to1(getValue()) + PI));



	}



	void RangedSliderInfluence::createLink()
	{
		linked = true;
		setVisible(true);






		setHintBarText(controlComp->name + " Amount");

		auto comps = controlComp->getComponentsThatShareHover();
		for (auto c : comps)
		{
			if (c != nullptr)
			{
				c->addHoverListener(this);
			}
		}



		parent->hoverMenu->addInfluence(this);


		parent->getControlParamter()->valueChanged();

		if (index >= BDSP_NUMBER_OF_MACROS)
		{
			parent->beginAnimation();
		}
		repaintThreadChecked();
		controlComp->addLink(getParameter());

	}

	void RangedSliderInfluence::removeLink()
	{
		linked = false;
		setHovering(false);
		setVisible(false);













		parent->hoverMenu->removeInfluence(this);

		auto comps = controlComp->getComponentsThatShareHover();
		for (auto c : comps)
		{
			if (c != nullptr)
			{
				c->removeHoverListener(this);
			}
		}



		parent->getControlParamter()->valueChanged();


		if (!parent->isLinkedToAnimatedControl())
		{
			parent->endAnimation();
		}
		repaintThreadChecked();

		controlComp->removeLink(getParameter());
	}


	int RangedSliderInfluence::getIndex()
	{
		return index;
	}






	LinkableControlComponent* RangedSliderInfluence::getControlComp()
	{
		return controlComp;
	}

	NamedColorsIdentifier RangedSliderInfluence::getLinkedControlColor()
	{
		//NamedColorsIdentifier out;
		//if (isLinked())
		//{
		//	out =universals->linkables->controls[index]->getColorID();
		//}
		//else
		//{
		//	out = parent->getParent()->valueColor;
		//}

		//return out;
		return controlComp->getColorID();
	}

	void RangedSliderInfluence::colorChanged(LinkableControlComponent* controlThatChanged)
	{
		setColor(controlThatChanged->getColorID());
	}










}


