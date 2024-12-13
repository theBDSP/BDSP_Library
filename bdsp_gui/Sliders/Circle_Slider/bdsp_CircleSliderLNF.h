#pragma once

namespace bdsp
{
#define trackGap 0.1
	class CircleSliderLNF : public LookAndFeel
	{

	public:
		CircleSliderLNF(GUI_Universals* universalsToUse)
			:LookAndFeel(universalsToUse)
		{


			setColour(juce::Label::outlineColourId, juce::Colour());
			setColour(juce::Label::backgroundColourId, juce::Colour());



		}
		~CircleSliderLNF()
		{

		}







		void drawLabel(juce::Graphics& g, juce::Label& label)override
		{
			//const auto& textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

			//auto alpha = label.isEnabled() ? 1.0f : universals->disabledAlpha;

			//auto bdspLabel = dynamic_cast<Label*>(&label);
			//jassert(bdspLabel != nullptr);
			//auto maxText = bdspLabel->getMaxText();
			//maxText = maxText.isEmpty() ? label.getText() : maxText;





			////g.setFont(bdsp::resizeFontToFit(getFont(bdspLabel->getFontIndex()), textArea.getWidth()*0.9, bdspLabel->getHeightRatio() * BDSP_FONT_HEIGHT_SCALING, maxText));
			//g.setFont(getFont(bdspLabel->getFontIndex()).withHeight(textArea.getHeight() * bdspLabel->getHeightRatio() * BDSP_FONT_HEIGHT_SCALING));


			//if (!label.isBeingEdited())
			//{
			//	g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), bdspLabel->isHovering()).withMultipliedAlpha(alpha));

			//	bdsp::drawText(g, g.getCurrentFont(), label.getText(), textArea, false, label.getJustificationType());


			//}

			//if (label.getCurrentTextEditor() != nullptr)
			//{
			//	label.getCurrentTextEditor()->applyFontToAllText(g.getCurrentFont());

			//	label.getCurrentTextEditor()->setJustification(label.getJustificationType());
			//	label.getCurrentTextEditor()->setIndents(0, 0);
			//	label.getCurrentTextEditor()->setBorder(juce::BorderSize<int>(0));
			//	label.getCurrentTextEditor()->setPopupMenuEnabled(false);

			//}
		}










	};
}// namnepace bdsp

