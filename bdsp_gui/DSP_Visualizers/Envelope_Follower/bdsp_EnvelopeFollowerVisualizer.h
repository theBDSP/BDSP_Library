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












	};

	template <typename SampleType>
	class EnvelopeFollowerVisualizer : public OpenGlComponentWrapper<EnvelopeFollowerVisualizerInternal<SampleType>>
	{
	public:
		EnvelopeFollowerVisualizer(GUI_Universals* universalsToUse, EnvelopeFollowerComponent<SampleType>* parentComp);
		virtual ~EnvelopeFollowerVisualizer() = default;
		void setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newTopCurveColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newBotCurveColor = NamedColorsIdentifier());



	};






} // namespace bdsp