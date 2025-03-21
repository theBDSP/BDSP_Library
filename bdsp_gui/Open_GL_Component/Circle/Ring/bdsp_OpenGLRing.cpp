#include "bdsp_OpenGLRing.h"
namespace bdsp
{
	OpenGLRing::OpenGLRing(GUI_Universals* universalsToUse, int maxRings)
		:OpenGLCircleRenderer(universalsToUse, maxRings)
	{
	}





	void OpenGLRing::createShaders()
	{
		OpenGLCircleRenderer::createShaders();
		circleFragmentShader = ring_fragment_shader;
	}

	void OpenGLRing::createUniforms()
	{
		OpenGLCircleRenderer::createUniforms();

		thicknessUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*circleShaderProgram.get(), "u_thickness");
		thicknessUniform->set(thicknessVal);
	}

	void OpenGLRing::setThickness(float newThickness)
	{
		thicknessVal = newThickness;
		if (thicknessUniform.operator bool())
		{
			thicknessUniform->set(thicknessVal);
		}
	}



} //namespace bdsp