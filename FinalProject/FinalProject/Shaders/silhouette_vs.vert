#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvp_matrix;
uniform float normal_offset;

void main()
{
	vec4 pos = vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);
	vec4 P = pos + vec4(normal, 1.0) * normal_offset;
	gl_Position = mvp_matrix * P;

}