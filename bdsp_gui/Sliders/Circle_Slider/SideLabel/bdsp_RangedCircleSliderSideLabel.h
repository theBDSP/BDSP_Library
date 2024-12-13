#pragma once



namespace bdsp
{


	class RangedCircleSliderSideLabel : public RangedSlider
	{
	public:
		RangedCircleSliderSideLabel(GUI_Universals* universalsToUse, const juce::String& baseName, bool isEdit = true);
		//RangedCircleSliderSideLabel(const RangedCircleSliderSideLabel& other);
		~RangedCircleSliderSideLabel();

		operator CircleSliderSideLabel();
		operator CircleSliderSideLabel* ();
		operator CircleSliderSideLabel& ();




		void resized() override;

		ComponentCore* getHoverMenuAnchor() override;


		CircleSliderSideLabel slider;


	};


}// namnepace bdsp
