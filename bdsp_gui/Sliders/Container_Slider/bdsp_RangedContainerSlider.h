#pragma once



namespace bdsp
{
	class RangedContainerSlider : public RangedSlider
	{
	public:
		RangedContainerSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		//RangedContainerSlider(const RangedContainerSlider& other);
		~RangedContainerSlider();

		operator ContainerSlider();
		operator ContainerSlider* ();
		operator ContainerSlider& ();


		void resized() override;


		ContainerSlider slider;


	};


}// namnepace bdsp
