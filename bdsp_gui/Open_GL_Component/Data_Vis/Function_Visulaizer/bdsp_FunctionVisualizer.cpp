#include "bdsp_FunctionVisualizer.h"
namespace bdsp
{
	OpenGLFunctionVisualizer::OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine, bool shouldRenderAreaUnderCurve, int numOfSamplePoints)
		:OpenGLLineRenderer(universalsToUse, numOfSamplePoints),
		lineColor(universalsToUse, this),
		zeroLine(universalsToUse, this),
		topCurve(universalsToUse, this),
		botCurve(universalsToUse, this),
		positionColor(universalsToUse, this)
	{
		hasZeroLine = shouldHaveZeroLine;
		renderAreaUnderCurve = shouldRenderAreaUnderCurve;

		samplePoints = numOfSamplePoints;
		//================================================================================================================================================================================================
		// zero line
		// can render as a simple quad b/c it will alywas be perfectly horizontal
		vertexBuffer.init(4);
		indexBuffer.clear();

		indexBuffer.addArray(
			{
				0,1,2,
				1,2,3
			}
		);
		//================================================================================================================================================================================================

		initArrays(numOfSamplePoints);
	}

	OpenGLFunctionVisualizer::~OpenGLFunctionVisualizer()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLFunctionVisualizer::setBipolar(bool newState)
	{
		if (isBipolar != newState)
		{
			isBipolar = newState;
			calculateZeroLine();
		}

	}



	void OpenGLFunctionVisualizer::resized()
	{
		if (!getBounds().isEmpty())
		{

			setThickness(0, universals->visualizerLineThickness);

			OpenGLLineRenderer::resized();

			// account for line thickness when setting scaling factors to avoid curve extending beyond the intended bounds
			scalingX = baseScalingX - lineScreenThickness.getFirst() / getWidth();
			scalingY = baseScalingY - lineScreenThickness.getFirst() / getHeight();
			calculateZeroLine();

		}
	}


	void OpenGLFunctionVisualizer::setColor(const NamedColorsIdentifier& newLineColor, const NamedColorsIdentifier& newZeroLineColor, const NamedColorsIdentifier& newTopCurveColor, const NamedColorsIdentifier& newBotCurveColor, const NamedColorsIdentifier& newPosColor)
	{
		lineColor.setColors(newLineColor, newLineColor.withMultipliedAlpha(universals->disabledAlpha));

		zeroLine.setColors(newZeroLineColor, newZeroLineColor.withMultipliedAlpha(universals->disabledAlpha));

		if (newTopCurveColor.isEmpty())
		{
			topCurve.setColors(newLineColor.withMultipliedAlpha(BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA), newLineColor.withMultipliedAlpha(BDSP_OPEN_GL_FUNCTION_DEFAULT_TOP_ALPHA * universals->disabledAlpha));
		}
		else
		{
			topCurve.setColors(newTopCurveColor, newTopCurveColor.withMultipliedAlpha(universals->disabledAlpha));
		}
		if (newBotCurveColor.isEmpty())
		{
			botCurve.setColors(newLineColor.withMultipliedAlpha(BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA), newLineColor.withMultipliedAlpha(BDSP_OPEN_GL_FUNCTION_DEFAULT_BOTTOM_ALPHA * universals->disabledAlpha));
		}
		else
		{
			botCurve.setColors(newBotCurveColor, newBotCurveColor.withMultipliedAlpha(universals->disabledAlpha));
		}
		if (newPosColor.isEmpty())
		{
			float a = 1 - juce::jmax(topCurve.getAlpha(), botCurve.getAlpha());
			positionColor.setColors(newLineColor.withMultipliedAlpha(a), newLineColor.withMultipliedAlpha(a * universals->disabledAlpha));
		}
		else
		{
			positionColor.setColors(newPosColor, newPosColor.withMultipliedAlpha(universals->disabledAlpha));
		}

		generateZeroLine();
		resized();
	}


	void OpenGLFunctionVisualizer::setScaling(float newXScalingFactor, float newYScalingFactor)
	{
		baseScalingX = newXScalingFactor;
		baseScalingY = newYScalingFactor == 0 ? newXScalingFactor : newYScalingFactor;

		resized();
	}

	void OpenGLFunctionVisualizer::generateZeroLine()
	{
		if (hasZeroLine)
		{
			float r, g, b, a;
			zeroLine.getComponents(r, g, b, a);

			float dH = lineScreenThickness.getFirst() / (4 * getHeight());
			vertexBuffer.set(0,
				{
					-scalingX, zeroY + dH,
					r,g,b,a
				});

			vertexBuffer.set(1,
				{
					scalingX, zeroY + dH,
					r,g,b,a
				});
			vertexBuffer.set(2,
				{
					-scalingX, zeroY - dH,
					r,g,b,a
				});

			vertexBuffer.set(3,
				{
					scalingX, zeroY - dH,
					r,g,b,a
				});
		}

	}

	void OpenGLFunctionVisualizer::renderWithoutGenerating()
	{

		OpenGLLineRenderer::renderWithoutGenerating();
		if (hasZeroLine)
		{
			juce::gl::glStencilMask(0x00); // disable writing to the stencil buffer
			OpenGLComponent::renderWithoutGenerating();
		}
		if (renderAreaUnderCurve)
		{
			juce::gl::glEnable(juce::gl::GL_STENCIL_TEST);
			juce::gl::glStencilFunc(juce::gl::GL_NOTEQUAL, 1, 0xFF); // only render fragments not rendered by the line shader pass

			// fill shader takes in same vertex and index buffers as line shader
			auto& vb = *lineVertexBuffer[0];
			auto& ib = *lineIndexBuffer[0];

			fillShaderProgram->use();

			zeroLineY->set(zeroY); // set zero line uniform for fragment and geometry shaders

			//================================================================================================================================================================================================
			// set color uniforms for fragment shader
			float r, g, b, a;
			topCurve.getComponents(r, g, b, a);
			topColor->set(r, g, b, a);

			botCurve.getComponents(r, g, b, a);
			botColor->set(r, g, b, a);

			positionColor.getComponents(r, g, b, a);
			posColor->set(r, g, b, a);
			//================================================================================================================================================================================================


			juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, lineVbo[0]);
			// Send the vertices data.
			juce::gl::glBufferData(
				juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
				sizeof(float) * vb.size(),   // The size (in bytes) of the data.
				vb.data(),                    // A pointer to the actual data.
				juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
			);






			juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, lineIbo[0]);
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
		}
		juce::gl::glDisable(juce::gl::GL_STENCIL_TEST);
		juce::gl::glDisable(juce::gl::GL_DEPTH_TEST);
	}

	void OpenGLFunctionVisualizer::newOpenGLContextCreated()
	{
		OpenGLComponent::newOpenGLContextCreated();
		OpenGLLineRenderer::newOpenGLContextCreated();



		fillVertexShader = function_vis_vertex_shader;
		fillGeometryShader = funciton_vis_geometry_shader;
		fillFragmentShader = funciton_vis_fragment_shader;




		fillShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		if (fillShaderProgram->addVertexShader(fillVertexShader)
			&& fillShaderProgram->addShader(fillGeometryShader, juce::gl::GL_GEOMETRY_SHADER)
			&& fillShaderProgram->addFragmentShader(fillFragmentShader)
			&& fillShaderProgram->link())
		{
			fillShaderProgram->use();
			zeroLineY = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_zeroY");
			zeroLineY->set(zeroY);

			topColor = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_topColor");
			botColor = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_botColor");
			posColor = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_posColor");

			pos = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_pos");
			pos->set(fillPos);

			width = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_width");
			width->set(fillWidth);

			endpoint = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*fillShaderProgram.get(), "u_endpoint");
			setEndpoint(endPointVal);
		}
		else
		{
			jassertfalse;
		}

	}

	void OpenGLFunctionVisualizer::openGLContextClosing()
	{
		OpenGLComponent::openGLContextClosing();
		OpenGLLineRenderer::openGLContextClosing();
		fillShaderProgram.reset();
	}

	void OpenGLFunctionVisualizer::initArrays(int numOfSamplePoints)
	{
		lineVertexBuffer.getFirst()->init(numOfSamplePoints);
		auto& ib = *lineIndexBuffer.getFirst();
		ib.clear();

		for (int i = 0; i < numOfSamplePoints; ++i)
		{
			ib.add(i);
		}
	}

	void OpenGLFunctionVisualizer::setFillPos(float newFillPos)
	{
		newFillPos = (scalingX * (2 * newFillPos - 1) + 1) / 2;
		if (newFillPos != fillPos)
		{
			fillPos = newFillPos;
			pos->set(fillPos);
		}
	}

	void OpenGLFunctionVisualizer::setEndpoint(float newEndpoint)
	{
		newEndpoint = (scalingX * (2 * newEndpoint - 1) + 1) / 2;
		if (newEndpoint != endPointVal)
		{
			endPointVal = newEndpoint;
			endpoint->set(endPointVal);
			repaintThreadChecked();
		}
	}

	void OpenGLFunctionVisualizer::paintOverChildren(juce::Graphics& g)
	{
		auto rect = getLocalBounds().toFloat().getProportion(juce::Rectangle<float>(endPointVal, 0, 1 - endPointVal, 1));

		auto c = getBackgroundColor().getColor(isEnabled()).withMultipliedAlpha(1 - universals->disabledAlpha);


		g.setColour(c);
		g.fillRect(rect);
	}

	void OpenGLFunctionVisualizer::visibilityChanged()
	{
	}



	void OpenGLFunctionVisualizer::generateVertexBuffer()
	{
		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		fillShaderProgram->use();
		newFrameInit();
		generateZeroLine();

		auto& p = *lineVertexBuffer[0];
		for (int i = 0; i < p.getNumVerticies(); ++i)
		{
			float normX = i / (samplePoints - 1.0f);
			float x = scalingX * (2.0f * normX - 1.0f);
			float y = scalingY * calculateFunctionSample(i, x, normX);
			p.set(i,
				{
					x, y,
					r,g,b, a
				}
			);

		}

	}


	inline float OpenGLFunctionVisualizer::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		return 0.0f;
	}

	inline void OpenGLFunctionVisualizer::newFrameInit()
	{
	}



	void OpenGLFunctionVisualizer::calculateZeroLine()
	{
		zeroY = isBipolar ? 0.0f : -scalingY;
		if (zeroLineY.operator bool())
		{
			zeroLineY->set(zeroY);
		}

		generateZeroLine();
	}


} //namespace bdsp