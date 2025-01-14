#pragma once



namespace bdsp
{
	class BitCrushVisualizerInternal : public OpenGLCompositeComponent
	{
	public:
		BitCrushVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider);
		virtual ~BitCrushVisualizerInternal();




		void resized() override;

		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float topCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float botCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA);

		void setScaling(float x, float y);

	protected:




		void generateVertexBuffer() override;




		OpenGLColor color, gridColor;


		BaseSlider* bitDepth, * sampleRate, * mix;


		float rateVal, depthVal;
		float mixVal;

		float xScaling = 0.9f, yScaling = 0.9f;



		OpenGLCirclePlotter* grid = nullptr;
		OpenGLLineRenderer* line = nullptr, * dry = nullptr;

		juce::Array<juce::Point<float>> dryPoints;

		float baseThickness;
	};


	class BitCrushVisualizer : public OpenGlComponentWrapper<BitCrushVisualizerInternal>
	{
	public:
		BitCrushVisualizer(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider);


	};



} // namespace bdsp