#pragma once



#define BDSP_CIRCLE_SLIDER_SIDE_LABEL_DEFAULT_WIDTH 2.5
#define BDSP_CIRCLE_SLIDER_SIDE_LABEL_GAP 0.33
namespace bdsp
{

	class CircleSliderSideLabel : public CircleSlider
	{
	public:
		CircleSliderSideLabel(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit = true);
		~CircleSliderSideLabel() = default;






	
		void setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds, juce::RectanglePlacement placement = juce::RectanglePlacement::centred) override;
		void resized() override;


		










	protected:



		JUCE_LEAK_DETECTOR(CircleSliderSideLabel)
	};
}// namnepace bdsp
