#pragma once
#include "Shaders/bdsp_DotGridFragmentShader.h"
namespace bdsp
{
	class OpenGLDotGrid : public OpenGLGridBase
	{
	public:
		OpenGLDotGrid(GUI_Universals* universalsToUse)
			:OpenGLGridBase(universalsToUse)
		{

		}

		void createShaders() override
		{
			OpenGLGridBase::createShaders();
			fragmentShader = dot_grid_fragment_shader;
		}

	};
} //namespace bdsp