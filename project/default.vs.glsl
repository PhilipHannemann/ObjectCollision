#version 430
#extension GL_ARB_enhanced_layouts : enable
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

//SSBO
struct ssbo_block
{
	mat4 model_matrix;
	mat4 normal_matrix;
	vec4 color;
};
layout(std430, binding = 2) buffer ssbo_data
{ 
	ssbo_block ssbo[];
};

//ATTRIBUTES
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

//SHADER OUTPUT
out vec4 fs_normal;
out vec4 fs_position;
flat out ivec4 indizes;

void main(void)
{
	indizes = dynamics[dynamics[gl_DrawIDARB].x + gl_InstanceID];
	//index = indizes;
	
	//diffuse = ssbo[indizes.y].color;
	
	mat3 normal_matrix = transpose(inverse(mat3(globals.view_matrix) * mat3(ssbo[indizes.y].model_matrix)));
	
	vec4 pos = ssbo[indizes.y].model_matrix * vec4(position, 1.0);		//*model_matrix
	
	//test
	//pos.y += float(indizes.y);
	//pos.z += ssbo[indizes.y].color.x; 
	
	//transforms
	vec4 eyespace_pos = globals.view_matrix * pos;
	fs_position = eyespace_pos;
	gl_Position = globals.projection_matrix * eyespace_pos;
	
	fs_normal = vec4(normalize(normal_matrix * normal), 1.0);
}


//vec4 pos = vec4(texelFetchBuffer(data, 2 * data_index + 0).rgb, 1.0);