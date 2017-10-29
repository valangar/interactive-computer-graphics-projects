#version 330 core

in vec3 Normal;
in vec3 Position;
in vec3 diffuse_comp;
in vec3 specular_comp;
in vec3 ambient_comp;
out vec3 color;	

uniform vec3 camera;
uniform samplerCube cubemap;
  
void main()
{
	vec3 I = normalize(Position - camera);
    vec3 R = reflect(I, normalize(Normal));
	color = ((ambient_comp + diffuse_comp  ) * texture(cubemap, R).rgb) + specular_comp;
} 