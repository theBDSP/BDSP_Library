#pragma once



#define BDSP_CIRCLE_SLIDER_LABEL_HEIGHT_RATIO 0.55
#define BDSP_CIRCLE_SLIDER_LABEL_BORDER_RATIO -0.1

namespace bdsp
{

	class CircleSlider : public BaseSlider
	{
	public:
		CircleSlider(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit = true);
		CircleSlider(const CircleSlider& other);
		~CircleSlider();

		BaseSlider* getSliderReference();





		bool hitTest(int x, int y) override;

		void updateLabel() override;

		

		void setBoundsToIncludeLabel(int x, int y, int w, int h, juce::RectanglePlacement placement = juce::RectanglePlacement::centred);
		virtual void setBoundsToIncludeLabel(const juce::Rectangle<int>& newBounds, juce::RectanglePlacement placement = juce::RectanglePlacement::centred);
		void resized() override;
		void moved() override;


		juce::Rectangle<int> getTrackBounds(bool absolute = false);
		juce::Rectangle<int> getKnobBounds(bool absolute = false);


		void initText();




		void setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text = NamedColorsIdentifier(), const NamedColorsIdentifier& textHighlight = NamedColorsIdentifier(), const NamedColorsIdentifier& textEdit = NamedColorsIdentifier(), const NamedColorsIdentifier& caret = NamedColorsIdentifier(), const NamedColorsIdentifier& highlight = NamedColorsIdentifier()) override;





		void showTextEditor() override;



		void paint(juce::Graphics& g) override;

		void lookAndFeelChanged() override;




		Label label;






		void parentHierarchyChanged() override;





	protected:
		double labelWRatio = 1;



		bool isEditable;



		JUCE_LEAK_DETECTOR(CircleSlider)
	};
}// namnepace bdsp
