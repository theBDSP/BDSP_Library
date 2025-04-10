#pragma once
#include "Shaders\bdsp_LineVertexShader.h"
#include "Shaders\bdsp_LineGeometryShader.h"
#include "Shaders\bdsp_LineFragmentShader.h"

#include "Shaders\bdsp_LineJointVertexShader.h"
#include "Shaders\bdsp_LineJointGeometryShader.h"
#include "Shaders\bdsp_LineJointFragmentShader.h"

namespace bdsp
{
	/**
	 * Renders lines with standard joint and cap options, variable line width, and per vertex colors.
	 * This class is designed for lines that do not cross over themselves (it can render these without overdraw, it does often cause noticible artifacting), but may otherwise have overdraw due to the width of the line.
	 */
	class OpenGLLineRenderer : public OpenGLComponent
	{
	public:

		enum CapType : int { Butt, Square, Round };
		enum JointType : int { Miter, Bevel, Rounded };

		OpenGLLineRenderer(GUI_Universals* universalsToUse, juce::Array<int> lineMaxPoints = juce::Array<int>(100));
		virtual ~OpenGLLineRenderer();





		void renderWithoutGenerating() override;

		void newOpenGLContextCreated() override;
		void openGLContextClosing() override;

		/**
		* @param idx Index of line to set the cap type for, negative value will change all lines
		*/
		void setCapType(int idx, CapType newType);

		/**
		 * @param idx Index of line to set the joint type for, negative value will change all lines
		 */
		void setJointType(int idx, JointType newType);

		/**
		 * @param idx Index of line to set width of, negative value will change all lines
		 */
		void setThickness(int idx, float thickness);

		/**
		 * Multiplies the thickness of the line at each vertex by a linearly changing value.
		 * Negative values decrease the width from 1 to abs(value)
		 * Positive values increase the width from value to 1
		 * value of 1 means no change in width
		 */
		void setThicknessRamp(int idx, float newValue);


		/**
		 * Sets whether seperate line test for and eliminate overdraws bewteen each other
		 * @param linesCanOverdrawEachOther If true, speperate lines will not check or eliminate overdraw between them
		 */
		void setInterLineOverdraw(bool linesCanOverdrawEachOther);

		juce::Array<float> lineScreenThickness; // line thicknesses in pixels


		juce::OwnedArray<VertexArray> lineVertexBuffer;
		juce::OwnedArray<IndexArray> lineIndexBuffer;

	protected:





		const char* lineVertexShader;
		const char* lineGeometryShader;
		const char* lineFragmentShader;


		const char* jointVertexShader;
		const char* jointGeometryShader;
		const char* jointFragmentShader;

		std::unique_ptr<juce::OpenGLShaderProgram> lineShaderProgram, jointShaderProgram;

		juce::Array<GLuint> lineVbo; // Vertex buffer objects.
		juce::Array<GLuint> lineIbo; // Index buffer objects.


	private:





		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> thickness, numVerts, thicknessRamp, viewport; // line shader uniforms
		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> jointThickness, jointType, capType, jointNumVerts, jointThicknessRamp, jointViewport; // joint shader uniforms
		juce::Array<CapType> cap;
		juce::Array<JointType> joint;
		juce::Array<float> thickRamp;

		bool interLineOverdraw = true; // if true each line will have seperate depth and stencil tests allowing for overdraw between seperate lines

	protected:
		int numLines = 1;

	};
} //namespace bdsp