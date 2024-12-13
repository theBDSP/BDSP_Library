#include "bdsp_RangedContainerSlider.h"
namespace bdsp
{

	RangedContainerSlider::RangedContainerSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		: RangedSlider(universalsToUse),
		slider(universalsToUse, baseName)
	{
		setParent(&slider);
		addAndMakeVisible(slider);
	}

	//RangedContainerSlider::RangedContainerSlider(const RangedContainerSlider& other)
	//	: RangedContainerSlider(other.universals, other.getComponentID())
	//{
	//	slider.setValue(other.slider.getValue());
	//}


	RangedContainerSlider::~RangedContainerSlider()
	{
	}

	RangedContainerSlider::operator ContainerSlider()
	{
		return slider;
	}

	RangedContainerSlider::operator ContainerSlider* ()
	{
		return &slider;
	}

	RangedContainerSlider::operator ContainerSlider& ()
	{
		return slider;
	}

	void RangedContainerSlider::resized()
	{

		slider.setBounds(getLocalBounds());



		juce::Rectangle<int> bounds;

		if (slider.getIsVertical())
		{
			bounds = slider.getBounds().withLeft(slider.label.getRight()).getProportion(juce::Rectangle<float>(0.55, 0, 0.4, 1)).reduced(0, universals->roundedRectangleCurve);
		}
		else
		{
			bounds = slider.getBounds().withTop(slider.label.getBottom()).getProportion(juce::Rectangle<float>(0, 0.55, 1, 0.4)).reduced(universals->roundedRectangleCurve, 0);
		}


		for (auto d : displays)
		{
			d->setBounds(bounds);// .expanded(slider.getWidth() / 8));
		}
	}

} // namespace bdsp
