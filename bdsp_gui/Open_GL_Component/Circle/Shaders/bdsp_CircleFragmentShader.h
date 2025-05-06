#pragma once
namespace bdsp
{
	inline const char* circle_fragment_shader = //fragment shader
		R"(
            #version 410 core
            
            in vec4 fragColor;
			in vec2 pos;
			in vec2 center;
            in vec2 rad;

            uniform vec2 u_viewport;

            layout(location = 0) out vec4 frag_color;
        


            void main()
            {
                float d = distance(pos/rad,center/rad); // normalized distance of this fragment to the center of its circle
                float fade = 2*fwidth(d);
                if(d<1)
                {
                    frag_color = vec4(fragColor.rgb, fragColor.a*(1-smoothstep(1-fade,1,d))); // MSAA doesn't work well enough here, need to feather the edge                
                }
                else
                {
                    discard;
                }


            }
        )";

} //namespace bdsp