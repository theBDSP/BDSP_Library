#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* line_joint_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(lines_adjacency) in;
				layout(triangle_strip, max_vertices = 18) out; // max possible is 4(start cap) + 4(end cap) + 5(round joint ABC) + 5(round joint BCD) = 18 vertices

				uniform float u_thickness;
	            uniform int u_numVerts;
				uniform int u_jointType; // 0-miter, 1-bevel, 2-rounded
				uniform int u_capType; // 0-butt, 1-square, 2-round
				uniform vec2 u_viewport;

				in vec4 vertexColor[];
				in float index[];
				in float widthMult[];

				//Output to Fragment Shader
				out vec4 fragColor;
				out vec2 lineCenter; // center of the line, only used when creating rounded caps/joints
				out vec2 pos; // the position of this vertex in OpenGL space [-1,1], only used when creating rounded caps/joints
				out float isCap; // whether this vertex is part of a joint(0) or cap(1)
				
				const float PI2 = 1.5707963267948966192313216916398; // PI/2
				
				vec2 getThickness(in float screenThickness, inout float alphaMult)
				{

					alphaMult = 1;
					float thickMult = 1;
					if(screenThickness<2)
					{
						thickMult = 2/screenThickness;
						alphaMult = screenThickness/2;
					}


					return thickMult * screenThickness / u_viewport;
				}

				// calculates the intersection of 2 line segments from their endpoints
				vec4 findIntersection(in vec4 line1Start, in vec4 line1End, in vec4 line2Start, in vec4 line2End)
				{
					// calculate coefficients for line1 in two-point form
					float a1 = line1Start.y - line1End.y;
					float b1 = line1End.x - line1Start.x;
					float c1 = a1*line1Start.x+b1*line1Start.y;

					// calculate coefficients for line2 in two-point form
					float a2 = line2Start.y - line2End.y;
					float b2 = line2End.x - line2Start.x;
					float c2 = a2*line2Start.x+b2*line2Start.y;

					return vec4((c1*b2-c2*b1)/(a1*b2-a2*b1),(a1*c2-a2*c1)/(a1*b2-a2*b1),line1Start.zw); // calculate intersection, accuracy of the depth value is not important
				}

				void createCap(in bool start, in float angle, in vec4 d, in vec2 thickness, in vec4 color)
				{
					int idx = start ? 0 : 3; // index to grab values from
					vec4 delta; // how far to extend the line to create the cap
					vec4 center, outside, inside;
					float alphaMult;

					if(start)
					{
						delta = vec4(thickness * vec2(cos(angle+PI2),sin(angle+PI2)),0,0);

						center = gl_in[0].gl_Position;
						outside = center + d;
						inside = center - d;
					}	
					else
					{
						delta = vec4(thickness * vec2(cos(angle-PI2),sin(angle-PI2)),0,0);
						center = gl_in[3].gl_Position;
						outside = center + d;
						inside = center - d;

					}



					if(u_capType>0) //square or round
					{

						fragColor = color;
						lineCenter = center.xy;
						gl_Position = outside;
						pos = gl_Position.xy;
						isCap = 1;
						EmitVertex();	



						fragColor = color;
						lineCenter = center.xy;
						gl_Position = inside;
						pos = gl_Position.xy;
						isCap = 1;
						EmitVertex();	

						fragColor = color;
						lineCenter = center.xy;
						gl_Position = outside + delta;
						pos = gl_Position.xy;
						isCap = 1;
						EmitVertex();
	
						fragColor = color;
						lineCenter = center.xy;
						gl_Position = inside + delta;
						pos = gl_Position.xy;
						isCap = 1;
						EmitVertex();	

						EndPrimitive();
					}
				}

				// creates a joint from 3 consecutive points
				void createJoint(in vec4 point1, in vec4 point2, in vec4 point3, in vec2 thickness1, in vec2 thickness2, in vec2 thickness3, in vec2 polar12, in vec2 polar23, in vec4 color2, in bool dir123)
				{

					// calculate the with offset for each point and angle
					vec4 d112 = vec4(thickness1 * polar12,0,0); // point 1, angle 12
					vec4 d212 = vec4(thickness2 * polar12,0,0); // point 2, angle 12
					vec4 d223 = vec4(thickness2 * polar23,0,0); // point 2, angle 23
					vec4 d323 = vec4(thickness3 * polar23,0,0); // point 3, angle 23
				
					vec4 Io1 = findIntersection(point1+d112,point2+d212,point2+d223,point3+d323); // calculate the intersection of the outside edges of the 2 lines			

					// Center
					fragColor = color2;
					lineCenter = point2.xy;
					gl_Position = point2;
					pos = gl_Position.xy;
					isCap = 0;
					EmitVertex();	

					// Outside edge 1
					fragColor = color2;
					lineCenter = point2.xy;
					gl_Position = point2+d212;
					pos = gl_Position.xy;
					isCap = 0;
					EmitVertex();

					// Outside edge 2
					fragColor = color2;
					lineCenter = point2.xy;
					gl_Position = point2+d223;
					pos = gl_Position.xy;
					isCap = 0;
					EmitVertex();

					
					if(u_jointType == 0) // miter
					{

						// Miter intersection
						fragColor = color2;
						lineCenter = point2.xy;
						gl_Position = Io1;
						pos = gl_Position.xy;
						isCap = 0;
						EmitVertex();
					}
					else if(u_jointType == 2) // round
					{
						float midpointAngle = atan((point2+d223).y-(point2+d212).y,(point2+d223).x-(point2+d212).x) + PI2; // angle of the line that bisects the joint
						vec4 midPoint = point2 + (dir123 ? 1 : -1) * vec4(thickness2 * vec2(cos(midpointAngle),sin(midpointAngle)),0,0); // point on rounding circle halfway between the tow outside vertices of the joint (point2 + d212, point2 + d223)

						vec2 polarMid = thickness2 * vec2(cos(midpointAngle+PI2),sin(midpointAngle+PI2)); // tangent vector to the midPoint

						// Intersection of the tangent vector and the first outside edge
						fragColor = color2;
						lineCenter = point2.xy;
						gl_Position = findIntersection(midPoint,midPoint+vec4(polarMid,0,0),point1+d112,point2+d212);
						pos = gl_Position.xy;
						isCap = 0;
						EmitVertex();	

						// Intersection of the tangent vector and the second outside edge
						fragColor = color2;
						lineCenter = point2.xy;
						gl_Position = findIntersection(midPoint,midPoint+vec4(polarMid,0,0),point2+d223,point3+d323);
						pos = gl_Position.xy;
						isCap = 0;
						EmitVertex();
					}
					EndPrimitive();
				}




				void main()
				{
					vec4 A = gl_in[0].gl_Position;
					vec4 B = gl_in[1].gl_Position;
					vec4 C = gl_in[2].gl_Position;
					vec4 D = gl_in[3].gl_Position;
		
					float angleAB = atan(B.y-A.y,B.x-A.x)+PI2;
					float angleBC = atan(C.y-B.y,C.x-B.x)+PI2;
					float angleCD = atan(D.y-C.y,D.x-C.x)+PI2;
				
					// direction (clockwise or counter-clockwise) of the 2 triplets of vertices
					// necessary for determining outside vs. inside edges
					bool dirABC = ((A.x-B.x)*(C.y-B.y))-((C.x-B.x)*(A.y-B.y))>0;
					bool dirBCD = ((B.x-C.x)*(D.y-C.y))-((D.x-C.x)*(B.y-C.y))>0;
	
					vec2 polarAB = (dirABC ? 1 : -1) * vec2(cos(angleAB),sin(angleAB));
					vec2 polarBC = (dirABC ? 1 : -1) * vec2(cos(angleBC),sin(angleBC));
					vec2 polarCD = (dirBCD ? 1 : -1) * vec2(cos(angleCD),sin(angleCD));
					
					float alphaMultA,alphaMultB,alphaMultC,alphaMultD;
					vec2 thicknessA = getThickness(u_thickness*widthMult[0],alphaMultA);
					vec2 thicknessB = getThickness(u_thickness*widthMult[1],alphaMultB);
					vec2 thicknessC = getThickness(u_thickness*widthMult[2],alphaMultC);
					vec2 thicknessD = getThickness(u_thickness*widthMult[3],alphaMultD);
	
					if(index[0]<0.5) //first
					{
						createCap(true, angleAB, vec4(thicknessA*polarAB,0,0), thicknessA, vec4(vertexColor[0].rgb,vertexColor[0].a*alphaMultA));
					}
					if((index[3]+2)>u_numVerts) //last
					{
						createCap(false, angleCD, vec4(thicknessD*polarCD,0,0), thicknessD, vec4(vertexColor[3].rgb,vertexColor[3].a*alphaMultD));
						createJoint(B,C,D,thicknessB,thicknessC,thicknessD,(dirABC==dirBCD?1:-1)*polarBC,polarCD,vec4(vertexColor[2].rgb,vertexColor[2].a*alphaMultC), dirBCD); // if this is the last set of 4 vertices we need to also create the joint for BCD
					}
	
				createJoint(A,B,C,thicknessA,thicknessB,thicknessC,polarAB,polarBC,vec4(vertexColor[1].rgb,vertexColor[1].a*alphaMultB), dirABC);

				}		
				)";
} //namespace bdsp