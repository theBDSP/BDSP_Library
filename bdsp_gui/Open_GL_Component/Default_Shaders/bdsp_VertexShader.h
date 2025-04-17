#pragma once
namespace bdsp
{
	inline const char* default_vertex_shader = // vertex shader
        R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec2 position;
                       layout(location = 1) in vec4 sourceColor;
            
            // Output to fragment shader.
            out vec4 fragColor;
            
            void main()
            {
                gl_Position = vec4(position.xy,0,1);
                fragColor = sourceColor;
            }
        )";

} //namespace bdsp