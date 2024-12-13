#include "bdsp_RangedCircleSliderSideLabel.h"

namespace bdsp
{



	RangedCircleSliderSideLabel::RangedCircleSliderSideLabel(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit)
		: RangedSlider(universalsToUse),
		slider(universalsToUse, baseName, isEdit)
	{
		auto angle = PI / 6.0;
		slider.setRotaryParameters(PI + angle, 3 * PI - angle, true);
		setParent(&slider, false);

		addAndMakeVisible(slider);
	}

	//RangedCircleSliderSideLabel::RangedCircleSliderSideLabel(const RangedCircleSliderSideLabel& other)
	//	: RangedCircleSliderSideLabel(nullptr, other.getComponentID(), other.linkables)
	//{
	//	slider.setValue(other.slider.getValue());
	//}
	RangedCircleSliderSideLabel::~RangedCircleSliderSideLabel()
	{

	}
	RangedCircleSliderSideLabel::operator CircleSliderSideLabel()
	{
		return slider;
	}
	RangedCircleSliderSideLabel::operator CircleSliderSideLabel* ()
	{
		return &slider;
	}
	RangedCircleSliderSideLabel::operator CircleSliderSideLabel& ()
	{
		return slider;
	}






	void RangedCircleSliderSideLabel::resized()
	{
		slider.setBoundsToIncludeLabel(getLocalBounds());

		for (auto d : displays)
		{
			d->setBounds(slider.getBounds());// .expanded(slider.getWidth() / 8));
		}

	}

	ComponentCore* RangedCircleSliderSideLabel::getHoverMenuAnchor()
	{
		return &slider;
	}










}// namnepace bdsp
