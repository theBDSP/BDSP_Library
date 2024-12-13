#pragma once


#define BDSP_DIFFUSOR_VIS_BASE_WIDTH 0.25f
namespace bdsp
{

	class DiffusorVisualizer : public Component
	{
	public:
		DiffusorVisualizer(GUI_Universals* universalsToUse, BaseSlider* timeSlider, BaseSlider* mixSlider);
		void resized() override;
		void paint(juce::Graphics& g) override;
		void setColor(const NamedColorsIdentifier& newColor);

		class DiffusorVisualizerInternal : public OpenGLCirclePlotter
		{
		public:
			DiffusorVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* timeSlider, BaseSlider* mixSlider);
			virtual ~DiffusorVisualizerInternal();
		

			void setColor(const NamedColorsIdentifier& newColor);
			

			
		protected:


	


			void generateVertexBuffer() override;
		

			OpenGLColor color;


		
			

			juce::Array<juce::Point<float>> baseVerticies;
			juce::Array<juce::Point<float>> displacementVectors;

			int rows = 25;
			int cols = 25;

			BaseSlider* time, * mix;

			float mixVal, timeVal;
			double aspectRatio;


		

		}vis;
	};




} // namespace bdsp