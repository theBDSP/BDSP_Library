#include "bdsp_CircleRenderer.h"
namespace bdsp
{
	OpenGLCircleRenderer::OpenGLCircleRenderer(GUI_Universals* universalsToUse, int maxCircles)
		:OpenGLComponent(universalsToUse, 8),
		circleVertexBuffer(8)
	{
		for (int i = 0; i < maxCircles; ++i)
		{
			circleIndexBuffer.add(i);
		}
	}



	OpenGLCircleRenderer::~OpenGLCircleRenderer()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLCircleRenderer::newOpenGLContextCreated()
	{
		//================================================================================================================================================================================================

		circleVertexShader = circle_vertex_shader;

		circleFragmentShader = circle_fragment_shader;

		circleGeometryShader = circle_geometry_shader;






		circleShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		if (circleShaderProgram->addVertexShader(circleVertexShader)
			&& circleShaderProgram->addShader(circleGeometryShader, juce::gl::GL_GEOMETRY_SHADER)
			&& circleShaderProgram->addFragmentShader(circleFragmentShader)
			&& circleShaderProgram->link())
		{
			circleShaderProgram->use();
			viewportUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*circleShaderProgram.get(), "u_viewport");
			viewportUniform->set(vpBounds.getWidth(), vpBounds.getHeight());
		}
		else
		{
			jassertfalse;
		}

		juce::gl::glGenBuffers(1, &circleVbo);

		juce::gl::glGenBuffers(1, &circleIbo);


		// Bind the VBOs
		juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, circleVbo);

		// Bind the IBOs
		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, circleIbo);


		buffersCreated = true;


	}

	void OpenGLCircleRenderer::renderWithoutGenerating()
	{

		circleShaderProgram->use();

		viewportUniform->set(vpBounds.getWidth(), vpBounds.getHeight());

		juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, circleVbo);

		auto& vb = circleVertexBuffer;
		auto& ib = circleIndexBuffer;

		// Send the vertices data.
		juce::gl::glBufferData(
			juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
			sizeof(float) * vb.size(),   // The size (in bytes) of the data.
			vb.data(),                    // A pointer to the actual data.
			juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
		);






		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, circleIbo);
		// Send the indices data.
		juce::gl::glBufferData(
			juce::gl::GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * ib.size(),
			ib.data(),
			juce::gl::GL_STREAM_DRAW
		);
		//================================================================================================================================================================================================


		removeVertexAttributes();
		createVertexAttributes();

		//================================================================================================================================================================================================

		juce::gl::glDrawElements(
			juce::gl::GL_POINTS,
			ib.size(), // How many indices we have.
			juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
			nullptr             // We already gave OpenGL our indices so we don't
								// need to pass that again here, so pass nullptr.
		);




		//================================================================================================================================================================================================


		removeVertexAttributes();
	}




	void OpenGLCircleRenderer::openGLContextClosing()
	{
		OpenGLComponent::openGLContextClosing();

		if (buffersCreated)
		{
			juce::gl::glDeleteBuffers(1, &circleVbo);
			juce::gl::glDeleteBuffers(1, &circleIbo);
		}
		circleShaderProgram.reset();
	}

	void OpenGLCircleRenderer::createVertexAttributes()
	{
		juce::gl::glEnableVertexAttribArray(0);
		// Enable the position attribute.
		juce::gl::glVertexAttribPointer(0, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, numAttributes * sizeof(float), nullptr);


		juce::gl::glEnableVertexAttribArray(1);
		// Enable the radius attribute.
		juce::gl::glVertexAttribPointer(1, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, numAttributes * sizeof(float), (GLvoid*)(sizeof(float) * 2));


		juce::gl::glEnableVertexAttribArray(2);
		// Enable the colour attribute.
		juce::gl::glVertexAttribPointer(2, 4, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, numAttributes * sizeof(float), (GLvoid*)(sizeof(float) * 4));

	}

	void bdsp::OpenGLCircleRenderer::removeVertexAttributes()
	{
		juce::gl::glDisableVertexAttribArray(0);
		juce::gl::glDisableVertexAttribArray(1);
		juce::gl::glDisableVertexAttribArray(2);
	}



	void OpenGLCircleRenderer::resized()
	{
		OpenGLComponent::resized();

		if (viewportUniform.operator bool())
		{
			viewportUniform->set(vpBounds.getWidth(), vpBounds.getHeight());
		}
	}




} //namespace bdsp