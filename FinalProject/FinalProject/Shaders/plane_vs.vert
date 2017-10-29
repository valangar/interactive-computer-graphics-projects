#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 plane_mvp_matrix;

out vec2 UV;

void main()
{
	vec4 P = vec4(1.5 * position.x , 1.5 * position.y , 1.5 * position.z , 1.0) ;
	gl_Position = plane_mvp_matrix * P;
	UV = (P.xy + vec2(1,1)) / 2.0;
}