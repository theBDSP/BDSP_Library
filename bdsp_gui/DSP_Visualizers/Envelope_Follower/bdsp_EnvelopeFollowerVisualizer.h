#pragma once



namespace bdsp
{
	template <typename SampleType>
	class EnvelopeFollowerVisualizerInternal final : public OpenGLControlValuesOverTime
	{
	public:
		EnvelopeFollowerVisualizerInternal(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp);
		~EnvelopeFollowerVisualizerInternal();


		void resized() override;



	private:











		float calculateAlpha(float x, float y) override; // calculates alpha of area under curve based on height of function

	



	};

	template <typename SampleType>
	class EnvelopeFollowerVisualizer : public OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>
	{
	public:
		EnvelopeFollowerVisualizer(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp);
		virtual ~EnvelopeFollowerVisualizer() = default;
		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity);



	};






} // namespace bdsp