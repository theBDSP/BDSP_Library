#include "bdsp_EnvelopeFollowerVisualizer.h"

namespace bdsp
{

	template<typename SampleType>
	inline EnvelopeFollowerVisualizer<SampleType>::EnvelopeFollowerVisualizer(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp)
		:OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>(universalsToUse, parentComp)
	{
	}

	template<typename SampleType>
	void EnvelopeFollowerVisualizer<SampleType>::setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor , const NamedColorsIdentifier& newTopCurveColor, const NamedColorsIdentifier& newBotCurveColor)

	{
		OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>::vis->setColor(newLineColor, newZeroLineColor, newTopCurveColor, newBotCurveColor);
	}

	template class EnvelopeFollowerVisualizer<float>;
	template class EnvelopeFollowerVisualizer<double>;
	//================================================================================================================================================================================================


	template<typename SampleType>
	EnvelopeFollowerVisualizerInternal<SampleType>::EnvelopeFollowerVisualizerInternal(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp)
		:OpenGLControlValuesOverTime(parentComp->control, universalsToUse, false)
	{
		setBipolar(false);
		setJointType(0,JointType::Bevel);
		setCapType(0,CapType::Butt);
		alphaFalloff = false;
		forceFullRange = true;


	}

	template<typename SampleType>
	EnvelopeFollowerVisualizerInternal<SampleType>::~EnvelopeFollowerVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}



	template<typename SampleType>
	void EnvelopeFollowerVisualizerInternal<SampleType>::resized()
	{
		OpenGLControlValuesOverTime::resized();
	}





	template class EnvelopeFollowerVisualizerInternal<float>;
	template class EnvelopeFollowerVisualizerInternal<double>;



} // namespace bdsp
