#pragma once
namespace bdsp
{
	inline const char* dot_grid_fragment_shader = //fragment shader
		R"(
            #version 410 core
            
            in vec2 pos;

            uniform vec2 u_viewport;
            uniform vec2 u_spacing;
            uniform vec4 u_color;
            uniform bvec2 u_centered;
            uniform float u_size;

            layout(location = 0) out vec4 frag_color;
        


            void main()
            {
                vec2 position = vec2((pos.x+1)/2*u_viewport.x,(pos.y+1)/2*u_viewport.y); // get this fragment's position in screen space

                // offset to add if axis should be centered
                float xOffset = (u_centered.x ? -u_viewport.x/2 : 0); 
                float yOffset = (u_centered.y ? -u_viewport.y/2 : 0);

                vec2 p =vec2(u_spacing.x/2-abs(mod(position.x+ xOffset, u_spacing.x)-u_spacing.x/2.0),u_spacing.y/2-abs(mod(position.y + yOffset, u_spacing.y)-u_spacing.y/2.0)); // point relative to nearest horizontal and vertical grid line

                frag_color = vec4(u_color.rgb,u_color.a * (1 - smoothstep(0.5,1,distance(p,vec2(0,0))/u_size)));

            }
        )";

} //namespace bdsp