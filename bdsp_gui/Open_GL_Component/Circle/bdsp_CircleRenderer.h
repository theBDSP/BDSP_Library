#pragma once
#include "Shaders\bdsp_CircleVertexShader.h"
#include "Shaders\bdsp_CircleGeometryShader.h"
#include "Shaders\bdsp_CircleFragmentShader.h"


namespace bdsp
{
	/**
	 * Renders Circles (Ellipses), each circle's position, radii, and color can be set individually.
	 * N.B. radii are set in vertex buffer as pixel values not OpenGL space values
	 */
	class OpenGLCircleRenderer : public OpenGLComponent
	{
	public:

		OpenGLCircleRenderer(GUI_Universals* universalsToUse, int maxCircles = 100);
		virtual ~OpenGLCircleRenderer();

		void resized() override;

		void createShaders() override;
		void createUniforms() override;


		void renderWithoutGenerating() override;

		void newOpenGLContextCreated() override;
		void openGLContextClosing() override;

		void createVertexAttributes() override;
		void removeVertexAttributes() override;


		VertexArray circleVertexBuffer;
		IndexArray circleIndexBuffer;

	protected:

		const char* circleVertexShader;
		const char* circleGeometryShader;
		const char* circleFragmentShader;


		std::unique_ptr<juce::OpenGLShaderProgram> circleShaderProgram;

		GLuint circleVbo; // Vertex buffer objects.
		GLuint circleIbo; // Index buffer objects.


	private:
		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> viewportUniform;


	};
} //namespace bdsp