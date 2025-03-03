#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* line_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(lines) in;
				layout(triangle_strip, max_vertices = 4) out;

				uniform vec2 u_thickness;
	            uniform int u_numVerts;

				in vec4 vertexColor[];
				in float index[];
				in float widthMult[];

				out vec4 fragColor;
				
				const float PI2 = 1.5707963267948966192313216916398; // PI/2
				

				// takes a line as input and outputs a quad representing the widened line to the fragment shader
				void main()
				{

					vec4 A = gl_in[0].gl_Position;
					vec4 B = gl_in[1].gl_Position;
					
					float angle = atan(B.y-A.y,B.x-A.x)+PI2; // angle of the normal to the line between A and B
				
					vec2 polar = u_thickness * vec2(cos(angle),sin(angle)); // width offset before per-vertex width multiplication

					vec4 delta1 = widthMult[0] * vec4(polar,0,0);
					vec4 delta2 = widthMult[1] * vec4(polar,0,0);

					fragColor = vertexColor[0];
					gl_Position = A + delta1;
					EmitVertex();

					fragColor = vertexColor[1];
					gl_Position = B + delta2;
					EmitVertex();


					fragColor = vertexColor[0];
					gl_Position = A-delta1;
					EmitVertex();

					fragColor = vertexColor[1];
					gl_Position = B-delta2;
					EmitVertex();

					
					EndPrimitive();

				}		
				)";
} //namespace bdsp
