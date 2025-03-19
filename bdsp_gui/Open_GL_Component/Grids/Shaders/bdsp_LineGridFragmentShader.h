#pragma once
namespace bdsp
{
	inline const char* line_grid_fragment_shader = //fragment shader
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
                vec2 position = vec2((pos.x+1)/2*u_viewport.x,(pos.y+1)/2*u_viewport.y);

                vec4 color = u_color;
                float w = u_size;

                if(u_size<1) // sub-pixel vertical and horizontal lines look bad, so instead we scale the alpha to represent smaller lines
                {
                    w=1;
                    color = vec4(color.rgb,color.a*u_size);
                }

                float xOffset = (u_centered.x ? -u_viewport.x/2 : 0)+u_size/2;
                bool vertical = mod(position.x+xOffset,u_spacing.x) < w;

                float yOffset = (u_centered.y ? -u_viewport.y/2 : 0)+u_size/2;
                bool horizontal = mod(position.y+yOffset,u_spacing.y) < w;

                if(horizontal || vertical)
                {
                    frag_color = color;
                }
                else
                {
                    discard;
                }
            }
        )";

} //namespace bdsp