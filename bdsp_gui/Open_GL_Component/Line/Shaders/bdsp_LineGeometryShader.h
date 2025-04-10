#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* line_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(lines) in;
				layout(triangle_strip, max_vertices = 4) out;

				uniform float u_thickness;
	            uniform int u_numVerts;
				uniform vec2 u_viewport;

				in vec4 vertexColor[];
				in float index[];
				in float widthMult[];

				out vec4 fragColor;
				
				const float PI2 = 1.5707963267948966192313216916398; // PI/2
				
				
						
				vec2 getThickness(in float screenThickness, inout float alphaMult)
				{

					alphaMult = 1;
					float thickMult = 1;
					if(screenThickness<1)
					{
						thickMult = 1/screenThickness;
						alphaMult = screenThickness;
					}


					return thickMult * screenThickness / u_viewport;
				}

				// takes a line as input and outputs a quad representing the widened line to the fragment shader
				void main()
				{

					vec4 A = gl_in[0].gl_Position;
					vec4 B = gl_in[1].gl_Position;
					
					float angle = atan(B.y-A.y,B.x-A.x)+PI2; // angle of the normal to the line between A and B
				

					vec2 polar = vec2(cos(angle),sin(angle)); // width offset before per-vertex width multiplication

					float alphaMult1,alphaMult2;
					vec4 delta1 = vec4(getThickness(widthMult[0]*u_thickness,alphaMult1) * polar,0,0);
					vec4 delta2 = vec4(getThickness(widthMult[1]*u_thickness,alphaMult2) * polar,0,0);

					fragColor = vec4(vertexColor[0].rgb,vertexColor[0].a*alphaMult1);
					gl_Position = A + delta1;
					EmitVertex();

					fragColor = vec4(vertexColor[1].rgb,vertexColor[1].a*alphaMult2);
					gl_Position = B + delta2;
					EmitVertex();


					fragColor = vec4(vertexColor[0].rgb,vertexColor[0].a*alphaMult1);
					gl_Position = A-delta1;
					EmitVertex();

					fragColor = vec4(vertexColor[1].rgb,vertexColor[1].a*alphaMult2);
					gl_Position = B-delta2;
					EmitVertex();

					
					EndPrimitive();

				}		
				)";
} //namespace bdsp
