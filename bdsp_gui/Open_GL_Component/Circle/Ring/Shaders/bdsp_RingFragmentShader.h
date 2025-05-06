#pragma once
namespace bdsp
{
	inline const char* ring_fragment_shader = //fragment shader
		R"(
            #version 410 core
            
            in vec4 fragColor;
			in vec2 pos;
			in vec2 center;
            in vec2 rad;

            uniform float u_thickness;
            uniform vec2 u_viewport;

            layout(location = 0) out vec4 frag_color;
        


            void main()
            {
                vec2 th = u_thickness/(rad*u_viewport);
                float t = (th.x+th.y)/2.0;
                float d = 1.0-distance(pos/rad,center/rad); // normalized distance of this fragment to the center of its outer circle
                float fade = 2.0*fwidth(d);
               

                
                float c1=smoothstep(0.0,fade,d);
                float c2=smoothstep(t+fade,t,d);

                frag_color = vec4(fragColor.rgb,fragColor.a*c1*c2);

            }
        )";

} //namespace bdsp