#version 430

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

in vec4 fs_normal;
in vec4 fs_position;

flat in ivec4 indizes;

in vec3 bary;

out vec4 color;

float edgeFactor()
{
    vec3 d = fwidth(bary);
    vec3 a3 = smoothstep(vec3(0.0), d * 1.5, bary);
    return min(min(a3.x, a3.y), a3.z);
}

void main(void)
{
	vec4 material = ssbo[indizes.y].color;
	color = vec4(0.0, 0.0, 0.0, 1.0);

	for(int i = 0; i < globals.numLigths; i++)
	{
		vec4 ldir = globals.lights[i].position - fs_position;
		float df = max(dot(fs_normal, normalize(ldir)), 0.0);
		color += material * globals.lights[i].diffuse * df;
		color += globals.lights[i].ambient * material;
	}
	
	color = clamp(color, 0.0, 1.0);
	
	if(material.a == 0.0)
	{
		color = vec4(0.0,0.0,1.0,1.0);//mix(color, vec4(0.0,0.0,1.0,1.0), (1.0 - edgeFactor()));
	}
	
	//color = fs_normal;
	//color = max(color, diffuse*0.2f);
	
	//color = (fs_normal + 1.0) / 2.0 ;
		
	//color = ssbo[indizes.y].color;
		
	//color = normalize(fs_normal)*0.5 + vec4(0.5,0.5,0.5,1.0);
	//color = vec4(length(fs_normal),0.0,0.0,0.0);
}