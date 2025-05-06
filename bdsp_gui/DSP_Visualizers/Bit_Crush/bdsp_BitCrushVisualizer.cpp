#include "bdsp_BitCrushVisualizer.h"

namespace bdsp
{
	BitCrushVisualizerInternal::BitCrushVisualizerInternal(GUI_Universals* universalsToUse, BaseSlider* bitDepthSlider, BaseSlider* sampleRateSlider, BaseSlider* mixSlider)
		: OpenGLComponent(universalsToUse, 2),
		color(universalsToUse, this)
	{
		bitDepth = bitDepthSlider;
		sampleRate = sampleRateSlider;
		mix = mixSlider;
		vertexBuffer.init(4);
		indexBuffer.addArray({ 0,1,2,1,2,3 });
		universals->contextHolder->registerOpenGlRenderer(this);
	}



	BitCrushVisualizerInternal::~BitCrushVisualizerInternal()
	{
		universals->contextHolder->unregisterOpenGlRenderer(this);
	}


	void BitCrushVisualizerInternal::setColor(const NamedColorsIdentifier& newColor)
	{
		color.setColors(newColor,newColor.withMultipliedAlpha(universals->disabledAlpha));
	}

	void BitCrushVisualizerInternal::setScaling(float x, float y)
	{
		xScaling = x;
		yScaling = y;
		resized();
	}


	void BitCrushVisualizerInternal::generateVertexBuffer()
	{
		shaderProgram->use();

		mixVal = mix->getNormalisableRange().convertTo0to1(mix->getActualValue());

		double hiRes = 6.0;
		double loRes = 2.0;
		depthVal = juce::jmap(bitDepth->getActualValue(), hiRes, loRes);
		rateVal = juce::jmap(sampleRate->getNormalisableRange().convertTo0to1(sampleRate->getActualValue()), hiRes, loRes);

		bitDepthUniform->set(depthVal);
		rateUniform->set(rateVal);
		mixUniform->set(mixVal);

		float r, g, b, a;
		color.getComponents(r, g, b, a);
		colorUniform->set(r, g, b, a);


		vertexBuffer.set(0, { -xScaling,yScaling });
		vertexBuffer.set(1, { xScaling,yScaling });
		vertexBuffer.set(2, { -xScaling,-yScaling });
		vertexBuffer.set(3, { xScaling,-yScaling });

	}
	//================================================================================================================================================================================================


	void BitCrushVisualizerInternal::createVertexAttributes()
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
	}
	void BitCrushVisualizerInternal::removeVertexAttributes()
	{
		juce::gl::glDisableVertexAttribArray(0);
	}
	void BitCrushVisualizerInternal::createShaders()
	{
		vertexShader = bit_crush_vertex_shader;
		fragmentShader = bit_crush_fragment_shader;
	}

	void BitCrushVisualizerInternal::createUniforms()
	{
		colorUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_color");
		float r, g, b, a;
		color.getComponents(r, g, b, a);
		colorUniform->set(r, g, b, a);

		bitDepthUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_depth");
		rateUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_rate");
		mixUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_mix");

	}
	void BitCrushVisualizerInternal::deleteUniforms()
	{
		colorUniform.reset();
		bitDepthUniform.reset();
		rateUniform.reset();
	}






	//================================================================================================================================================================================================






} // namespace bdsp
