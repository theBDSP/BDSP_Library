#pragma once
namespace bdsp
{
    inline const char* function_vis_vertex_shader = // vertex shader
        R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec2 position;
                       layout(location = 1) in vec4 sourceColor;

            
            void main()
            {
                gl_Position = vec4(position.xy,0,1);
            }
        )";

} //namespace bdsp