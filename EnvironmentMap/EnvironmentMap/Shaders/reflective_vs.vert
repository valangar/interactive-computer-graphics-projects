#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_matrix;
uniform vec3 light_pos;

vec3 diffuse = vec3(0.7);
vec3 specular = vec3(1.0);
float power = 25.0;
vec3 ambient = vec3(0.6);

out vec3 Normal;
out vec3 Position;
out vec3 diffuse_comp;
out vec3 specular_comp;
out vec3 ambient_comp;

void main()
{
	vec4 pos = vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);
	vec4 P = view * model * pos;
	vec4 light = light_matrix * vec4(light_pos, 1.0);
	vec3 N = mat3(view * model) * normal;
	vec3 L = light.xyz - P.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	vec3 R = reflect(-L, N);

	diffuse_comp = max(dot(N, L), 0.0) * diffuse;
	specular_comp = pow(max(dot(R, V), 0.0), power) * specular;
	ambient_comp = ambient;
	Normal = mat3(transpose(inverse(view * model))) * normal;
	Position = vec3(view * model * pos);
	gl_Position = projection * view * model * pos;
}