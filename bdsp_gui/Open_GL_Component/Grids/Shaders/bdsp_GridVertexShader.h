#pragma once
namespace bdsp
{
	inline const char* grid_vertex_shader = //vertex shader
		R"(
            #version 410 core
            
            // Input attributes.
                layout(location = 0) in vec2 position;
        
            out vec2 pos;
            void main()
            {            
                gl_Position = vec4(position,0,1);
                pos = gl_Position.xy;
            }
        )";




} //namespace bdsp