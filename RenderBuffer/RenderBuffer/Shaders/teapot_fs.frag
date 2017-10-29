#version 330 core

uniform sampler2D tex_d;	
uniform sampler2D tex_s;
	
in vec3 one_color;
in vec3 diffuse_comp;
in vec3 specular_comp;
in vec3 ambient_comp;
in vec2 Texcoord;	
  
layout(location = 0) out vec3 color;

void main()
{
	vec3 diffuse_tex =  texture2D(tex_d, Texcoord).rgb ;
	vec3 specular_tex =  texture2D(tex_s, Texcoord).rgb;
	color = ambient_comp + (diffuse_comp * diffuse_tex) + (specular_comp * specular_tex);
} 