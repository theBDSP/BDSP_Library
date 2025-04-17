#pragma once
#include "Shaders/bdsp_LineGridFragmentShader.h"
namespace bdsp
{
	class OpenGLLineGrid : public OpenGLGridBase
	{
	public:
		OpenGLLineGrid(GUI_Universals* universalsToUse)
			:OpenGLGridBase(universalsToUse)
		{

		}

		void createShaders() override
		{
			OpenGLGridBase::createShaders();
			fragmentShader = line_grid_fragment_shader;
		}

	};
} //namespace bdsp