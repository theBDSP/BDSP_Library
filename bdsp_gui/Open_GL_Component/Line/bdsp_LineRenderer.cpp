#include "bdsp_LineRenderer.h"
namespace bdsp
{
	OpenGLLineRenderer::OpenGLLineRenderer(GUI_Universals* universalsToUse, juce::Array<int> lineMaxPoints)
		:OpenGLComponent(universalsToUse)
	{
		numLines = lineMaxPoints.size();

		for (int i = 0; i < numLines; ++i)
		{
			lineVertexBuffer.add(new VertexArray());
			lineIndexBuffer.add(new IndexArray());

			lineVertexBuffer.getLast()->init(lineMaxPoints[i]);


			for (int j = 0; j < lineMaxPoints[i]; ++j)
			{
				lineIndexBuffer[i]->add(j);
			}

			lineScreenThickness.add(1);
			// set default values
			joint.set(i, OpenGLLineRenderer::JointType::Bevel);
			cap.set(i, OpenGLLineRenderer::CapType::Butt);
			thickRamp.set(i, 1);
		}

		lineVbo.resize(numLines);
		lineIbo.resize(numLines);


	}



	OpenGLLineRenderer::~OpenGLLineRenderer()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLLineRenderer::newOpenGLContextCreated()
	{
		//================================================================================================================================================================================================

		lineVertexShader = line_vertex_shader;

		lineFragmentShader = line_fragment_shader;

		lineGeometryShader = line_geometry_shader;



		jointVertexShader = line_joint_vertex_shader;

		jointFragmentShader = line_joint_fragment_shader;

		jointGeometryShader = line_joint_geometry_shader;




		lineShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		if (lineShaderProgram->addVertexShader(lineVertexShader)
			&& lineShaderProgram->addShader(lineGeometryShader, juce::gl::GL_GEOMETRY_SHADER)
			&& lineShaderProgram->addFragmentShader(lineFragmentShader)
			&& lineShaderProgram->link())
		{
			lineShaderProgram->use();
			thickness = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*lineShaderProgram.get(), "u_thickness");

			thicknessRamp = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*lineShaderProgram.get(), "u_thicknessRamp");
			numVerts = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*lineShaderProgram.get(), "u_numVerts");
			
			viewport = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*lineShaderProgram.get(), "u_viewport");
		}
		else
		{
			jassertfalse;
		}

		juce::gl::glGenBuffers(numLines, lineVbo.begin());

		juce::gl::glGenBuffers(numLines, lineIbo.begin());


		for (int i = 0; i < numLines; ++i)
		{

			// Bind the VBOs
			juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, lineVbo[i]);

			// Bind the IBOs
			juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, lineIbo[i]);
		}

		//================================================================================================================================================================================================

		jointShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		if (jointShaderProgram->addVertexShader(jointVertexShader)
			&& jointShaderProgram->addShader(jointGeometryShader, juce::gl::GL_GEOMETRY_SHADER)
			&& jointShaderProgram->addFragmentShader(jointFragmentShader)
			&& jointShaderProgram->link())
		{
			jointShaderProgram->use();
			jointThickness = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*jointShaderProgram.get(), "u_thickness");

			jointThicknessRamp = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*lineShaderProgram.get(), "u_thicknessRamp");

			jointType = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*jointShaderProgram.get(), "u_jointType");


			capType = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*jointShaderProgram.get(), "u_capType");

			jointNumVerts = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*jointShaderProgram.get(), "u_numVerts");
			jointViewport = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*jointShaderProgram.get(), "u_viewport");

		}
		else
		{
			jassertfalse;
		}


		//================================================================================================================================================================================================


		buffersCreated = true;


	}

	void OpenGLLineRenderer::renderWithoutGenerating()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		juce::gl::glEnable(juce::gl::GL_MULTISAMPLE); // enable MSAA


		juce::gl::glDepthFunc(juce::gl::GL_LESS);


		juce::gl::glEnable(juce::gl::GL_STENCIL_TEST);
		for (int i = 0; i < numLines; ++i)
		{
			lineShaderProgram->use();

			juce::gl::glEnable(juce::gl::GL_DEPTH_TEST); // depth testing for overdraw during a single shader pass

			juce::gl::glStencilMask(0xFF); // enable writing to the stencil buffer

			if (interLineOverdraw)
			{
				juce::gl::glClear(juce::gl::GL_DEPTH_BUFFER_BIT); // reset the depth buffer from the previous line
				juce::gl::glClear(juce::gl::GL_STENCIL_BUFFER_BIT); // reset the stencil buffer from the previous line
			}


			// set all fragments rendered this pass to 1 in the stencil buffer 
			juce::gl::glStencilOp(juce::gl::GL_KEEP, juce::gl::GL_KEEP, juce::gl::GL_REPLACE);
			juce::gl::glStencilFunc(juce::gl::GL_NOTEQUAL, 1, 0xFF); // only render fragments not rendered by the previous shader pass

			// grab the current vertex and index buffer
			auto& vb = *lineVertexBuffer[i];
			auto& ib = *lineIndexBuffer[i];

			// set this shaders uniforms
			thickness->set(lineScreenThickness[i]);
			thicknessRamp->set(thickRamp[i]);
			numVerts->set(vb.getNumVerticies());
			viewport->set(vpBounds.getWidth(), vpBounds.getHeight());

			juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, lineVbo[i]);

			// Send the vertices data.
			juce::gl::glBufferData(
				juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
				sizeof(float) * vb.size(),   // The size (in bytes) of the data.
				vb.data(),                    // A pointer to the actual data.
				juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
			);






			juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, lineIbo[i]);
			// Send the indices data.
			juce::gl::glBufferData(
				juce::gl::GL_ELEMENT_ARRAY_BUFFER,
				sizeof(unsigned int) * ib.size(),
				ib.data(),
				juce::gl::GL_STREAM_DRAW
			);
			//================================================================================================================================================================================================

			createVertexAttributes();

			//================================================================================================================================================================================================

			juce::gl::glDrawElements(
				juce::gl::GL_LINE_STRIP,
				ib.size(), // How many indices we have.
				juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
				nullptr             // We already gave OpenGL our indices so we don't
									// need to pass that again here, so pass nullptr.
			);

			//================================================================================================================================================================================================
			//================================================================================================================================================================================================
			//================================================================================================================================================================================================

			jointShaderProgram->use();

			juce::gl::glClear(juce::gl::GL_DEPTH_BUFFER_BIT); // reset the depth buffer from the previous pass


			juce::gl::glStencilFunc(juce::gl::GL_NOTEQUAL, 1, 0xFF); // only render fragments not rendered by the previous shader pass

			// set this shaders uniforms
			jointThickness->set(lineScreenThickness[i]);
			jointThicknessRamp->set(thickRamp[i]);
			jointType->set(joint[i]);
			capType->set(cap[i]);
			jointNumVerts->set(vb.getNumVerticies());
			jointViewport->set(vpBounds.getWidth(), vpBounds.getHeight());



			juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, lineVbo[i]);


			// Send the vertices data.
			juce::gl::glBufferData(
				juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
				sizeof(float) * vb.size(),   // The size (in bytes) of the data.
				vb.data(),                    // A pointer to the actual data.
				juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
			);






			juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, lineIbo[i]);
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
				juce::gl::GL_LINE_STRIP_ADJACENCY,
				ib.size(), // How many indices we have.
				juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
				nullptr             // We already gave OpenGL our indices so we don't
									// need to pass that again here, so pass nullptr.
			);


		}


		//================================================================================================================================================================================================

		juce::gl::glDisable(juce::gl::GL_STENCIL_TEST);
		juce::gl::glDisable(juce::gl::GL_DEPTH_TEST);

		removeVertexAttributes();

	}


	void OpenGLLineRenderer::setCapType(int idx, CapType newType)
	{
		if (idx < 0)
		{
			for (int i = 0; i < cap.size(); ++i)
			{
				cap.set(i, newType);
			}
		}
		else
		{
			cap.set(idx, newType);
		}
	}
	void OpenGLLineRenderer::setJointType(int idx, JointType newType)
	{
		if (idx < 0)
		{
			for (int i = 0; i < joint.size(); ++i)
			{
				joint.set(i, newType);
			}
		}
		else
		{
			joint.set(idx, newType);
		}
	}

	void OpenGLLineRenderer::setThickness(int idx, float thickness)
	{
		if (idx < 0)
		{
			for (int i = 0; i < lineScreenThickness.size(); ++i)
			{
				lineScreenThickness.set(i, thickness);
			}
		}
		else
		{
			lineScreenThickness.set(idx, thickness);
		}
		OpenGLLineRenderer::resized();
	}

	void OpenGLLineRenderer::setThicknessRamp(int idx, float newValue)
	{
		if (idx < 0)
		{
			for (int i = 0; i < thickRamp.size(); ++i)
			{
				thickRamp.set(i, newValue);
			}
		}
		else
		{
			thickRamp.set(idx, newValue);
		}
	}

	void OpenGLLineRenderer::setInterLineOverdraw(bool linesCanOverdrawEachOther)
	{
		interLineOverdraw = linesCanOverdrawEachOther;
	}


	void OpenGLLineRenderer::openGLContextClosing()
	{
		OpenGLComponent::openGLContextClosing();

		if (buffersCreated)
		{
			juce::gl::glDeleteBuffers(numLines, lineVbo.begin());
			juce::gl::glDeleteBuffers(numLines, lineIbo.begin());
		}
		lineShaderProgram.reset();
		jointShaderProgram.reset();
	}





} //namespace bdsp