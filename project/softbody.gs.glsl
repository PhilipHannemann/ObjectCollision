#version 430

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;
 
in vec4 gs_normal[];
in vec4 gs_position[];
in flat ivec4 gs_indizes[];

out vec4 fs_normal;
out	vec4 fs_position;
flat out ivec4 indizes;
out vec3 bary;

 void main()
{
  for(int i = 0; i < 3; i++)
  {
    gl_Position = gl_in[i].gl_Position;
    fs_normal = gs_normal[i];
    fs_position = gs_position[i];
	indizes = gs_indizes[i];
	bary = vec3(0.0, 0.0, 0.0);
	bary[i] = 1.0;
    // done with the vertex
    EmitVertex();
  }
  
  EndPrimitive();
  
}









