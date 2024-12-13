#include "bdsp_RangedCircleSlider.h"

namespace bdsp
{



	RangedCircleSlider::RangedCircleSlider(GUI_Universals* universalsToUse, const  juce::String& baseName, bool isEdit)
		: RangedSlider(universalsToUse),
		slider(universalsToUse, baseName, isEdit)
	{
		auto angle = PI / 6.0;
		slider.setRotaryParameters(PI + angle, 3 * PI - angle, true);
		setParent(&slider, false);

		addAndMakeVisible(slider);
	}

	//RangedCircleSlider::RangedCircleSlider(const RangedCircleSlider& other)
	//	: RangedCircleSlider(other.universals, other.getComponentID())
	//{
	//	slider.setValue(other.slider.getValue());
	//}
	RangedCircleSlider::~RangedCircleSlider()
	{

	}
	RangedCircleSlider::operator CircleSlider()
	{
		return slider;
	}
	RangedCircleSlider::operator CircleSlider* ()
	{
		return &slider;
	}
	RangedCircleSlider::operator CircleSlider& ()
	{
		return slider;
	}
	void RangedCircleSlider::setNoLabel()
	{

		slider.label.setVisible(false);
		slider.label.getTitle().setVisible(false);


		resized();
	}
	//void RangedCircleSlider::updateLabel()
	//{

	//	CircleSlider::updateLabel();
	//	if (pathFromValueFunction.operator bool())
	//	{
	//		influenceLabel.setVisible(false);
	//		suffix.setVisible(false);

	//		labelPath = pathFromValueFunction(getActualValue());
	//	}
	//	//auto textUp = getTextFromValue(getActualValue());

	//	//influenceLabel.setText(textUp, juce::NotificationType::dontSendNotification);
	//	BaseSlider::repaint();
	//}






	void RangedCircleSlider::resized()
	{


		if (slider.hasLabel())
		{
			slider.setBoundsToIncludeLabel(getLocalBounds(), boundsPlacement);
		}
		else
		{
			slider.setBounds(getLocalBounds());
		}



		for (auto d : displays)
		{
			d->setBounds(slider.getBounds());// .expanded(slider.getWidth() / 8));
		}

	}


	ComponentCore* RangedCircleSlider::getComponent()
	{
		return nullptr;
	}








}// namnepace bdsp
