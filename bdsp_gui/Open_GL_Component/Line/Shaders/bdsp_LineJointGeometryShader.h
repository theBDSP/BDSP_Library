#pragma once
namespace bdsp
{
	// compute shader comment added for GLSL syntax highlighting extension in Visual Studio (does not support geometry shaders)
	inline const char* line_joint_geometry_shader = // compute shader
		R"(
            #version 410 core

				layout(lines_adjacency) in;
				layout(triangle_strip, max_vertices = 18) out; // max possible is 4(start cap) + 4(end cap) + 5(round joint ABC) + 5(round joint BCD) = 18 vertices

				uniform vec2 u_thickness;
	            uniform int u_numVerts;
				uniform int u_jointType; // 0-miter, 1-bevel, 2-rounded
				uniform int u_capType; // 0-butt, 1-square, 2-round

				in vec4 vertexColor[];
				in float index[];
				in float widthMult[];

				//Output to Fragment Shader
				out vec4 fragColor;
				out vec2 lineCenter; // center of the line, only used when creating rounded caps/joints
				out vec2 pos; // the position of this vertex in OpenGL space [-1,1], only used when creating rounded caps/joints
				out float isCap; // whether this vertex is part of a joint(0) or cap(1)
				out float widthMultiplier; // width multiplier of the line at this vertex
				
				const float PI2 = 1.5707963267948966192313216916398; // PI/2
				

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

				void createCap(in bool start, in float angle, in vec4 dAB, in vec4 dCD)
				{
					int idx = start ? 0 : 3; // index to grab values from
					vec4 delta; // how far to extend the line to create the cap
					vec4 center, outside, inside;
					float w; // width multiplier for the cap

					if(start)
					{
						delta = vec4(u_thickness * vec2(cos(angle+PI2),sin(angle+PI2)),0,0);

						center = gl_in[0].gl_Position;
						outside = center + dAB;
						inside = center - dAB;
						w = widthMult[0];
					}	
					else
					{
						delta = vec4(u_thickness * vec2(cos(angle-PI2),sin(angle-PI2)),0,0);
						center = gl_in[3].gl_Position;
						outside = center + dCD;
						inside = center - dCD;
						w = widthMult[3];
					}

					if(u_capType>0) //square or round
					{

						fragColor = vertexColor[idx];
						lineCenter = center.xy;
						gl_Position = outside;
						pos = gl_Position.xy;
						isCap = 1;
						widthMultiplier = w;
						EmitVertex();	



						fragColor = vertexColor[idx];
						lineCenter = center.xy;
						gl_Position = inside;
						pos = gl_Position.xy;
						isCap = 1;
						widthMultiplier = w;
						EmitVertex();	

						fragColor = vertexColor[idx];
						lineCenter = center.xy;
						gl_Position = outside + delta;
						pos = gl_Position.xy;
						isCap = 1;
						widthMultiplier = w;
						EmitVertex();
	
						fragColor = vertexColor[idx];
						lineCenter = center.xy;
						gl_Position = inside + delta;
						pos = gl_Position.xy;
						isCap = 1;
						widthMultiplier = w;
						EmitVertex();	

						EndPrimitive();
					}
				}

				// creates a joint from 3 consecutive points
				void createJoint(in int idx1, in int idx2, in int idx3, in vec4 d12, in vec4 d23, in bool dir123)
				{
					//define the three points
					vec4 point1 = gl_in[idx1].gl_Position;
					vec4 point2 = gl_in[idx2].gl_Position;
					vec4 point3 = gl_in[idx3].gl_Position;


					// calculate the with offset for each point and angle
					vec4 d112 = d12 * widthMult[idx1]; // point 1, angle 12
					vec4 d212 = d12 * widthMult[idx2]; // point 2, angle 12
					vec4 d223 = d23 * widthMult[idx2]; // point 2, angle 23
					vec4 d323 = d23 * widthMult[idx3]; // point 3, angle 23
				
					vec4 Io1 = findIntersection(point1+d112,point2+d212,point2+d223,point3+d323); // calculate the intersection of the outside edges of the 2 lines			

					// Center
					fragColor = vertexColor[idx2];
					lineCenter = point2.xy;
					gl_Position = point2;
					pos = gl_Position.xy;
					isCap = 0;
					widthMultiplier = widthMult[idx2];
					EmitVertex();	

					// Outside edge 1
					fragColor = vertexColor[idx2];
					lineCenter = point2.xy;
					gl_Position = point2+d212;
					pos = gl_Position.xy;
					isCap = 0;
					widthMultiplier = widthMult[idx2];
					EmitVertex();

					// Outside edge 2
					fragColor = vertexColor[idx2];
					lineCenter = point2.xy;
					gl_Position = point2+d223;
					pos = gl_Position.xy;
					isCap = 0;
					widthMultiplier = widthMult[idx2];
					EmitVertex();

					
					if(u_jointType == 0) // miter
					{

						// Miter intersection
						fragColor = vertexColor[idx2];
						lineCenter = point2.xy;
						gl_Position = Io1;
						pos = gl_Position.xy;
						isCap = 0;
						widthMultiplier = widthMult[idx2];
						EmitVertex();
					}
					else if(u_jointType == 2) // round
					{
						float midpointAngle = atan((point2+d223).y-(point2+d212).y,(point2+d223).x-(point2+d212).x) + PI2; // angle of the line that bisects the joint
						vec4 midPoint = point2 + (dir123 ? 1 : -1) * widthMult[idx2] * vec4(u_thickness * vec2(cos(midpointAngle),sin(midpointAngle)),0,0); // point on rounding circle halfway between the tow outside vertices of the joint (point2 + d212, point2 + d223)
									
						vec2 polarMid = widthMult[idx2] * u_thickness * vec2(cos(midpointAngle+PI2),sin(midpointAngle+PI2)); // tangent vector to the midPoint

						// Intersection of the tangent vector and the first outside edge
						fragColor = vertexColor[idx2];
						lineCenter = point2.xy;
						gl_Position = findIntersection(midPoint,midPoint+vec4(polarMid,0,0),point1+d112,point2+d212);
						pos = gl_Position.xy;
						isCap = 0;
						widthMultiplier = widthMult[idx2];
						EmitVertex();	

						// Intersection of the tangent vector and the second outside edge
						fragColor = vertexColor[idx2];
						lineCenter = point2.xy;
						gl_Position = findIntersection(midPoint,midPoint+vec4(polarMid,0,0),point2+d223,point3+d323);
						pos = gl_Position.xy;
						isCap = 0;
						widthMultiplier = widthMult[idx2];
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

					vec4 dAB = vec4(u_thickness * polarAB,0,0);
					vec4 dBC = vec4(u_thickness * polarBC,0,0);
					vec4 dCD = vec4(u_thickness * polarCD,0,0);
					
		

					
					if(index[0]<0.5) //first
					{
						createCap(true, angleAB, widthMult[0] * dAB, dCD);
					}
					if((index[3]+2)>u_numVerts) //last
					{
						createCap(false, angleCD, dAB, widthMult[3] * dCD);
						createJoint(1,2,3, (dirABC==dirBCD ? 1:-1) * dBC,dCD, dirBCD); // if this is the last set of 4 vertices we need to also create the joint for BCD
					}
	
				createJoint(0,1,2, dAB, dBC, dirABC);

				}		
				)";
} //namespace bdsp