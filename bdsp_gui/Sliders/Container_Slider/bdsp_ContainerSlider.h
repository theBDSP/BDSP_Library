#pragma once



#define BDSP_CONTAINER_SLIDER_LABEL_HEIGHT_RATIO 0.75
#define BDSP_CONTAINER_SLIDER_LABEL_WIDTH_RATIO 0.75

namespace bdsp
{


	class ContainerSlider : public BaseSlider
	{
	public:

		ContainerSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		ContainerSlider(const ContainerSlider& other);
		~ContainerSlider();










		void resized() override;


		void initText();




		void setSliderColors(const NamedColorsIdentifier& knob, const NamedColorsIdentifier& value, const NamedColorsIdentifier& valueTrack, const NamedColorsIdentifier& valueTrackInside, const NamedColorsIdentifier& text = NamedColorsIdentifier(), const NamedColorsIdentifier& textHighlight = NamedColorsIdentifier(), const NamedColorsIdentifier& textEdit = NamedColorsIdentifier(), const NamedColorsIdentifier& caret = NamedColorsIdentifier(), const NamedColorsIdentifier& highlight = NamedColorsIdentifier()) override;





		void paint(juce::Graphics& g) override;


		Label label;


		void setIsVertical(bool shouldBeVertical);
		bool getIsVertical();

		void setCornerCurves(CornerCurves curveValues);



	protected:


		bool isVertical = false;


		CornerCurves curvesVal = CornerCurves(topLeft | topRight | bottomLeft | bottomRight); // defualt all curves

		JUCE_LEAK_DETECTOR(ContainerSlider)
	};

} // namespace bdsp