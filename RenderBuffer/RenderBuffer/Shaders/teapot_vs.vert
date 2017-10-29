#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat4 light_matrix;
uniform vec3 light_pos;
uniform vec3 Kd;
uniform vec3 Ks;

vec3 diffuse = vec3(0.5, 0.2, 0.7);
vec3 specular = vec3(0.7);
float power = 5.0;
vec3 ambient = vec3(0.2, 0.2, 0.2);

out vec3 one_color;
out vec3 diffuse_comp;
out vec3 specular_comp;
out vec3 ambient_comp;
out vec2 Texcoord;

void main()
{
	vec4 P = mv_matrix * vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);
	vec4 light = light_matrix * vec4(light_pos, 1.0);
	vec3 N = mat3(mv_matrix) * normal;
	vec3 L = light.xyz - P.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	vec3 R = reflect(-L, N);

	diffuse_comp = max(dot(N, L), 0.0) * Kd;
	specular_comp = pow(max(dot(R, V), 0.0), power) * Ks;
	ambient_comp = ambient * Kd;

	one_color = (( ambient + diffuse_comp ) * vec3(1.0, 0.0, 0.0))+ specular_comp;
	
	Texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
	
	gl_Position = mvp_matrix * vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);

}