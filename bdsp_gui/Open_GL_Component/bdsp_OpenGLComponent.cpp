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
		vpBounds = getBoundsRelativeToDesktopManager();
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

		juce::gl::glEnable(juce::gl::GL_BLEND);
		juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

		if (shouldDrawBackground)
		{
			juce::OpenGLHelpers::clear(background);
		}

		renderWithoutGenerating();

	}



	void OpenGLComponent::openGLContextClosing()
	{
		//vertexBuffer.clear();
		//indexBuffer.clear();

		deleteUniforms();
		shaderProgram.reset();
		if (buffersCreated)
		{
			juce::gl::glDeleteBuffers(1, &vbo);
			juce::gl::glDeleteBuffers(1, &ibo);
		}

	}


	void OpenGLComponent::createShaders()
	{
		vertexShader = default_vertex_shader;

		fragmentShader = default_fragment_shader;
	}

	void OpenGLComponent::renderWithoutGenerating()
	{

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
	

	void OpenGLComponent::deleteUniforms()
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
	


	//================================================================================================================================================================================================





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


	//================================================================================================================================================================================================








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

	} // namespace bdsp
