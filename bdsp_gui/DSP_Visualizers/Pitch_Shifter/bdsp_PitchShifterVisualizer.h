#pragma once




namespace bdsp
{
	class PitchShifterVisualizer : public Component
	{
	public:
		PitchShifterVisualizer(GUI_Universals* universalsToUse, BaseSlider* leftAmt, BaseSlider* rightAmt, BaseSlider* mixAmt);
		void resized() override;
		void paint(juce::Graphics& g) override;

		void setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newDryColor);

		void linkAmounts(bool shouldLink);
		void setBarWidth(float barRatio, float scaleRatio);
		void setDotSize(float dotRatio);


		void setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind);





		class PitchShifterVisualizerInternal : public OpenGLCompositeComponent
		{
		public:
			PitchShifterVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* leftAmt, BaseSlider* rightAmt, BaseSlider* mixAmt);
			~PitchShifterVisualizerInternal();

			void resized() override;
			void setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newScaleColor, const NamedColorsIdentifier& newTextColor = NamedColorsIdentifier(BDSP_COLOR_WHITE));

			void linkAmounts(bool shouldLink);
			void setBarWidth(float barRatio, float scaleRatio);
			void setDotSize(float dotRatio);


			void paintOverChildren(juce::Graphics& g) override;

	
		private:
			//TODO switch to parameter references

			BaseSlider* left, * right, * mix;


			float leftVal, rightVal, mixVal;


			float scaleX = 0.9, scaleY = 0.9;
			float gap = 0.2;
			float barSize = -1.0f, scaleSize = -1.0f, dotSize = -1.0f;
			void generateVertexBuffer() override;
			void onShaderCreation() override;





			bool amountsLinked = false;



			OpenGLColor color, scaleColor, textColor;

			OpenGLCirclePlotter* dots = nullptr;
			OpenGLLineRenderer* scale = nullptr;
			OpenGLLineRenderer* bars = nullptr;
		}vis;
	};





} // namespace bdsp
