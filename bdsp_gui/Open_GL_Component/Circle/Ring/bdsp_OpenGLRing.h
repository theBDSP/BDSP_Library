#pragma once
#include "Shaders/bdsp_RingFragmentShader.h"
namespace bdsp
{
	/**
	 * Renders Rings, Ellipses with inner and outer radii.
	 * You have the same control over each ring as you do with the circle renderer, plus a uniform that sets the thickness of each ring
	 */
	class OpenGLRing : public OpenGLCircleRenderer
	{
	public:

		OpenGLRing(GUI_Universals* universalsToUse, int maxRings = 100);

		void createShaders() override;
		void createUniforms() override;

		void setThickness(float newThickness);

	private:
		std::unique_ptr<juce::OpenGLShaderProgram::Uniform> thicknessUniform;
		float thicknessVal = 1.0f;

	};
} //namespace bdsp