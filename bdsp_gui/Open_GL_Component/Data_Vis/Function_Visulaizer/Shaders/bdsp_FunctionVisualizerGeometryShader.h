#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* funciton_vis_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(lines) in;
				layout(triangle_strip, max_vertices = 4) out;

				uniform float u_zeroY;


				out vec2 fragPos;
				
				

				// takes a line segment as input and outputs a quad representing the area between that line segment and the zero line
				void main()
				{
					vec4 A = gl_in[0].gl_Position;
					vec4 B = gl_in[1].gl_Position;
					
					gl_Position = A;
					fragPos = (gl_Position.xy+vec2(1,1))/2; // map position to UV space [0,1]
					EmitVertex();

					gl_Position = B;
					fragPos = (gl_Position.xy+vec2(1,1))/2; // map position to UV space [0,1]
					EmitVertex();


					gl_Position = vec4(A.x,u_zeroY,A.zw);
					fragPos = (gl_Position.xy+vec2(1,1))/2; // map position to UV space [0,1]
					EmitVertex();

					gl_Position = vec4(B.x,u_zeroY,B.zw);
					fragPos = (gl_Position.xy+vec2(1,1))/2; // map position to UV space [0,1]
					EmitVertex();

					
					EndPrimitive();
				}	
	
				)";
} //namespace bdsp
