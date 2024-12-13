
#pragma once



namespace bdsp
{

	class RangedSliderLNF : public CircleSliderLNF
	{
	public:

		RangedSliderLNF(GUI_Universals* universalsToUse)
			:CircleSliderLNF(universalsToUse)
		{
			setColour(juce::Slider::ColourIds::rotarySliderFillColourId, universals->colors.getColor(BDSP_COLOR_DARK));
			setColour(juce::Slider::rotarySliderOutlineColourId, universals->colors.getColor(BDSP_COLOR_DARK));
		}
		~RangedSliderLNF()
		{

		}

		void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)override
		{



		}

		void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& s)override
		{

		}

		void drawLinearSliderBackground(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& s)override
		{

		}

		void drawLinearSliderThumb(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& s)override
		{

		}


	//	void drawLabel(juce::Graphics& g, juce::Label& label)override
	//	{
	//		auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

	//		auto alpha = label.isEnabled() ? 1.0f : universals->disabledAlpha;

	//		auto bdspLabel = dynamic_cast<Label*>(&label);
	//		auto maxText = bdspLabel->getMaxText();
	//		maxText = maxText.isEmpty() ? label.getText() : maxText;
	//		jassert(bdspLabel != nullptr);





	//		g.setFont(resizeFontToFit(getFont(bdspLabel->getFontIndex()), textArea.getWidth() * 0.9, textArea.getHeight() * 0.9, maxText));



	//		if (!label.isBeingEdited())
	//		{
	//			g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), bdspLabel->isHovering()).withMultipliedAlpha(alpha));

	//			drawText(g, g.getCurrentFont(), label.getText(), textArea, label.getJustificationType());

	//		}

	//	}





	};


}// namnepace bdsp
