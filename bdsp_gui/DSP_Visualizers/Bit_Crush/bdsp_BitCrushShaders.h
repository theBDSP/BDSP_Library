#pragma once
namespace bdsp
{

	inline const char* bit_crush_vertex_shader = //vertex shader
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

	inline const char* bit_crush_fragment_shader = //fragment shader
		R"(
    #version 410 core
            
            in vec2 pos;

     
            uniform vec4 u_color;
			uniform float u_depth;
			uniform float u_rate;
			uniform float u_mix;

            layout(location = 0) out vec4 frag_color;
        
            // pertforms bit chrushing centered around 0
			float crush(in float x, in float br)
			{
					float p = pow(2.0,br);
					return floor(sign(x)*(x+0.5/p)*p)/p;
			}

            void main()
            {
                //perform bit curshing on the x and y positions
				float x = crush(abs(pos.x),u_rate);
				float y = crush(abs(pos.y),u_depth);
                
                float dry = sqrt(pos.x*pos.x+pos.y*pos.y);
				float a = sqrt(x*x+y*y);

				float gray = (u_color.r+u_color.g+u_color.b)/3.0; // grayscale value of u_color

				frag_color = mix(vec4(gray,gray,gray,(1-dry)*u_color.a),vec4(u_color.rgb,u_color.a*(1-a)),u_mix);
            }

			)";
} //namespace bdsp