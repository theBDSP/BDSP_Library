#pragma once



namespace bdsp
{
#define ptrRatio 0.25


	class RangedBarSlider : public RangedSlider
	{
	public:
		RangedBarSlider(GUI_Universals* universalsToUse, const juce::String& baseName);
		~RangedBarSlider();

		operator BarSlider();
		operator BarSlider* ();
		operator BarSlider& ();



		void paint(juce::Graphics& g) override;

		void resized() override;

		BarSlider slider;
	};


}// namnepace bdsp

