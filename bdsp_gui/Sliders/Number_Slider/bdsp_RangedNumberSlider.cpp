
namespace bdsp
{
	RangedNumberSlider::RangedNumberSlider(juce::Component* parent, GUI_Universals* universalsToUse, const juce::String& baseName, bool setListen)
		:NumberSlider(parent, universalsToUse, baseName, setListen), RangedSlider(universalsToUse)
	{
		setParent(this);
	}

	RangedNumberSlider::~RangedNumberSlider()
	{
	}





}// namnepace bdsp
//==============================================================================
