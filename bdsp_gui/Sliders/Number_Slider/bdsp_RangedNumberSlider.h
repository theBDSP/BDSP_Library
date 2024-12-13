#pragma once



namespace bdsp
{
	class RangedNumberSlider : public NumberSlider, public RangedSlider
	{
	public:
		RangedNumberSlider(juce::Component* parent, GUI_Universals* universalsToUse, const juce::String& baseName, bool setListen = true);
		~RangedNumberSlider() override;





	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RangedNumberSlider)
	};

}// namnepace bdspnamespace bdsp

