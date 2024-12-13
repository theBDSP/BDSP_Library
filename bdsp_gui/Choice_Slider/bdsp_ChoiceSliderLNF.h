#pragma once

namespace bdsp
{
	class ChoiceSliderLNF : public LookAndFeel
	{

	public:
		ChoiceSliderLNF(GUI_Universals* universalsToUse)
			:LookAndFeel(universalsToUse)
		{

		}
		~ChoiceSliderLNF() = default;



		void drawLabel(juce::Graphics& g, juce::Label& label)override
		{
			bool selected = label.getProperties().getWithDefault("Selected", false);
			auto textArea = label.getLocalBounds().withLeft(label.getProperties().getWithDefault("X", 0));
			g.setColour(getHoverColor(label.findColour(juce::Label::textWhenEditingColourId), label.findColour(juce::Label::textColourId), selected, label.isMouseOver(), universals->hoverMixAmt).withMultipliedAlpha(label.isEnabled() ? 1.0f : universals->disabledAlpha));

			g.setFont(getFont(0).withHeight(label.getHeight()));

			juce::GlyphArrangement ga;
			ga.addLineOfText(g.getCurrentFont(), label.getText(), 0, 0);
			
			juce::Path p;
			ga.createPath(p);


			scaleToFit(p, p.getBounds().withCentre(juce::Point<float>(textArea.getX() + p.getBounds().getWidth() / 2.0, textArea.getCentreY())));
			g.fillPath(p);


		}

		void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& s) override
		{
		}
	};
} // namespace bdsp
