#pragma once
#include "Shaders/bdsp_FunctionVisualizerVertexShader.h"
#include "Shaders/bdsp_FunctionVisualizerGeometryShader.h"
#include "Shaders/bdsp_FunctionVisualizerFragmentShader.h"
namespace bdsp
{
	/**
	 * Renders a function at regularly spaced intervals.
	 * The area under the curve can optionally be rendered as well, with many color options
	 */
	class OpenGLFunctionVisualizer : public OpenGLLineRenderer
	{
	public:
		OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine = true, bool shouldRenderAreaUnderCurve = true, int numOfSamplePoints = 100);
		virtual ~OpenGLFunctionVisualizer();

		void setBipolar(bool newState);

		void resized() override;

		/**
		 * Sets colors for all parts of the visualization
		 * @param newLineColor color of the function curve
		 * @param newZeroLineColor color of the zero line -  defaults to transparent
		 * @param newTopCurveColor color of the area under the curve at y=1 (and y=-1 when bipolar) - defaults to newLineColor with alpha multiplied by default macro (BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA)
		 * @param newBotCurveColor color of the area under the curve at the zero line - defaults to newLineColor with alpha multiplied by default macro (BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA)
		 * @param newPosColor color of the position information added to the area under the curve - defaults to newLineColor with alpha multiplied by 1- the maixmum alpha between the top and bot curve colors
		 */
		void setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newTopCurveColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newBotCurveColor = NamedColorsIdentifier(), const NamedColorsIdentifier& newPosColor = NamedColorsIdentifier());
		void setScaling(float newXScalingFactor, float newYScalingFactor = 0);
		juce::Point<float> getScaling();
		void generateZeroLine();

		void renderWithoutGenerating() override;

		void newOpenGLContextCreated() override;
		void openGLContextClosing() override;

		void generateVertexBuffer() override;

		void initArrays(int numOfSamplePoints);

		void setFillPos(float newFillPos);
		void setEndpoint(float newEndpoint);

		void paintOverChildren(juce::Graphics& g) override;

		void visibilityChanged() override;

		OpenGLColor topCurve, botCurve;
	protected:

		const char* fillVertexShader;
		const char* fillGeometryShader;
		const char* fillFragmentShader;

		std::unique_ptr<juce::OpenGLShaderProgram> fillShaderProgram;
		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> zeroLineY, topColor, botColor, posColor, pos, width, endpoint; // fill shader uniforms


		// Inherited via OpenGLComponent
		virtual inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX);
		virtual inline void newFrameInit(); // called at beginning of new frame calculation to grab any necessary values


		virtual void calculateZeroLine();

		bool isBipolar = false;
		bool hasZeroLine;
		OpenGLColor lineColor, zeroLine;
		OpenGLColor positionColor;

		int samplePoints = 2;

		bool renderAreaUnderCurve = true;


		float fillPos = -1.0f;
		float fillWidth = 0.0f;
		float zeroY = 0.0f;
		float scalingX = 1.0f, scalingY = 1.0f, baseScalingX = 1.0f, baseScalingY = 1.0f;
		float endPointVal = 1.0f;

		juce::NormalisableRange<float> xAxisMapping;

	};

} //namespace bdsp