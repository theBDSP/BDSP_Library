#pragma once
namespace bdsp
{
    inline const char* pitch_shift_vertex_shader = //vertex shader
        R"(
            #version 410 core
            
            // Input attributes.
                layout(location = 0) in vec2 position;
        
            out vec2 pos;

            // Simply sets and forwards the position to the fragment shader
            void main()
            {            
                gl_Position = vec4(position,0,1);
                pos = gl_Position.xy;
            }
        )";


    inline const char* pitch_shift_fragment_shader = //fragment shader
        R"(
            #version 410 core
            

			in vec2 pos;


            uniform float u_left;
            uniform float u_right;
            uniform float u_mix;
            uniform vec4 u_color;
            uniform vec2 u_viewport;

            layout(location = 0) out vec4 frag_color;
        

              float func(in vec2 p, in float m)
            {
                float x = p.x*u_viewport.x/u_viewport.y;
                float d = sqrt(x*x+p.y*p.y);
                return clamp((1.5-d),0.0,1.0)*smoothstep(0.0,0.5*m,abs(2.0*fract(3.5*m*d)-1.0));
            }

            void main()
            {
                vec4 gray =vec4(vec3((u_color.r+u_color.g+u_color.b)/3.0),u_color.a);
                
                vec4 d = vec4(gray.rgb,gray.a*func(pos,1.0));
                if(pos.x<0.0)
                {

                    if(pos.y<0.0)
                    {
                        frag_color = d;
                    }
                    else
                    {
                        frag_color = vec4(mix(gray,u_color,u_mix).rgb,u_color.a*func(pos,u_left));
                    }
                }
                else
                {
                    if(pos.y<0.0)
                    {
                        frag_color =d;
                    }
                    else
                    {
                        frag_color = vec4(mix(gray,u_color,u_mix).rgb,u_color.a*func(pos,u_right));
                    }
                }
            }
        )";

} //namespace bdspbdsp