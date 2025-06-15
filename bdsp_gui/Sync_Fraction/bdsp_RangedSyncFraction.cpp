#include "bdsp_RangedSyncFraction.h"

namespace bdsp
{
	RangedSyncFraction::RangedSyncFraction(juce::AudioParameterChoice* divParam, GUI_Universals* universalsToUse, BPMComponent* BPMComp, const juce::String& baseName, bool isRate)
		:SyncFraction(divParam, universalsToUse, BPMComp, baseName, isRate)
	{

		auto prev = divisionCombo->onChange;
		divisionCombo->onChange = [=](int i)
		{
			if (snapButton != nullptr)
			{
				snapButton->setEnabled(i > 0);
			}
			if (prev.operator bool())
			{
				prev(i);
			}
		};
	}

	RangedSyncFraction::~RangedSyncFraction()
	{

	}

	void RangedSyncFraction::swap(RangedSyncFraction* other)
	{
		circleTime->swap(other->circleTime.get());
		circleFrac->swap(other->circleFrac.get());


		auto otherDivision = other->getDivision()->getIndex();
		other->division->selectItem(division->getIndex());

		division->selectItem(otherDivision);

	}



	void RangedSyncFraction::paint(juce::Graphics& g)
	{

	}

	void RangedSyncFraction::resized()
	{
		//		bdsp::CircleSlider& frac = *fraction.get();
		bdsp::CircleSlider& ms = *circleTime.get();


		circleFrac->boundsPlacement = timeRectanglePlacement;
		circleTime->boundsPlacement = timeRectanglePlacement;



		circleFrac->setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(timeX, 0, timeW, 1)));
		circleTime->setBounds(circleFrac->getBounds());



		//division->setBounds(juce::Rectangle<int>(fraction->getRight() + borderColor, 0, fraction->getWidth(), getHeight()).withSizeKeepingCentre(fraction->getWidth(), ms.getKnobBounds().getHeight() * 0.75));
		divisionCombo->setBounds(getLocalBounds().getProportion(juce::Rectangle<float>(divX, 0, divW, 1)).withBottom(ms.getKnobBounds(true).getBottom()).withTop(ms.getKnobBounds(true).getY()));

	}

	float RangedSyncFraction::getTimeInMs(double bpm, bool isRate)
	{
		return calculateTimeInMs(bpm, circleTime->getControlParamter()->getActualValue(), circleFrac->getControlParamter()->getActualValue(), divisionCombo->getIndex(), isRate);
	}

	void RangedSyncFraction::setSnapButton(juce::Button* buttonToLink)
	{
		snapButton = buttonToLink;
		divisionCombo->onChange(divisionCombo->getIndex());
	}


	template class SyncFraction<RangedCircleSlider>;


}// namnepace bdsp

