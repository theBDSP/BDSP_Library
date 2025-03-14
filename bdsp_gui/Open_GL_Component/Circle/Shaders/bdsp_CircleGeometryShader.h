#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* circle_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(points) in;
				layout(triangle_strip, max_vertices = 4) out;

				in vec4 vertexColor[];
				in vec2 radii[];

	            uniform vec2 u_viewport;

				out vec4 fragColor;
				out vec2 pos;
				out vec2 center;
				out vec2 rad;
				

				void main()
				{
					vec4 c = gl_in[0].gl_Position; // center of the circle
					vec2 r = 2 * radii[0]/u_viewport; // x and y radius of the circle


					fragColor = vertexColor[0];
					gl_Position = c + vec4(r.x,r.y,0,0);
					pos = gl_Position.xy;
					center = c.xy;
					rad = r;
					EmitVertex();

					fragColor = vertexColor[0];
					gl_Position = c + vec4(r.x,-r.y,0,0);
					pos = gl_Position.xy;
					center = c.xy;
					rad = r;
					EmitVertex();

					fragColor = vertexColor[0];
					gl_Position = c + vec4(-r.x,r.y,0,0);
					pos = gl_Position.xy;
					center = c.xy;
					rad = r;
					EmitVertex();

					fragColor = vertexColor[0];
					gl_Position = c + vec4(-r.x,-r.y,0,0);
					pos = gl_Position.xy;
					center = c.xy;
					rad = r;
					EmitVertex();

					
					EndPrimitive();

				}		
				)";
} //namespace bdsp
