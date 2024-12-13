#pragma once



namespace bdsp
{


	class BitCrushVisualizer : public Component
	{
	public:
		BitCrushVisualizer(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider);
		void resized() override;
		void paint(juce::Graphics& g) override;
		void setColor(const NamedColorsIdentifier& newColor);

		void setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind);

		void visibilityChanged() override;

		class BitCrushVisualizerInternal : public OpenGLCompositeComponent
		{
		public:
			BitCrushVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider);
			virtual ~BitCrushVisualizerInternal();


		

			void resized() override;

			void setColor(const NamedColorsIdentifier& newColor);

		protected:




			void generateVertexBuffer() override;


		

			OpenGLColor color, gridColor;


			BaseSlider* bitDepth, * sampleRate, * mix;


			float rateVal, depthVal;
			float mixVal;

			float yScaling = 0.9f;

		

			OpenGLCirclePlotter* grid = nullptr;
			OpenGLLineRenderer* line = nullptr, *dry = nullptr;

			juce::Array<juce::Point<float>> dryPoints;

			float baseThickness;
		}vis;
	};



} // namespace bdsp