#version 430

layout(location = 0) uniform mat4 model_matrix;
layout(location = 1) uniform mat4 projection_matrix;

layout(location = 0) in vec3 position;

void main(void)
{
	//fs_normal = vec4(normalize(normal_matrix * texelFetchBuffer(data, 2 * data_index + 1).rgb),1.0);
	//fs_normal = color;
	//vec4 pos = vec4(texelFetchBuffer(data, 2 * data_index + 0).rgb, 1.0);
    //vec4 p = projection_matrix * (model_matrix * pos);
	//fs_position = model_matrix * pos;
	//gl_Position = pos;//projection_matrix * (model_matrix * pos);
	//float f = clamp(float(data_index),0.0,26.0);
	//fs_position = model_matrix * pos;
	gl_Position = projection_matrix * (model_matrix * vec4(position, 1.0));
}