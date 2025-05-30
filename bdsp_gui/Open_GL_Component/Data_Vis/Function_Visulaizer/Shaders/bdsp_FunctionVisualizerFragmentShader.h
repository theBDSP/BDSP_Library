#pragma once
namespace bdsp
{
	inline const char* funciton_vis_fragment_shader = // fragment shader
		R"(
            #version 410 core
            
            layout(location = 0) in vec2 fragPos;

            uniform float u_zeroY;
            uniform float u_pos;
            uniform float u_width;
            uniform vec4 u_topColor;
            uniform vec4 u_botColor;
            uniform vec4 u_posColor;
            uniform float u_endpoint;

            layout(location = 0) out vec4 frag_color;



            void main()
            {
                float y = 2*fragPos.y-1;
                float v = abs(y-u_zeroY)/(1+abs(u_zeroY));
                vec4 c =  mix(u_botColor, u_topColor,v); // base color, mix between top and bottom color based on y position
                float wrap = fract((fragPos.x-u_pos)/u_endpoint); // adjusted x value of fragment based on its position and the position uniform
                float m = 0; // amount of position color to add in
                if(fragPos.x<u_endpoint && wrap>1-u_width/u_endpoint) // only calculate if within the width of the falloff
                {
                    m = u_endpoint*wrap/u_width + (u_width-u_endpoint)/u_width;
                    m*=m;
                }
                frag_color =  c + m*u_posColor;
            }
				)";
} //namespace bdsp
