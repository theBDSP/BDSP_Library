#include "bdsp_envelopeFollowerComponent.h"

namespace bdsp
{
	
	
	template class EnvelopeFollowerParameterListener<float>;
	template class EnvelopeFollowerParameterListener<double>;

	//================================================================================================================================================================================================

	//================================================================================================================================================================================================


	
	template class EnvelopeFollowerComponent<float>;
	template class EnvelopeFollowerComponent<double>;
	//================================================================================================================================================================================================

template<typename SampleType>
EnvelopeFollowerSection<SampleType>::EnvelopeFollowerSection(GUI_Universals* universalsToUse, dsp::SampleSourceList<SampleType>* list, const NamedColorsIdentifier& backgroundColor, juce::ValueTree* nameValueLocation)
    :TexturedContainerComponent(universalsToUse, backgroundColor)
{
    auto proc = dynamic_cast<Processor*>(&universalsToUse->APVTS->processor);
    ;
    for (int i = 0; i < BDSP_NUMBER_OF_ENVELOPE_FOLLOWERS; ++i)
    {
        followers.add(std::make_unique<EnvelopeFollowerComponent<SampleType>>(universalsToUse, list, nameValueLocation, i));
        addAndMakeVisible(followers[i]);
        followers.getLast()->setColor(BDSP_COLOR_WHITE, BDSP_COLOR_COLOR);


        followers.getLast()->getSourceComponent()->getListuniversals()->onShow = [=]()
        {
            getFollower(i)->setSourceEnablement("Sidechain", proc->sidechainEnabled);
        };


        //================================================================================================================================================================================================
        auto prevSelect = getFollower(i)->getSourceComponent()->getListComponent()->onSelect;
        followers.getLast()->getSourceComponent()->getListComponent()->onSelect = [=](int idx)
        {
            getFollower(i)->setSourceEnablement("Sidechain", proc->sidechainEnabled);
            if (prevSelect.operator bool())
            {
                prevSelect(idx);
            }
        };


    }

    if (proc->sidechainExists)
    {
        startTimer(1000);
    }
}

template<typename SampleType>
void EnvelopeFollowerSection<SampleType>::timerCallback()
{
    for (auto f : followers)
    {
        f->setSourceEnablement("Sidechain", dynamic_cast<Processor*>(&universals->APVTS->processor)->sidechainEnabled);
    }


}
	
template class EnvelopeFollowerSection<float>;
template class EnvelopeFollowerSection<double>;

} // namespace bdsp
