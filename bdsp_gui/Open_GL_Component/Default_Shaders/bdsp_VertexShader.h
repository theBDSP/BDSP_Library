#pragma once
namespace bdsp
{
	inline const char* default_vertex_shader =
        R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec4 position;
                       layout(location = 1) in vec4 sourceColor;
            
            // Output to fragment shader.
            out vec4 fragColor;
            
            void main()
            {
                gl_Position = position;
                fragColor = sourceColor;
            }
        )";

} //namespace bdsp