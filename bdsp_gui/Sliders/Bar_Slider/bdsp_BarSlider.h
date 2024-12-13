#pragma once



#define BDSP_BAR_SLIDER_LABEL_HEIGHT_RATIO 0.5
#define BDSP_BAR_SLIDER_LABEL_BORDER_RATIO 0.1

namespace bdsp
{

	class BarSlider : public BaseSlider
	{
	public:
		BarSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		BarSlider(const BarSlider& other);

		~BarSlider();
		void setVertical(bool isVertical);

		void setBoundsToIncludeLabel(int x, int y, int w, int h);
		void setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds);

		void resized() override;


		void lookAndFeelChanged() override;

		void setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack = NamedColorsIdentifier(), const NamedColorsIdentifier& valueTrackInside=NamedColorsIdentifier(), const NamedColorsIdentifier& text = NamedColorsIdentifier(), const NamedColorsIdentifier& textHighlight = NamedColorsIdentifier(), const NamedColorsIdentifier& textEdit = NamedColorsIdentifier(), const NamedColorsIdentifier& caret = NamedColorsIdentifier(), const NamedColorsIdentifier& highlight = NamedColorsIdentifier()) override;




		void disableLabels();

		void paint(juce::Graphics& g) override;


		void parentHierarchyChanged() override;


		Label label;

	protected:
		int labelBottom = 0;
	};


}// namnepace bdsp

