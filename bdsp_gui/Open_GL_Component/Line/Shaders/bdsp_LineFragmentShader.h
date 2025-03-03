#pragma once
namespace bdsp
{
    inline const char* line_fragment_shader = //fragment shader
        R"(
            #version 410 core
            
            in vec4 fragColor;


            layout(location = 0) out vec4 frag_color;



            void main()
            {
                frag_color = fragColor;                
            }
        )";

} //namespace bdsp