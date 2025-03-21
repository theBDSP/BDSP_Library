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
                vec2 t = u_thickness/u_viewport;
                float d = distance(pos/rad,center/rad); // normalized distance of this fragment to the center of its outer circle
                float di = distance(pos/(rad-t),center/(rad-t)); // normalized distance of this fragment to the center of its inner circle
 

                float r = (d-di)/(1.0-di);
                float a =  smoothstep(1.0-0.01/max(t.x,t.y),1.0,abs(r));
         
                frag_color = vec4(fragColor.rgb,fragColor.a*a);

            }
        )";

} //namespace bdsp