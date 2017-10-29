#version 330 core

uniform sampler2D tex_obj_1;	
uniform sampler2D tex_obj_2;	
uniform sampler2D tex_obj_3;	
uniform sampler2D tex_obj_4;	
uniform sampler2D tex_obj_5;	

in float one_color;
in vec2 Texcoord;
  
out vec4 color;

void main()
{
	if(one_color == 1.0)
	{
		color = vec4(1.0) ;
	}
	if(one_color > 0.9 )
	{
		color = vec4(texture2D(tex_obj_1, Texcoord).rgb, 1.0) ;
	}
	else if(one_color > 0.85)
	{
		color = vec4(texture2D(tex_obj_2, Texcoord).rgb, 1.0) ;
	}
	else if(one_color > 0.5)
	{
		color = vec4(texture2D(tex_obj_3, Texcoord).rgb, 1.0) ;
	}
	else if(one_color > 0.25)
	{
		color = vec4(texture2D(tex_obj_4, Texcoord).rgb, 1.0) ;
	}
	else
	{
		color = vec4(texture2D(tex_obj_5, Texcoord).rgb, 1.0) ;
	}
} 