#pragma once

#include "bdsp_gui/Sliders/Circle_Slider/bdsp_CircleSliderLNF.h"
namespace bdsp
{
	class BarSliderLNF : public CircleSliderLNF
	{
	public:
		BarSliderLNF(GUI_Universals* universalsToUse)
			:CircleSliderLNF(universalsToUse)
		{

			GUI_UniversalsChanged();
		}

		~BarSliderLNF()
		{

		}




		void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& te)override
		{

		}

		void GUI_UniversalsChanged() override
		{

			setColour(juce::Slider::backgroundColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0));

			setColour(juce::Slider::thumbColourId, universals->colors.getColor(BDSP_COLOR_DARK)); // outline
			setColour(juce::Slider::trackColourId, universals->colors.getColor(BDSP_COLOR_DARK).withAlpha(universals->midOpacity));
		}


	};
}// namnepace bdsp

