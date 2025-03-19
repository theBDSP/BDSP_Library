#pragma once
#include "bdsp_GridBase.h"
namespace bdsp
{
	OpenGLGridBase::OpenGLGridBase(GUI_Universals* universalsToUse)
		:OpenGLComponent(universalsToUse, 2),
		c(universalsToUse, this)
	{
		vertexBuffer.init(4);
		vertexBuffer.set(0, { -1,1 });
		vertexBuffer.set(1, { 1,1 });
		vertexBuffer.set(2, { -1,-1 });
		vertexBuffer.set(3, { 1,-1 });

		indexBuffer.addArray({ 0,1,2,1,2,3 });
	}
	void OpenGLGridBase::createVertexAttributes()
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
	void OpenGLGridBase::removeVertexAttributes()
	{
		juce::gl::glDisableVertexAttribArray(0);
	}
	void OpenGLGridBase::createShaders()
	{
		vertexShader = grid_vertex_shader;
	}

	void OpenGLGridBase::createUniforms()
	{
		spacing = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_spacing");
		spacing->set(spacingVal.first, spacingVal.second);

		size = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_size");
		size->set(sizeVal);

		color = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_color");
		float r, g, b, a;
		c.getComponents(r, g, b, a);
		color->set(r, g, b, a);

		centered = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_centered");
		centered->set(centeredVal.first, centeredVal.second);


		viewport = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*shaderProgram.get(), "u_viewport");
		viewport->set(vpBounds.getWidth(), vpBounds.getHeight());

	}
	void OpenGLGridBase::deleteUniforms()
	{
		spacing.reset();
		size.reset();
		color.reset();
		centered.reset();
		viewport.reset();
	}

	void OpenGLGridBase::setSpacing(float xSpacing, float ySpacing)
	{
		spacingVal = { xSpacing,ySpacing };
		if (spacing.operator bool())
		{
			shaderProgram->use();
			spacing->set(spacingVal.first, spacingVal.second);
		}

	}

	void OpenGLGridBase::setSize(float newSize)
	{
		sizeVal = newSize;
		if (size.operator bool())
		{
			shaderProgram->use();
			size->set(sizeVal);
		}

	}

	void OpenGLGridBase::setColor(const NamedColorsIdentifier& newColor)
	{
		c.setColors(newColor, newColor.withMultipliedAlpha(universals->disabledAlpha));

		if (color.operator bool())
		{
			shaderProgram->use();
			float r, g, b, a;
			c.getComponents(r, g, b, a);
			color->set(r, g, b, a);
		}
	}

	void OpenGLGridBase::setCentered(bool xCentered, bool yCentered)
	{
		centeredVal = { xCentered,yCentered };
		if (centered.operator bool())
		{
			shaderProgram->use();
			centered->set(centeredVal.first, centeredVal.second);
		}

	}

	void OpenGLGridBase::resized()
	{
		OpenGLComponent::resized();
		setViewportUniform();
	}

	void OpenGLGridBase::setViewportUniform()
	{
		if (viewport.operator bool())
		{
			viewport->set(vpBounds.getWidth(), vpBounds.getHeight());
		}
	}


	std::pair<float, float> OpenGLGridBase::getSpacing()
	{
		return spacingVal;
	}
	float OpenGLGridBase::getSize()
	{
		return sizeVal;
	}
	OpenGLColor OpenGLGridBase::getColor()
	{
		return c;
	}
	std::pair<bool, bool> OpenGLGridBase::getIsCentered()
	{
		return centeredVal;
	}
	std::pair<float, float> OpenGLGridBase::getViewportSize()
	{
		return { vpBounds.getWidth(),vpBounds.getHeight() };
	}
} //namespace bdsp
