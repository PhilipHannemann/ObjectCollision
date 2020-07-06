#version 430

layout(location = 100) uniform vec4 diffuse;

out vec4 color;

void main(void)
{
	//vec4 ldir = pos - fs_position;
	//float df = max(dot(fs_normal, normalize(ldir)), 0.0);
	//color = diffuse * df;
	//color = fs_normal;
	//color = max(color, diffuse*0.2f);
		
	//color = (fs_normal + 1.0) / 2.0 ;
		
	//color = normalize(fs_normal)*0.5 + vec4(0.5,0.5,0.5,1.0);
	//color = vec4(length(fs_normal),0.0,0.0,0.0);
	
	color = diffuse;
}