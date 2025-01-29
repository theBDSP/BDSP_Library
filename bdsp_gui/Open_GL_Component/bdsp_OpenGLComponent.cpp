#include "bdsp_OpenGLComponent.h"

namespace bdsp
{

	OpenGLComponent::OpenGLComponent(GUI_Universals* universalsToUse, int numAttr)
		:Component(universalsToUse),
        vertexBuffer(numAttr),
        background(universalsToUse, this),
        numAttributes(numAttr)
	{
		setBackgroundColor(BDSP_COLOR_PURE_BLACK, BDSP_COLOR_DARK);
		setOpaque(true);

		universals->contextHolder->registerOpenGlRenderer(this);


	}




	OpenGLComponent::~OpenGLComponent()
	{
		//if (shaderProgram != nullptr)
		//{
		//	shaderProgram.release();
		//}

		openGLContextClosing();
		universals->contextHolder->unregisterOpenGlRenderer(this);

	}




	void OpenGLComponent::setBackgroundColor(const NamedColorsIdentifier& bkgd, const NamedColorsIdentifier& bkgdBehind)
	{
		background.setColors(bkgd, bkgdBehind.mixedWith(&bkgd, universals->disabledAlpha));
	}

	OpenGLColor OpenGLComponent::getBackgroundColor()
	{
		return background;
	}







	void OpenGLComponent::resized()
	{

	}







	void OpenGLComponent::newOpenGLContextCreated()
	{
		createShaders();

		// Create an instance of OpenGLShaderProgram
		shaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		// Each of these methods will return false if something goes wrong so we'll
		// wrap them in an if statement
		if (shaderProgram->addVertexShader(vertexShader)
			&& shaderProgram->addFragmentShader(fragmentShader)
			&& shaderProgram->link())
		{
			// No compilation errors - set the shader program to be active
			shaderProgram->use();
			createUniforms();
		}
		else
		{
			// Oops - something went wrong with our shaders!
			// Check the output window of your IDE to see what the error might be.
			jassertfalse;
		}
		// Generate 1 buffer, using our vbo variable to store its ID.
		juce::gl::glGenBuffers(1, &vbo);

		// Generate 1 more buffer, this time using our IBO variable.
		juce::gl::glGenBuffers(1, &ibo);

		// Bind the VBO.
		juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);

		// Bind the IBO.
		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);

		buffersCreated = true;
	}



	void OpenGLComponent::renderOpenGL()
	{

		generateVertexBuffer();


		renderWithoutGenerating();

	}



	void OpenGLComponent::openGLContextClosing()
	{
		//vertexBuffer.clear();
		//indexBuffer.clear();


		shaderProgram.reset();
		if (buffersCreated)
		{
			juce::gl::glDeleteBuffers(1, &vbo);
			juce::gl::glDeleteBuffers(1, &ibo);
		}

	}


	void OpenGLComponent::createShaders()
	{
		vertexShader =
			R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec4 position;
                       layout(location = 1) in vec4 sourceColour;
            
            // Output to fragment shader.
            out vec4 fragColour;
            
            void main()
            {
                // Set the position to the attribute we passed in.
                gl_Position = position;
                // Set the frag colour to the attribute we passed in.
                // This value will be interpolated for us before the fragment
                // shader so the colours we defined ateach vertex will create a
                // gradient across the shape.
                fragColour = sourceColour;
            }
        )";

		fragmentShader =
			R"(
            #version 410 core
            
            // The value that was output by the vertex shader.
            // This MUST have the exact same name that we used in the vertex shader.
            in vec4 fragColour;
            layout(location = 0) out vec4 frag_color;
            void main()
            {
                // Set the fragment's colour to the attribute passed in by the
                // vertex shader.
                frag_color = fragColour;
            }
        )";

	}

	void OpenGLComponent::renderWithoutGenerating()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		if (shouldDrawBackground)
		{
			juce::OpenGLHelpers::clear(background);
		}


		// Tell the renderer to use this shader program
		shaderProgram->use();

		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, vbo);
		//juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);

		// Send the vertices data.
		juce::gl::glBufferData(
			juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
			sizeof(float) * vertexBuffer.size(),   // The size (in bytes) of the data.
			vertexBuffer.data(),                    // A pointer to the actual data.
			juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
		);
		//juce::gl::glBufferData(
		//	juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
		//	sizeof(float) * vertexBuffer.size(),   // The size (in bytes) of the data.
		//	vertexBuffer.data(),                    // A pointer to the actual data.
		//	juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
		//);





		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
		//juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
		// Send the indices data.

		juce::gl::glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indexBuffer.size(),
			indexBuffer.data(),
			juce::gl::GL_STREAM_DRAW);

		//juce::gl::glBufferData(
		//	juce::gl::GL_ELEMENT_ARRAY_BUFFER,
		//	sizeof(unsigned int) * indexBuffer.size(),
		//	indexBuffer.data(),
		//	juce::gl::GL_STREAM_DRAW
		//);

		//================================================================================================================================================================================================

		createVertexAttributes();

		//================================================================================================================================================================================================


		juce::gl::glDrawElements(
			juce::gl::GL_TRIANGLES,       // Tell OpenGL to render triangles.
			indexBuffer.size(), // How many indices we have.
			juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
			nullptr             // We already gave OpenGL our indices so we don't
								// need to pass that again here, so pass nullptr.
		);


		removeVertexAttributes();

	}



	void OpenGLComponent::createUniforms()
	{
	}



	void OpenGLComponent::createVertexAttributes()
	{
		juce::gl::glEnableVertexAttribArray(0);
		// Enable the position attribute.
		juce::gl::glVertexAttribPointer(
			0,              // The attribute's index (AKA location).
			2,              // How many values this attribute contains.
			juce::gl::GL_FLOAT,       // The attribute's type (float).
			juce::gl::GL_FALSE,       // Tells OpenGL NOT to normalise the values.
			numAttributes * sizeof(float), // How many bytes to move to find the attribute with
			// the same index in the next vertex.
			nullptr         // How many bytes to move from the start of this vertex
							// to find this attribute (the default is 0 so we just
							// pass nullptr here).
		);


		juce::gl::glEnableVertexAttribArray(1);
		// Enable to colour attribute.
		juce::gl::glVertexAttribPointer(
			1,                              // This attribute has an index of 1
			4,                              // This time we have four values for the
			// attribute (r, g, b, a)
			juce::gl::GL_FLOAT,
			juce::gl::GL_FALSE,
			numAttributes * sizeof(float),
			(GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
			// position attribute in the Vertex
			// struct, so we need to skip over the
			// size of the position array to find
			// the start of this attribute.
		);


	}



	void OpenGLComponent::removeVertexAttributes()
	{
		juce::gl::glDisableVertexAttribArray(0);
		juce::gl::glDisableVertexAttribArray(1);
	}



	void OpenGLComponent::generateVertexBuffer()
	{
	}


	juce::Array<juce::Point<float>> OpenGLComponent::generateRoundedCornerPoints(float centerX, float centerY, float angleStart, float angleEnd, float radiusX, float radiusY, int numTriangles, float feather)
	{
		juce::Array<juce::Point<float>>out;


		out.resize(feather == 0 ? numTriangles : numTriangles * 2);



		for (int i = 0; i < numTriangles; ++i)
		{
			float angle = juce::jmap(float(i) / (numTriangles - 1), angleStart, angleEnd);
			float dx = radiusX * cos(angle);
			float dy = radiusY * sin(angle);

			out.set(i, { centerX + dx, centerY + dy });
			if (feather != 0)
			{
				out.set(i + numTriangles, { centerX + dx * feather, centerY + dy * feather });
			}
		}

		return out;
	}



	//================================================================================================================================================================================================
	OpenGLLineRenderer::OpenGLLineRenderer(GUI_Universals* universalsToUse, juce::Array<int> lineMaxPoints)
		:OpenGLComponent(universalsToUse)
	{
		numLines = lineMaxPoints.size();

		for (int i = 0; i < numLines; ++i)
		{
			lineVertexBuffer.add(new VertexArray(6));
			linePoints.add(new VertexArray(6));
			lineIndexBuffer.add(new IndexArray());

			for (int j = 0; j < 8 * (lineMaxPoints[i] - 1) - 5; ++j)
			{
				lineIndexBuffer[i]->addArray(
					{
					j, j + 1,  j + 4,
					j + 1, j + 4, j + 5

					});
			}

			linePoints[i]->init(lineMaxPoints[i]);

		}


		lineVbo.resize(numLines);
		lineIbo.resize(numLines);
	}



	OpenGLLineRenderer::~OpenGLLineRenderer()
	{
		//if (lineShaderProgram != nullptr)
		//{
		//	lineShaderProgram.release();
		//}
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLLineRenderer::newOpenGLContextCreated()
	{
		OpenGLComponent::newOpenGLContextCreated();


		//================================================================================================================================================================================================

		lineVertexShader = vertexShader;

		lineFragmentShader = fragmentShader;

		// Create an instance of OpenGLShaderProgram
		lineShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(*universals->openGLContext);

		// Compile and link the shader.
		// Each of these methods will return false if something goes wrong so we'll
		// wrap them in an if statement
		if (lineShaderProgram->addVertexShader(lineVertexShader)
			&& lineShaderProgram->addFragmentShader(lineFragmentShader)
			&& lineShaderProgram->link())
		{
			// No compilation errors - set the shader program to be active
			lineShaderProgram->use();

		}
		else
		{
			// Oops - something went wrong with our shaders!
			// Check the output window of your IDE to see what the error might be.
			jassertfalse;
		}

		//================================================================================================================================================================================================

		// Generate 1 buffer, using our vbo variable to store its ID.
		juce::gl::glGenBuffers(numLines, lineVbo.begin());

		// Generate 1 more buffer, this time using our IBO variable.
		juce::gl::glGenBuffers(numLines, lineIbo.begin());

		for (int i = 0; i < numLines; ++i)
		{

			// Bind the VBO.
			juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, lineVbo[i]);

			// Bind the IBO.
			juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, lineIbo[i]);
		}

	}

	void OpenGLLineRenderer::renderWithoutGenerating()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		if (shouldDrawBackground)
		{
			juce::OpenGLHelpers::clear(background);
		}





		//================================================================================================================================================================================================



		// Tell the renderer to use this shader program
		shaderProgram->use();

		juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);

		// Send the vertices data.
		juce::gl::glBufferData(
			juce::gl::GL_ARRAY_BUFFER,                        // The type of data we're sending.           
			sizeof(float) * vertexBuffer.size(),   // The size (in bytes) of the data.
			vertexBuffer.data(),                    // A pointer to the actual data.
			juce::gl::GL_STREAM_DRAW                        // How we want the buffer to be drawn.
		);






		juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
		// Send the indices data.
		juce::gl::glBufferData(
			juce::gl::GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int) * indexBuffer.size(),
			indexBuffer.data(),
			juce::gl::GL_STREAM_DRAW
		);

		//================================================================================================================================================================================================

		createVertexAttributes();

		//================================================================================================================================================================================================


		juce::gl::glDrawElements(
			juce::gl::GL_TRIANGLES,       // Tell OpenGL to render triangles.
			indexBuffer.size(), // How many indices we have.
			juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
			nullptr             // We already gave OpenGL our indices so we don't
								// need to pass that again here, so pass nullptr.
		);


		//================================================================================================================================================================================================

		lineShaderProgram->use();
		for (int i = 0; i < numLines; ++i)
		{
			auto& vb = *lineVertexBuffer[i];
			auto& ib = *lineIndexBuffer[i];

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
				juce::gl::GL_TRIANGLES,       // Tell OpenGL to render triangles.
				ib.size(), // How many indices we have.
				juce::gl::GL_UNSIGNED_INT,    // What type our indices are.
				nullptr             // We already gave OpenGL our indices so we don't
									// need to pass that again here, so pass nullptr.
			);

		}
		//================================================================================================================================================================================================



		removeVertexAttributes();

	}

	void OpenGLLineRenderer::setHasAlpha(bool canHaveAlpha)
	{
		hasAlpha = canHaveAlpha;
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
	}

	void OpenGLLineRenderer::generateLineTriangles()
	{
		for (int k = 0; k < linePoints.size(); k++)
		{
			auto& p = *linePoints[k];

			juce::Point<float>A;
			juce::Point<float>B = juce::Point<float>(p.getAttribute(0, 0), p.getAttribute(0, 1));


			auto& vb = *lineVertexBuffer[k];
			auto& ib = *lineIndexBuffer[k];


			ib.resize((8 * (p.getNumVerticies() - 1) - 5) * 6);

			for (int i = 0; i < p.getNumVerticies() - 1; ++i)
			{


				//================================================================================================================================================================================================



				A = B;
				B = juce::Point<float>(p.getAttribute(i + 1, 0), p.getAttribute(i + 1, 1));

				juce::Line<float>AB(A, B);

				//float angle = AB.getAngle();
				float angle = std::atan2f(B.x - A.x, A.y - B.y);

				//angle1 = angle+PI/2, use trig identity to swap and save an addition
				float sin1 = cosf(angle);
				float cos1 = -sinf(angle);

				// angle2 =angle1-PI, use trig identities to save  on computations
				float sin2 = -sin1;
				float cos2 = -cos1;


				juce::Point<float> EdgeOffset1 = juce::Point<float>(lineThicknessX[k] * sin1, -lineThicknessY[k] * cos1);
				juce::Point<float> EdgeOffset2 = juce::Point<float>(lineThicknessX[k] * sin2, -lineThicknessY[k] * cos2);
				juce::Point<float> FeatherOffset1 = juce::Point<float>(lineFeatherX[k] * sin1, -lineFeatherY[k] * cos1);
				juce::Point<float> FeatherOffset2 = juce::Point<float>(lineFeatherX[k] * sin2, -lineFeatherY[k] * cos2);





				float red = p.getAttribute(i + 1, 2);
				float green = p.getAttribute(i + 1, 3);
				float blue = p.getAttribute(i + 1, 4);
				float alpha = hasAlpha ? p.getAttribute(i + 1, 5) : 1.0f;

				int n = 8 * i;
				vb.set(n,
					{
						A.x + FeatherOffset1.x, A.y + FeatherOffset1.y,
						red, green, blue , 0.0f
					});
				vb.set(n + 1,
					{
						A.x + EdgeOffset1.x, A.y + EdgeOffset1.y,
						red, green, blue , alpha
					});
				vb.set(n + 2,
					{
						A.x + EdgeOffset2.x, A.y + EdgeOffset2.y,
						red, green, blue , alpha
					});
				vb.set(n + 3,
					{
						A.x + FeatherOffset2.x, A.y + FeatherOffset2.y,
						red, green, blue , 0.0f
					});

				vb.set(n + 4,
					{
						B.x + FeatherOffset1.x, B.y + FeatherOffset1.y,
						red, green, blue , 0.0f
					});
				vb.set(n + 5,
					{
						B.x + EdgeOffset1.x, B.y + EdgeOffset1.y,
						red, green, blue , alpha
					});
				vb.set(n + 6,
					{
						B.x + EdgeOffset2.x, B.y + EdgeOffset2.y,
						red, green, blue , alpha
					});
				vb.set(n + 7,
					{
						B.x + FeatherOffset2.x, B.y + FeatherOffset2.y,
						red, green, blue , 0.0f
					});
			}


		}
	}

	void OpenGLLineRenderer::resized()
	{
		for (int i = 0; i < numLines; ++i)
		{
			lineThicknessX.set(i, lineScreenThickness[i] / getWidth());
			lineThicknessY.set(i, lineScreenThickness[i] / getHeight());

			lineScreenFeather.set(i, featherRatio * lineScreenThickness[i]);

			if (lineScreenFeather[i] > BDSP_OPEN_GL_LINE_MINIMUM_FEATHER_SIZE)
			{
				lineFeatherX.set(i, featherRatio * lineThicknessX[i]);
				lineFeatherY.set(i, featherRatio * lineThicknessY[i]);
			}
			else // need to expand feather to minimum size
			{
				float ratio = BDSP_OPEN_GL_LINE_MINIMUM_FEATHER_SIZE / lineScreenFeather[i];


				lineFeatherX.set(i, featherRatio * lineThicknessX[i] * ratio);
				lineFeatherY.set(i, featherRatio * lineThicknessY[i] * ratio);
			}
		}

		OpenGLComponent::resized();
	}

	void OpenGLLineRenderer::setThickness(int idx, float thickness)
	{
		lineScreenThickness.set(idx, thickness);
		//resized();
	}

	void OpenGLLineRenderer::setFeatherRatio(float newRatio)
	{
		featherRatio = newRatio;
		resized();
	}


	//================================================================================================================================================================================================



	OpenGLFunctionVisualizer::OpenGLFunctionVisualizer(GUI_Universals* universalsToUse, bool shouldHaveZeroLine, bool shouldRenderAreaUnderCurve, int numOfSamplePoints)
		:OpenGLLineRenderer(universalsToUse, 1),
		lineColor(universalsToUse, this),
		zeroLine(universalsToUse, this)
	{
		hasZeroLine = shouldHaveZeroLine;
		renderAreaUnderCurve = shouldRenderAreaUnderCurve;

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

			scalingX = baseScalingX - (lineThicknessX.getFirst() + lineFeatherX.getFirst());
			scalingY = baseScalingY - (lineThicknessY.getFirst() + lineFeatherY.getFirst());
			calculateZeroLine();

		}
	}


	void OpenGLFunctionVisualizer::setColor(const NamedColorsIdentifier& c, const NamedColorsIdentifier& line, float topCurveOpacity, float botCurveOpacity)
	{
		lineColor.setColors(c, background.getColorID(false).mixedWith(&c, universals->disabledAlpha));

		zeroLine.setColors(line, background.getColorID(false).mixedWith(&line, universals->disabledAlpha));

		topCurveAlpha = topCurveOpacity;
		botCurveAlpha = botCurveOpacity;

		generateZeroLine();
		resized();
	}



	void OpenGLFunctionVisualizer::setScaling(float newXScalingFactor, float newYScalingFactor)
	{

		baseScalingX = newXScalingFactor;
		baseScalingY = newYScalingFactor == 0 ? newXScalingFactor : newYScalingFactor;

		resized();
		//universals->openGLContext->triggerRepaint();
	}

	void OpenGLFunctionVisualizer::generateZeroLine()
	{

		if (hasZeroLine)
		{

			float r, g, b, a;
			zeroLine.getComponents(r, g, b, a);

			vertexBuffer.set(zeroLineStart,
				{
					-1.0f, zeroY + BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
					r,g,b,a
				});

			vertexBuffer.set(zeroLineStart + 1,
				{
					1.0f, zeroY + BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
					r,g,b,a
				});
			vertexBuffer.set(zeroLineStart + 2,
				{
					-1.0f, zeroY - BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
					r,g,b,a
				});

			vertexBuffer.set(zeroLineStart + 3,
				{
					1.0f, zeroY - BDSP_OPEN_GL_LINE_FEATHER_RATIO * lineThicknessY.getFirst() / 2,
					r,g,b,a
				});
		}

	}

	void OpenGLFunctionVisualizer::initArrays(int numSamplePoints)
	{
		samplePoints = numSamplePoints;
		vertexBuffer.init((renderAreaUnderCurve ? 2 * samplePoints : 0) + (hasZeroLine ? 4 : 0));
		linePoints[0]->init(samplePoints);
		lineVertexBuffer[0]->init(8 * samplePoints);
		indexBuffer.clear();
		zeroLineStart = vertexBuffer.getNumVerticies() - 4;
		if (hasZeroLine) // put behind function
		{
			indexBuffer.addArray(
				{
					zeroLineStart,zeroLineStart + 1,zeroLineStart + 2,
					zeroLineStart + 2, zeroLineStart + 3,zeroLineStart + 1
				}
			);
		}

		if (renderAreaUnderCurve)
		{
			for (int i = 0; i < samplePoints - 1; ++i)
			{
				indexBuffer.addArray(
					{
						//================================================================================================================================================================================================
						//Area Under Curve					
						//samplePoints + i,i,samplePoints + i + 1, //bl, tl, br
						//samplePoints + i + 1, i,i + 1, //br, tl, tr

						i, i + 1, samplePoints + i + 1,
						i, samplePoints + i, samplePoints + i + 1

					}
				);
			}
		}

		lineIndexBuffer[0]->clear();
		for (int i = 0; i < 8 * (samplePoints - 1) - 5; ++i)
		{
			lineIndexBuffer[0]->addArray(
				{
				i, i + 1,  i + 4,
				i + 1, i + 4, i + 5

				});
		}
	}

	void OpenGLFunctionVisualizer::generateVertexBuffer()
	{
		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		newFrameInit();
		//generateZeroLine();

		auto& p = *linePoints[0];
		for (int i = 0; i < p.getNumVerticies(); ++i)
		{
			float normX = i / (samplePoints - 1.0f);
			float x = scalingX * (2.0f * normX - 1.0f);
			float y = scalingY * calculateFunctionSample(i, x, normX);
			if (renderAreaUnderCurve)
			{
				vertexBuffer.set(i,
					{
						x, y,
						r,g,b, calculateAlpha(x,y)
					} // AUC Top-i
				);
				vertexBuffer.set(samplePoints + i,
					{
						x, zeroY,
						r,g,b, calculateAlpha(x,zeroY)
					} // line AUC-i
				);
			}

			p.set(i,
				{
					x, y,
					r,g,b, 1.0f
				}
			);

		}
		generateLineTriangles();

	}



	inline float OpenGLFunctionVisualizer::calculateAlpha(float x, float y)
	{
		return isBipolar ? juce::jmap(abs(y), 0.0f, 1.0f, botCurveAlpha, topCurveAlpha) : juce::jmap(y, -1.0f, 1.0f, botCurveAlpha, topCurveAlpha);
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
		generateZeroLine();
	}





	//================================================================================================================================================================================================
	OpenGLSpectrumVisualizer::OpenGLSpectrumVisualizer(GUI_Universals* universalsToUse, int numSamples, bool isStereo, bool useLogFrequency)
		:OpenGLComponent(universalsToUse),
		color(universalsToUse, this)
	{
		samplePoints = numSamples;

		stereo = isStereo;
		logFrequency = useLogFrequency;
		vertexBuffer.init(numSamples * (isStereo ? 4 : 2));

		for (int i = 0; i < numSamples - 1; ++i)
		{
			if (isStereo)
			{

				indexBuffer.addArray({
					4 * i, 4 * i + 1,4 * i + 4,
					4 * i + 1,4 * i + 4, 4 * i + 5,

					4 * i + 2, 4 * i + 3,4 * i + 6,
					4 * i + 3,4 * i + 6,4 * i + 7
					});
			}
			else
			{
				indexBuffer.addArray({
					2 * i,2 * i + 1,2 * i + 2,
					2 * i + 1,2 * i + 2,2 * i + 3
					});
			}
		}


		for (int i = 0; i < samplePoints; ++i)
		{
			float prop = i / (samplePoints - 1.0f);
			float x = 2 * prop - 1;
			if (useLogFrequency)
			{
				freqVals.add({ x,universals->freqRange.convertFrom0to1(prop) });
			}
			else
			{
				freqVals.add({ x,juce::jmap(prop, universals->freqRange.start, universals->freqRange.end) });
			}
		}

	}

	OpenGLSpectrumVisualizer::~OpenGLSpectrumVisualizer()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}


	void OpenGLSpectrumVisualizer::setColor(const NamedColorsIdentifier& c)
	{
		color.setColors(c, background.getColorID(false).mixedWith(&c, universals->disabledAlpha));
	}

	void OpenGLSpectrumVisualizer::generateVertexBuffer()
	{
		float r, g, b, a;

		color.getComponents(r, g, b, a);

		newFrameInit();
		//generateZeroLine();

		for (int i = 0; i < samplePoints; ++i)
		{
			if (stereo)
			{
				a = calculateFrequencyResponse(0, freqVals[i].second);

				vertexBuffer.set(4 * i,
					{
						freqVals[i].first, 1,
						r,g,b,a
					});
				vertexBuffer.set(4 * i + 1,
					{
						freqVals[i].first, 0,
						r,g,b,a
					});

				a = calculateFrequencyResponse(1, freqVals[i].second);

				vertexBuffer.set(4 * i + 2,
					{
						freqVals[i].first, 0,
						r,g,b,a
					});
				vertexBuffer.set(4 * i + 3,
					{
						freqVals[i].first, -1,
						r,g,b,a
					});
			}
			else
			{
				a = calculateFrequencyResponse(0, freqVals[i].second);

				vertexBuffer.set(2 * i,
					{
						freqVals[i].first, 1,
						r,g,b,a
					});
				vertexBuffer.set(2 * i + 1,
					{
						freqVals[i].first, -1,
						r,g,b,a
					});

			}
		}
	}

	inline float OpenGLSpectrumVisualizer::calculateFrequencyResponse(int channel, float freq)
	{
		return 0.0f;
	}
	inline void OpenGLSpectrumVisualizer::newFrameInit()
	{
	}
	//================================================================================================================================================================================================



	SpectrogramController::SpectrogramController(dsp::SampleSource<float>* sourceToTrack, dsp::DSP_Universals<float>* lookupsToUse, int order)
		:window(quickPow2(order + 1), juce::dsp::WindowingFunction<float>::blackman)
	{
		source = sourceToTrack;
		source.get()->onBufferFilled = [=]() {newBufferCreated(); };
		fft = lookupsToUse->getFFT(order);

		input.resize(quickPow2(order + 1));
		response.resize(quickPow2(order + 1));
	}


	void SpectrogramController::prepare(const juce::dsp::ProcessSpec& spec)
	{

		source.get()->prepare(spec);

		numChannels = spec.numChannels;
		numSamples = spec.maximumBlockSize;
	}


	void SpectrogramController::newBufferCreated()
	{
		const auto& buffer = source.getBuffer(numChannels, numSamples);
		for (int i = 0; i < buffer.getNumSamples(); ++i)
		{
			input.set(inputPtr++, (buffer.getSample(0, i) + buffer.getSample(1, i)) / 2);
			if (inputPtr == input.size())
			{
				response.swapWith(input);
				window.multiplyWithWindowingTable(response.data(), response.size());
				fft->performFrequencyOnlyForwardTransform(response.data(), true);

				if (bufferToPushTo != nullptr)
				{
					bufferToPushTo->set(response);
				}

				if (freqValsToPushTo != nullptr)
				{
					freqValsToPushTo->swapWith(response);
				}

				inputPtr = 0;
			}
		}
	}


	int SpectrogramController::getSize()
	{
		return fft->getSize();
	}

	void SpectrogramController::setBufferToPushTo(CircularBuffer<juce::Array<float>>* newBuffer)
	{
		bufferToPushTo = newBuffer;
	}

	void SpectrogramController::setFreqValsToPushTo(juce::Array<float>* newBuffer)
	{
		freqValsToPushTo = newBuffer;
	}



	OpenGLSpectrogram::OpenGLSpectrogram(GUI_Universals* universalsToUse, SpectrogramController* specController, float sampleRate, float windowSizeInMilliseconds)
		:OpenGLComponent(universalsToUse),
		c(universalsToUse, this)
	{
		bucketsPerWindow = specController->getSize() / 2;
		bucketValues.resize(sampleRate * windowSizeInMilliseconds / (1000 * bucketsPerWindow));
		vertexBuffer.resize(4 * bucketValues.getSize() * bucketsPerWindow);


		for (int i = 0; i < vertexBuffer.size(); ++i)
		{
			indexBuffer.addArray({
				4 * i,4 * i + 1,4 * i + 2,
				4 * i + 1,4 * i + 2,4 * i + 3
				});
		}


		w = 2.0f / (bucketValues.getSize() - 1);
		h = 2.0f / (bucketsPerWindow - 1);

		specController->setBufferToPushTo(&bucketValues);
	}

	OpenGLSpectrogram::~OpenGLSpectrogram()
	{
	}

	void OpenGLSpectrogram::generateVertexBuffer()
	{
		float r, g, b, a;
		c.getComponents(r, g, b, a);

		float x = -1;
		float y = -1;

		int n = 0;
		for (int i = 0; i < bucketValues.getSize(); ++i)
		{
			y = -1;
			for (int j = 0; j < bucketsPerWindow; ++j)
			{
				float v = bucketValues.get(i)[j];

				vertexBuffer.set(++n,
					{
						x,y,
						r,g,b,v
					});

				vertexBuffer.set(++n,
					{
						x + w,y,
						r,g,b,v
					});
				vertexBuffer.set(++n,
					{
						x,y + h,
						r,g,b,v
					});

				vertexBuffer.set(++n,
					{
						x + w,y + h,
						r,g,b,v
					});
				y += h;
			}
			x += w;
		}

	}



	void OpenGLSpectrogram::pushBuckets(juce::Array<float> newBuckets)
	{
		bucketValues.set(newBuckets);
	}

	void OpenGLSpectrogram::setColor(const NamedColorsIdentifier& newColor)
	{
		c.setColors(newColor, background.getColorID(isEnabled()).mixedWith(newColor, universals->disabledAlpha));
	}



	//===============================================================================================================================================================================================
	OpenGLControlValuesListener::OpenGLControlValuesListener(float windowSizeInMilliseconds, LinkableControl* controlToFollow)
		:LinkableControl::SampleListener(controlToFollow)
	{
		functionSamples.resize(windowSizeInMilliseconds * BDSP_CONTROL_SAMPLE_RATE / 1000.0f);
	}

	void OpenGLControlValuesListener::pushNewSample(float smp)
	{

		functionSamples.set(smp);
	}



	OpenGLControlValuesOverTime::OpenGLControlValuesOverTime(LinkableControl* controlToFollow, GUI_Universals* universalsToUse, bool shouldHaveZeroLine, bool shouldRenderAreaUnderCurve)
		:OpenGLFunctionVisualizer(universalsToUse, shouldHaveZeroLine, shouldRenderAreaUnderCurve)
	{
		control = controlToFollow;

		for (auto l : control->getListeners())
		{
			auto cast = dynamic_cast<OpenGLControlValuesListener*>(l);
			if (l != nullptr)
			{
				listener = cast;
			}
		}

		jassert(listener != nullptr);

		initArrays(listener->functionSamples.getSize());

	}





	OpenGLControlValuesOverTime::~OpenGLControlValuesOverTime()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}




	void OpenGLControlValuesOverTime::generateVertexBuffer()
	{
		float r, g, b, a;

		lineColor.getComponents(r, g, b, a);

		newFrameInit();

		setBipolar(control->getPolarity());


		//generateZeroLine();

		auto p = linePoints.getFirst();

		for (int i = 0; i < listener->functionSamples.getSize(); ++i)
		{
			float prop = float(i) / (listener->functionSamples.getSize() - 1);
			float x = scalingX * (2 * prop - 1);
			float y;
			float smp = listener->functionSamples.get(i);


			if (forceFullRange)
			{
				y = isBipolar ? scalingY * smp : -zeroY * (2.0f * smp - 1.0f); // zero Y ensures line is visible on env follower with no input
			}
			else
			{
				y = scalingY * smp;
			}

			float alpha = alphaFalloff ? prop : 1.0f;

			if (renderAreaUnderCurve)
			{

				vertexBuffer.set(i,
					{
						x, y,
						r,g,b, alpha * calculateAlpha(x, y)
					} // AUC Top-i
				);
				vertexBuffer.set(samplePoints + i,
					{
						x, zeroY,
						r,g,b, alpha * calculateAlpha(x, zeroY)
					} // line AUC-i
				);
			}

			p->set(i,
				{
					x, y,
					//{r, g, b, 1.0f}
					alpha * r, alpha * g, alpha * b, 1.0f
				}
			);

		}

		generateLineTriangles();
	}



	//================================================================================================================================================================================================


	OpenGLCompositeComponent::OpenGLCompositeComponent(GUI_Universals* universalsToUse)
		:OpenGLComponent(universalsToUse)
	{
		setBackgroundColor(BDSP_COLOR_PURE_BLACK, BDSP_COLOR_DARK);
		setOpaque(true);
		universals->contextHolder->registerOpenGlRenderer(this);
	}




	OpenGLCompositeComponent::~OpenGLCompositeComponent()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}








	void OpenGLCompositeComponent::resized()
	{
		for (int i = 0; i < subClasses.size(); ++i)
		{
			subClasses[i]->setBoundsRelative(0, 0, 1, 1);
		}
	}



	void OpenGLCompositeComponent::newOpenGLContextCreated()
	{
		OpenGLComponent::newOpenGLContextCreated();
		for (int i = 0; i < subClasses.size(); ++i)
		{
			subClasses[i]->newOpenGLContextCreated();
		}
		onShaderCreation();
	}


	void OpenGLCompositeComponent::renderOpenGL()
	{
		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		juce::OpenGLHelpers::clear(background);

		generateVertexBuffer();
		for (int i = 0; i < subClasses.size(); ++i)
		{
			subClasses[i]->renderWithoutGenerating();
		}
	}


	void OpenGLCompositeComponent::openGLContextClosing()
	{
		OpenGLComponent::openGLContextClosing();
		for (int i = 0; i < subClasses.size(); ++i)
		{
			subClasses[i]->openGLContextClosing();
		}
	}


	void OpenGLCompositeComponent::generateVertexBuffer()
	{

	}


	void OpenGLCompositeComponent::initSubClasses()
	{

		for (auto s : subClasses)
		{
			s->shouldDrawBackground = false;
			addChildComponent(s);
		}
	}



	void OpenGLCompositeComponent::onShaderCreation()
	{

	}








	//================================================================================================================================================================================================



	OpenGLGrid::OpenGLGrid(GUI_Universals* universalsToUse, int maxRows, int maxCols)
		:OpenGLComponent(universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		thickness = universals->visualizerLineThickness;

		int maxNum = maxRows * maxCols;
		vertexBuffer.init(maxNum * 4);

		for (int i = 0; i < maxNum; ++i)
		{
			indexBuffer.addArray({
				4 * i,4 * i + 1, 4 * i + 2,
				4 * i + 1,4 * i + 2,4 * i + 3
				});
		}

	}

	OpenGLGrid::~OpenGLGrid()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLGrid::resized()
	{
		xThickness = thickness / getWidth();
		yThickness = thickness / getHeight();
	}





	void OpenGLGrid::setXSpacing(float newSpacing)
	{
		xSpacing = newSpacing;
	}

	void OpenGLGrid::setYSpacing(float newSpacing)
	{
		ySpacing = newSpacing;
	}

	void OpenGLGrid::setThickness(float newThickness)
	{
		thickness = newThickness;
		resized();
	}

	void OpenGLGrid::generateVertexBuffer()
	{
		int n = 2;
		float x = 0;

		vertexBuffer.set(0, {
			-xThickness,-1,
			r,g,b,1
			});
		vertexBuffer.set(1, {
			xThickness,-1,
			r,g,b,1
			});

		vertexBuffer.set(2, {
			-xThickness,1,
			r,g,b,1
			});
		vertexBuffer.set(3, {
			xThickness,1,
			r,g,b,1
			});


		//================================================================================================================================================================================================

		vertexBuffer.set(4, {
			-1,-yThickness,
			r,g,b,1
			});
		vertexBuffer.set(5, {
			1,-yThickness,
			r,g,b,1
			});

		vertexBuffer.set(6, {
			-1,yThickness,
			r,g,b,1
			});
		vertexBuffer.set(7, {
			1,yThickness,
			r,g,b,1
			});

		while (x <= 1.0f)
		{
			vertexBuffer.set(4 * n, {
				x - xThickness,-1,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 1, {
				x + xThickness,-1,
				r,g,b,1
				});

			vertexBuffer.set(4 * n + 2, {
				x - xThickness,1,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 3, {
				x + xThickness,1,
				r,g,b,1
				});


			//================================================================================================================================================================================================

			vertexBuffer.set(4 * n + 4, {
				-x - xThickness,-1,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 5, {
				-x + xThickness,-1,
				r,g,b,1
				});

			vertexBuffer.set(4 * n + 6, {
				-x - xThickness,1,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 7, {
				-x + xThickness,1,
				r,g,b,1
				});


			x += xSpacing;
			n += 2;
		}

		float y = 0;


		while (y <= 1.0f)
		{
			vertexBuffer.set(4 * n, {
				-1,y - yThickness,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 1, {
				1, y - yThickness,
				r,g,b,1
				});

			vertexBuffer.set(4 * n + 2, {
				-1, y + yThickness,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 3, {
				1, y + yThickness,
				r,g,b,1
				});


			//================================================================================================================================================================================================

			vertexBuffer.set(4 * n + 4, {
					-1,-y - yThickness,
					r,g,b,1
				});
			vertexBuffer.set(4 * n + 5, {
				1, -y - yThickness,
				r,g,b,1
				});

			vertexBuffer.set(4 * n + 6, {
				-1, -y + yThickness,
				r,g,b,1
				});
			vertexBuffer.set(4 * n + 7, {
				1, -y + yThickness,
				r,g,b,1
				});



			y += ySpacing;
			n += 2;
		}

		vertexBuffer.resize(4 * n);
		indexBuffer.resize(6 * n);
	}

	void OpenGLGrid::setColor(const NamedColorsIdentifier& newColor)
	{
		color = newColor;
		GUI_UniversalsChanged();
	}

	void OpenGLGrid::GUI_UniversalsChanged()
	{
		auto c = getColor(color);
		r = c.getFloatRed();
		g = c.getFloatGreen();
		b = c.getFloatBlue();

	}


	//================================================================================================================================================================================================
	OpenGLCirclePlotter::OpenGLCirclePlotter(GUI_Universals* universalsToUse, int maxCircles)
		:OpenGLComponent(universalsToUse, 10),
		circleData(6)
	{

		for (int i = 0; i < maxCircles; ++i)
		{
			indexBuffer.addArray({
				4 * i, 4 * i + 1, 4 * i + 2,
				4 * i + 1, 4 * i + 2, 4 * i + 3
				});
		}
	}

	OpenGLCirclePlotter::~OpenGLCirclePlotter()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLCirclePlotter::resized()
	{
		OpenGLComponent::resized();
		if (!getBounds().isEmpty())
		{
			pointW = screenRadius / getWidth();
			pointH = screenRadius / getHeight();

			featherW = pointW * featherRatio;
			featherH = pointH * featherRatio;
		}
	}

	void OpenGLCirclePlotter::setRadius(float newRadius)
	{
		screenRadius = newRadius;
		resized();
	}

	void OpenGLCirclePlotter::setFeatherRatio(float newRatio)
	{
		featherRatio = newRatio;
		featherRatioUniform->set(featherRatio);
		resized();
	}

	float OpenGLCirclePlotter::getScreenRadius()
	{
		return screenRadius;
	}


	void OpenGLCirclePlotter::generateCircleVerticies()
	{
		int n = 0;
		indexBuffer.resize(circleData.size());
		for (int i = 0; i < circleData.size(); ++i)
		{
			auto x = circleData.getAttribute(i, 0);
			auto y = circleData.getAttribute(i, 1);
			auto r = circleData.getAttribute(i, 2);
			auto g = circleData.getAttribute(i, 3);
			auto b = circleData.getAttribute(i, 4);
			auto a = circleData.getAttribute(i, 5);
			vertexBuffer.set(n, {
				x - featherW , y - featherH ,
				x, y,
				pointW, pointH,
				r, g, b, a
				});
			vertexBuffer.set(n + 1, {
				x + featherW , y - featherH ,
				x, y,
				pointW, pointH,
				r, g, b, a
				});
			vertexBuffer.set(n + 2, {
				x - featherW , y + featherH ,
				x, y,
				pointW, pointH,
				r, g, b, a
				});
			vertexBuffer.set(n + 3, {
				x + featherW , y + featherH ,
				x, y,
				pointW, pointH,
				r, g, b, a
				});

			n += 4;
		}
	}

	void OpenGLCirclePlotter::createShaders()
	{
		vertexShader =
			R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec4 position;
                       layout(location = 1) in vec2 center;
                       layout(location = 2) in vec2 radius;
                       layout(location = 3) in vec4 color;
            
            // Output to fragment shader.
					layout(location = 0) out vec4 frag_position;
                    layout(location = 1) out vec2 frag_center;
                    layout(location = 2) out vec2 frag_radius;            
                    layout(location = 3) out vec4 frag_color;            
            void main()
            {
                gl_Position = position;

                frag_position = position;
				frag_center = center;
				frag_radius = radius;
				frag_color = color;
            }
        )";

		fragmentShader =
			R"(
            #version 410 core
            
          
					layout(location = 0) in vec4 frag_position;
                    layout(location = 1) in vec2 frag_center;
                    layout(location = 2) in vec2 frag_radius;   
                    layout(location = 3) in vec4 frag_color;   
				
					uniform float u_feather;


           layout(location = 0) out vec4 out_color;
            void main()
            {

				float x = frag_position.x - frag_center.x;
				float y = frag_position.y - frag_center.y;
            
				float r = sqrt(x*x/(frag_radius.x*frag_radius.x) + y*y/(frag_radius.y*frag_radius.y));

				float a = 1.0;
				if(r>1)
				{
					a = -(r-u_feather)/(u_feather-1.0);
				}

		
                out_color = vec4(frag_color.rgb,frag_color.a * a);
            }
        )";
	}

	void OpenGLCirclePlotter::createUniforms()
	{
		featherRatioUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_feather");
		setFeatherRatio(featherRatio);
	}


	void OpenGLCirclePlotter::createVertexAttributes()
	{
		juce::gl::glEnableVertexAttribArray(0);
		// Enable the position attribute.
		juce::gl::glVertexAttribPointer(
			0,              // The attribute's index (AKA location).
			2,              // How many values this attribute contains.
			juce::gl::GL_FLOAT,       // The attribute's type (float).
			juce::gl::GL_FALSE,       // Tells OpenGL NOT to normalise the values.
			sizeof(float) * numAttributes, // How many bytes to move to find the attribute with
			// the same index in the next vertex.
			nullptr         // How many bytes to move from the start of this vertex
							// to find this attribute (the default is 0 so we just
							// pass nullptr here).
		);

		juce::gl::glEnableVertexAttribArray(1);
		// Enable to center attribute.
		juce::gl::glVertexAttribPointer(
			1,                              // This attribute has an index of 1
			2,                              // This time we have four values for the
			juce::gl::GL_FLOAT,
			juce::gl::GL_FALSE,
			sizeof(float) * numAttributes,
			(GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
			// position attribute in the Vertex
			// struct, so we need to skip over the
			// size of the position array to find
			// the start of this attribute.
		);

		juce::gl::glEnableVertexAttribArray(2);
		// Enable to radius attribute.
		juce::gl::glVertexAttribPointer(
			2,                              // This attribute has an index of 1
			2,                              // This time we have four values for the
			juce::gl::GL_FLOAT,
			juce::gl::GL_FALSE,
			sizeof(float) * numAttributes,
			(GLvoid*)(sizeof(float) * 4)    // This attribute comes after the
			// position attribute in the Vertex
			// struct, so we need to skip over the
			// size of the position array to find
			// the start of this attribute.
		);

		juce::gl::glEnableVertexAttribArray(3);
		// Enable to radius attribute.
		juce::gl::glVertexAttribPointer(
			3,                              // This attribute has an index of 1
			4,                              // This time we have four values for the
			juce::gl::GL_FLOAT,
			juce::gl::GL_FALSE,
			sizeof(float) * numAttributes,
			(GLvoid*)(sizeof(float) * 6)    // This attribute comes after the
			// position attribute in the Vertex
			// struct, so we need to skip over the
			// size of the position array to find
			// the start of this attribute.
		);
	}

	void OpenGLCirclePlotter::removeVertexAttributes()
	{
		juce::gl::glDisableVertexAttribArray(0);
		juce::gl::glDisableVertexAttribArray(1);
		juce::gl::glDisableVertexAttribArray(2);
		juce::gl::glDisableVertexAttribArray(3);
	}



	//================================================================================================================================================================================================






	OpenGLRing::OpenGLRing(GUI_Universals* universalsToUse, int numSamples)
		:OpenGLComponent(universalsToUse),
		GUI_Universals::Listener(universalsToUse)
	{
		samplePoints = numSamples;

		for (int i = 0; i < samplePoints - 1; ++i)
		{
			int n = 4 * i;
			indexBuffer.addArray({
				n, n + 1, n + 4,
				n + 1,n + 4,n + 5,

				n + 1,n + 2,n + 5,
				n + 2,n + 5,n + 6,


				n + 2,n + 3,n + 6,
				n + 3,n + 6,n + 7
				});

			float angleX = cos(2 * PI * i / (float)samplePoints);
			float angleY = sin(2 * PI * i / (float)samplePoints);


			angles.set(i, { angleX,angleY });

		}



		float angleX = cos(2 * PI * (samplePoints - 1) / (float)samplePoints);
		float angleY = sin(2 * PI * (samplePoints - 1) / (float)samplePoints);


		angles.add({ angleX,angleY });

		int n = 4 * (samplePoints - 1);
		indexBuffer.addArray({
			n, n + 1, 0,
			n + 1,0,1,

			n + 1,n + 2,1,
			n + 2,1,2,


			n + 2,n + 3,2,
			n + 3,2,3
			});


		vertexBuffer.init(4 * numSamples);
	}

	OpenGLRing::~OpenGLRing()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}

	void OpenGLRing::generateVertexBuffer()
	{



		for (int i = 0; i < samplePoints; ++i)
		{
			vertexBuffer.set(4 * i,
				{
					center.x + featherOutX * angles[i].x, center.y + featherOutY * angles[i].y,
					r,g,b,0
				});
			vertexBuffer.set(4 * i + 1,
				{
					center.x + radOutX * angles[i].x, center.y + radOutY * angles[i].y,
					r,g,b,a
				});
			vertexBuffer.set(4 * i + 2,
				{
					center.x + radInX * angles[i].x, center.y + radInY * angles[i].y,
					r,g,b,a
				});
			vertexBuffer.set(4 * i + 3,
				{
					center.x + featherInX * angles[i].x, center.y + featherInY * angles[i].y,
					r,g,b,0
				});

		}
	}

	void OpenGLRing::resized()
	{
		OpenGLComponent::resized();
		if (!getBounds().isEmpty())
		{
			radX = screenRadius / getWidth();
			radY = screenRadius / getHeight();

			thicknessX = screenThickness / getWidth();
			thicknessY = screenThickness / getHeight();


			featherOutX = radX + featherRatio * thicknessX;
			radOutX = radX + thicknessX;
			radInX = radX - thicknessX;
			featherInX = radX - featherRatio * thicknessX;



			featherOutY = radY + featherRatio * thicknessY;
			radOutY = radY + thicknessY;
			radInY = radY - thicknessY;
			featherInY = radY - featherRatio * thicknessY;



		}
	}

	void OpenGLRing::setRadius(float newRadius)
	{
		screenRadius = newRadius;
		resized();
	}

	void OpenGLRing::setThickness(float newThickness)
	{
		screenThickness = newThickness;
		resized();
	}

	void OpenGLRing::setFeatherRatio(float newFeatherRatio)
	{
		featherRatio = newFeatherRatio;
		resized();
	}

	void OpenGLRing::setCenter(juce::Point<float> newCenter, bool isGLCoord)
	{
		if (isGLCoord)
		{
			center = newCenter;
		}
		else
		{
			float x = 2 * (newCenter.x / getWidth()) - 1;
			float y = -(2 * (newCenter.y / getHeight()) - 1);
            
            center = {x,y};
		}

	}

	void OpenGLRing::setColor(const NamedColorsIdentifier& newColor)
	{
		color = newColor;
		GUI_UniversalsChanged();
	}

	void OpenGLRing::GUI_UniversalsChanged()
	{
		auto c = getColor(color);
		r = c.getFloatRed();
		g = c.getFloatGreen();
		b = c.getFloatBlue();
		a = c.getFloatAlpha();

	}







	OpenGLFFTVisualizer::OpenGLFFTVisualizer(GUI_Universals* universalsToUse, SpectrogramController* controller, bool isStereo, bool useLogFrequency, int numFramesToAvg)
		:OpenGLSpectrumVisualizer(universalsToUse, controller->getSize() / 2, isStereo, useLogFrequency)
	{
		vals.resize(numFramesToAvg);
		controller->setBufferToPushTo(&vals);
	}

	OpenGLFFTVisualizer::~OpenGLFFTVisualizer()
	{
	}

	inline float OpenGLFFTVisualizer::calculateFrequencyResponse(int channel, float freq)
	{
		auto arr = vals.getInternalArray();
		float tot = 0;

		auto fftDataIndex = juce::jmap(freq, universals->freqRange.start, universals->freqRange.end, 0.0f, (float)samplePoints - 1);

		for (int i = 0; i < vals.getSize(); ++i)
		{
			tot += juce::jmap(fftDataIndex - (int)fftDataIndex, arr[i][floor(fftDataIndex)], arr[i][ceil(fftDataIndex)]);
		}

		auto level = juce::jmap(juce::Decibels::gainToDecibels(tot / vals.getSize()) - juce::Decibels::gainToDecibels((float)samplePoints), -60.0f, 0.0f, 0.0f, 1.0f);
		return level;
	}

	inline void OpenGLFFTVisualizer::newFrameInit()
	{
		bin = 0;
	}

	OpenGLFFTLineVisualizer::OpenGLFFTLineVisualizer(GUI_Universals* universalsToUse, SpectrogramController* controller, bool isStereo, bool useLogFrequency, int numFramesToAvg)
		:OpenGLFunctionVisualizer(universalsToUse, false, true, controller->getSize())
	{
		vals.resize(numFramesToAvg);
		controller->setBufferToPushTo(&vals);

	}

	OpenGLFFTLineVisualizer::~OpenGLFFTLineVisualizer()
	{
	}

	inline float OpenGLFFTLineVisualizer::calculateFunctionSample(int sampleNumber, float openGL_X, float normX)
	{
		auto arr = vals.getInternalArray();
		float tot = 0;

		auto fftDataIndex = juce::jmap(universals->freqRange.convertFrom0to1(normX), universals->freqRange.start, universals->freqRange.end, 0.0f, (float)samplePoints - 1);

		for (int i = 0; i < vals.getSize(); ++i)
		{
			tot += juce::jmap(fftDataIndex - (int)fftDataIndex, arr[i][floor(fftDataIndex)], arr[i][ceil(fftDataIndex)]);
		}

		auto level = juce::jmap(juce::Decibels::gainToDecibels(tot / vals.getSize()) - juce::Decibels::gainToDecibels((float)samplePoints), -60.0f, 0.0f, -1.0f, 1.0f);
		return level;
	}

	inline void OpenGLFFTLineVisualizer::newFrameInit()
	{
	}

} // namespace bdsp
