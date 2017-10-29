#version 330 core
layout (location = 0) in vec3 position;

out vec3 texCoord;

uniform mat4 mvp_matrix;

void main()
{
	vec4 P = vec4(position.x ,  position.y , position.z , 1.0) ;
	gl_Position = mvp_matrix * P;
	texCoord = vec3(P);
}