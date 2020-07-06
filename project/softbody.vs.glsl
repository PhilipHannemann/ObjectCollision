#version 430
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_shader_draw_parameters : enable

//GLOBALS
struct LightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
};
layout(std140, binding = 0) uniform shader_data
{ 
	mat4 projection_matrix;
	mat4 view_matrix;
	mat4 normal_matrix;
	LightStruct lights[16];
	int numLigths;
} globals;

//INDIZES
layout(std140, binding = 1) uniform dynamic_data
{ 
	ivec4 dynamics[1000]; 
};

//ATTRIBUTES
layout(location = 1) in int data_index;
layout(location = 0) in vec2 tex;

//DATA
layout (binding=3, rgba32f) readonly uniform image2D vertex_data_tex;

//SHADER OUTPUT
out vec4 gs_normal;
out vec4 gs_position;
flat out ivec4 gs_indizes;
//out vec4 fs_color;

void main(void)
{
	gs_indizes = dynamics[dynamics[gl_DrawIDARB].x + gl_InstanceID];
	
	mat3 normal_matrix = transpose(inverse(mat3(globals.view_matrix))); //* mat3(ssbo[indizes.y].model_matrix)));
	//vec4 pos = ssbo[indizes.y].model_matrix * vec4(position, 1.0);		//*model_matrix

	//DATA from texture
	ivec2 data_id = ivec2(2 * data_index + 0, gs_indizes.z);
	vec4 pos    = vec4( imageLoad(vertex_data_tex, data_id).xyz, 1.0);
	//pos.x += gl_InstanceID;
	//pos.y += gs_indizes.z;
	data_id.x  += 1;
	vec3 normal = imageLoad(vertex_data_tex, data_id).xyz;
	
	//transforms
	vec4 eyespace_pos = globals.view_matrix * pos;
	gs_position = eyespace_pos;
	gl_Position = globals.projection_matrix * eyespace_pos;
	
	gs_normal = vec4(normalize(normal_matrix * normal), 1.0);

	//Get Data from TextureBuffer
	//vec4 pos =  vec4(texelFetchBuffer(data, 2 * data_index + 0).rgb, 1.0);
	//pos.y += float(gl_BaseInstanceARB) * 0.5;
	//pos.z += float(gl_InstanceID) * 0.5;
	//pos = model_matrix * pos;
	//fs_normal = vec4(normalize(normal_matrix * texelFetchBuffer(data, 2 * data_index + 1).rgb),1.0);	

	//fs_position = pos;
	//gl_Position = projection_matrix * pos;
	//fs_color = vec4((sin(pos.x*10) + 1.0)/2.0,0.0,0.0,1.0);
	//gl_Position = vec4(cos(gl_VertexID), sin(gl_VertexID), 0.0, 1.0);
}

