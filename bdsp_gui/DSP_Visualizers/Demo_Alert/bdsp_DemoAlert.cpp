#if BDSP_DEMO_VERSION
#include "bdsp_DemoAlert.h"
#include "bdsp_processor_and_editor/bdsp_processor_and_editor.h"
namespace bdsp
{
template <typename SampleType>
DemoAlert<SampleType>::DemoAlert(Editor* editorToUse, dsp::DemoTimer<SampleType>* timer)
    :dsp::DemoTimer<SampleType>::Listener(timer)
{
    editor = editorToUse;
    Header = "Audio Silenced in Demo Version";



    setNewTimes(timer->normalTimer.getLengthInMs() / 1000, timer->silenceTimer.getLengthInMs() / 1000);

}


template <typename SampleType>
void DemoAlert<SampleType>::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (dsp::DemoTimer<SampleType>::Listener::p->isBypassed()) // if not silencing
    {
        editor->demoDisable(true);
    }
    else // if silencing
    {
        editor->demoDisable(false);
    }

}

template class DemoAlert<float>;
template class DemoAlert<double>;
}
#endif