#pragma once

namespace bdsp
{
	// standardizes things like rendering curves, area under curve
	// great for visualizers (filter,distortion, lfo, etc.)
	class OpenGLFunctionVisualizer : public OpenGLLineRenderer
	{
	public:
		OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine = true, bool shouldRenderAreaUnderCurve = true, int numOfSamplePoints = 100);
		virtual ~OpenGLFunctionVisualizer();

		void setBipolar(bool newState);

		void resized() override;

		void setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line = NamedColorsIdentifier(), float topCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA, float botCurveOpacity = BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA);
		void setScaling(float newXScalingFactor, float newYScalingFactor = 0);
		void generateZeroLine();

		void initArrays(int numSamplePoints);

	protected:


		// Inherited via OpenGLComponent
		void generateVertexBuffer() override;
		virtual inline float calculateAlpha(float x, float y); // calculates alpha of area under curve based on height of function
		virtual inline float calculateFunctionSample(int sampleNumber, float openGL_X, float normX);
		virtual inline void newFrameInit(); // called at beginning of new frame calculation to grab any necessary values


		virtual void calculateZeroLine();

		bool isBipolar = false;
		bool hasZeroLine;
		OpenGLColor lineColor, zeroLine;

		int samplePoints = 2;

		int zeroLineStart;
		bool renderAreaUnderCurve = true;

		float topCurveAlpha = 0.5f, botCurveAlpha = 0.0f;


		float zeroY = 0.0f;
		float scalingX = 1.0f, scalingY = 1.0f, baseScalingX = 1.0f, baseScalingY = 1.0f;


		juce::NormalisableRange<float> xAxisMapping;

	};

} //namespace bdsp