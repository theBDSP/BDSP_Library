#pragma once
namespace bdsp
{
	inline const char* circle_vertex_shader = //vertex shader
		R"(
            #version 410 core
            
            // Input attributes.
                layout(location = 0) in vec2 position;
                layout(location = 1) in vec2 radius;
                layout(location = 2) in vec4 sourceColor;
            

            // Output to geometry shader.
            out vec4 vertexColor; // color of this vertex
            out vec2 radii; // radii of this circle
            void main()
            {            
                gl_Position = vec4(position,0,1);
                vertexColor = sourceColor;
                radii = radius;
            }
        )";




} //namespace bdsp