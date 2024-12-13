#pragma once



namespace bdsp
{

#define labelRatio 0.4
#define BDSP_RANGED_CIRCLE_SLIDER_INFLUENCE_RATIO 0.25f


	class RangedCircleSlider : public RangedSlider
	{
	public:
		RangedCircleSlider(GUI_Universals* universalsToUse, const juce::String& baseName , bool isEdit = true);
		//RangedCircleSlider(const RangedCircleSlider& other);
		~RangedCircleSlider();

		operator CircleSlider();
		operator CircleSlider* ();
		operator CircleSlider& ();

		void setNoLabel();



		void resized() override;


		CircleSlider slider;

		juce::RectanglePlacement boundsPlacement = juce::RectanglePlacement::centred;



		ComponentCore* getComponent() override;

	};


}// namnepace bdsp
