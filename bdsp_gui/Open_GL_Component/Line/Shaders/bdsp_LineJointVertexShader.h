#pragma once
namespace bdsp
{
    inline const char* line_joint_vertex_shader = //vertex shader
        R"(
            #version 410 core
            
            // Input attributes.
                       layout(location = 0) in vec2 position;
                       layout(location = 1) in vec4 sourceColor;
            
            // Output to geometry shader.
            out vec4 vertexColor; // color of this vertex
            out float index; // index of this vertex
            out float widthMult; // width multiplier of this vertex, calculated from the thickness ramp uniform and the index
            
            uniform int u_numVerts;
            uniform float u_thicknessRamp;
            void main()
            {
                
                index = gl_VertexID;
                gl_Position = vec4(position.xy,index/u_numVerts,1);
                vertexColor = sourceColor;

                if(u_thicknessRamp < 0)
                {
                    widthMult = mix(1,-u_thicknessRamp,index/(u_numVerts+1)); // decrease the width from 1 to the absolute value of the thickness ramp uniform
                }
                else
                {
                    widthMult = mix(u_thicknessRamp,1,index/(u_numVerts+1)); // increase the width from the value of the thickness ramp uniform to 1
                }

            }
        )";




} //namespace bdsp