#include "bdsp_RangedBarSlider.h"


namespace bdsp
{
	RangedBarSlider::RangedBarSlider(GUI_Universals* universalsToUse, const juce::String& baseName)
		:slider(universalsToUse, baseName),
		RangedSlider(universalsToUse)
	{
		setParent(&slider, false);
		addAndMakeVisible(slider);

	}
	RangedBarSlider::~RangedBarSlider()
	{

	}

	RangedBarSlider::operator BarSlider()
	{
		return slider;
	}

	RangedBarSlider::operator BarSlider* ()
	{
		return &slider;
	}

	RangedBarSlider::operator BarSlider& ()
	{
		return slider;
	}




	void RangedBarSlider::paint(juce::Graphics& g)
	{
		//paintLinearSlider(g, &slider, getLocalBounds(), false);
	}

	void RangedBarSlider::resized()
	{
		//switch (slider.getKnobPlacement())
		//{
		//case bdsp::BaseSlider::Above:
		//	track.setBounds(getLocalBounds().withTrimmedTop(getHeight() / 2.0));
		//	break;

		//case bdsp::BaseSlider::Below:
		//	track.setBounds(getLocalBounds().withTrimmedBottom(getHeight() / 2.0));
		//	break;

		//case bdsp::BaseSlider::Left:
		//	track.setBounds(getLocalBounds().withTrimmedLeft(getWidth() / 2.0));
		//	break;

		//case bdsp::BaseSlider::Right:
		//	track.setBounds(getLocalBounds().withTrimmedRight(getWidth() / 2.0));
		//	break;

		//}
		slider.setBoundsToIncludeLabel(getLocalBounds());

		for (auto d : displays)
		{
			d->setBounds(slider.getBounds());// .expanded(slider.getWidth() / 8));
		}

	}



}// namnepace bdsp
