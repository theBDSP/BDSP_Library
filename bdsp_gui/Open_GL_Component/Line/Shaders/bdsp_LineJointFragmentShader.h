#pragma once
namespace bdsp
{
    inline const char* line_joint_fragment_shader = //fragment shader
        R"(
            #version 410 core
            
            in vec4 fragColor;
            in vec2 lineCenter; // center of the line, only used when creating rounded caps/joints
            in vec2 pos; // the position of this fragment in OpenGL space [-1,1], only used when creating rounded caps/joints
            in float isCap; // changes shader code depending on if current fragment is part of a cap or a joint
            in float widthMultiplier; // width of the line at the vertex this fragment is associated with

            uniform vec2 u_thickness;
			uniform int u_jointType; // 0-miter, 1-bevel, 2-rounded
			uniform int u_capType; // 0-butt, 1-square, 2-round

            layout(location = 0) out vec4 frag_color;

            void main()
            {
                if((isCap>0 && u_capType == 2) || (isCap<1 && u_jointType == 2)) // this fragment is either part of a rounded cap or rounded joint
                {
                    vec2 dist = (pos- lineCenter)/u_thickness; // normalized distance from this fragment to the line center
                    float d = sqrt(dist.x*dist.x+dist.y*dist.y);
                    if(d<=widthMultiplier) // fragment is within radius of cap/joint
                    {
                        frag_color =  fragColor;
                    }
                    else
                    {
                        discard;
                    }
                }
                else // this fragment is not part of a rounded cap or rounded joint, just output the color passed in
                {
                    frag_color = fragColor;
                }


                
            }
        )";

} //namespace bdsp