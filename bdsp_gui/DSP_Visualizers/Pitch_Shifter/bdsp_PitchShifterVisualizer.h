#pragma once
namespace bdsp
{
	/**
	 * Represents the state of a pitch shifter
	 */
	class PitchShifterVisualizerInternal : public OpenGLCompositeComponent
	{
	public:
		PitchShifterVisualizerInternal(GUI_Universals* universalsToUse, Parameter* leftAmt, Parameter* rightAmt, Parameter* mixAmt);
		~PitchShifterVisualizerInternal();

		void renderOpenGL() override;
		void paintOverChildren(juce::Graphics& g) override;

		void setColor(const NamedColorsIdentifier& newColor, const NamedColorsIdentifier& newScaleColor = BDSP_COLOR_MID);
		void linkAmounts(bool shouldLink);
		void resized() override;

	private:

		Parameter* left, * right, * mix;


		float leftVal, rightVal, mixVal;

		float scaleX = 0.9, scaleY = 0.8;

		void generateVertexBuffer() override;

		bool amountsLinked = false;


		OpenGLColor color, scaleColor;
		const int numPoints = 50; // number of points to render in each tail
		OpenGLLineRenderer* scale, * trigTails;
		OpenGLCircleRenderer* thumbs, * dryThumbs;
		float time = 0;
		float gap = 0.1f; // proportion of the width of the visualizer to cutout for the semiton numbers


	};

	class PitchShifterVisualizer : public OpenGlComponentWrapper<PitchShifterVisualizerInternal>
	{
	public:
		PitchShifterVisualizer(GUI_Universals* universalsToUse, Parameter* leftAmt, Parameter* rightAmt, Parameter* mixAmt)
			:OpenGlComponentWrapper<PitchShifterVisualizerInternal>(universalsToUse, leftAmt, rightAmt, mixAmt)
		{

		}

	};




} // namespace bdsp
