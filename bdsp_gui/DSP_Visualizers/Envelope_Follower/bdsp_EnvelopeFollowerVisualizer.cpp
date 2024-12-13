#include "bdsp_EnvelopeFollowerVisualizer.h"

namespace bdsp
{

	template<typename SampleType>
	inline EnvelopeFollowerVisualizer<SampleType>::EnvelopeFollowerVisualizer(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp)
		:OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>(universalsToUse, parentComp)
	{
	}

	template<typename SampleType>
	void EnvelopeFollowerVisualizer<SampleType>::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity)
	{
		OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>::vis->setColor(c, line, topCurveOpacity, botCurveOpacity);
	}

	template class EnvelopeFollowerVisualizer<float>;
	template class EnvelopeFollowerVisualizer<double>;
	//================================================================================================================================================================================================


	template<typename SampleType>
	EnvelopeFollowerVisualizerInternal<SampleType>::EnvelopeFollowerVisualizerInternal(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp)
		:OpenGLControlValuesOverTime(parentComp->control, universalsToUse, false)
	{
		setBipolar(false);
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




	template<typename SampleType>
	float EnvelopeFollowerVisualizerInternal<SampleType>::calculateAlpha(float x, float y)
	{
		return 0.5f;
	}


	template class EnvelopeFollowerVisualizerInternal<float>;
	template class EnvelopeFollowerVisualizerInternal<double>;



} // namespace bdsp
