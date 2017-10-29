#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat4 light_matrix;
uniform vec3 light_pos;

float power = 5.0;

out float one_color;
out vec2 Texcoord;
out vec3 N;

void main()
{
	vec4 P = mv_matrix * vec4(0.045 *position.x, 0.045 *position.y, 0.045 *position.z, 1.0);
	vec4 light = vec4(light_pos, 1.0);
	vec3 N = mat3(mv_matrix) * normal;
	vec3 L = light.xyz - P.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	vec3 R = reflect(-L, N);

	one_color = max(dot(N, L), 0.0) + pow(max(dot(R, V), 0.0), power);
	Texcoord = vec2(texcoord.x, 1.0 - texcoord.y);

	gl_Position = mvp_matrix * vec4(0.045 *position.x, 0.045 *position.y, 0.045 *position.z, 1.0);

}