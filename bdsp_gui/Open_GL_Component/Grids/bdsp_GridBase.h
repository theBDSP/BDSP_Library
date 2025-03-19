#pragma once
#include "Shaders/bdsp_GridVertexShader.h"
namespace bdsp
{
	/**
	 * Provides everything needed to implement a grid via a fragment shader, all you need to do is write the fragment shader
	 */
	class OpenGLGridBase : public OpenGLComponent
	{
	public:

		OpenGLGridBase(GUI_Universals* universalsToUse);
		void createVertexAttributes() override;
		void removeVertexAttributes() override;

		void createShaders() override;
		void createUniforms() override;
		void deleteUniforms() override;
		void setSpacing(float xSpacing, float ySpacing);
		void setSize(float newSize);
		void setColor(const NamedColorsIdentifier& newColor);
		void setCentered(bool xCentered, bool yCentered);
		void resized() override;

		std::pair<float,float> getSpacing();
		float getSize();
		OpenGLColor getColor();
		std::pair<bool, bool> getIsCentered();
		std::pair<float, float> getViewportSize();

		void setViewportUniform();
	protected:

		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> spacing, size, color, centered, viewport;
		std::pair<float, float> spacingVal = { 10.0f,10.0f };
		float sizeVal = 1.0f;
		OpenGLColor c;
		std::pair<bool, bool> centeredVal = {true,true};
	};

} //namespace bdsp