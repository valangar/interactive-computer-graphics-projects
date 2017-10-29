#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat4 light_matrix;
uniform vec3 light_pos;

vec3 diffuse = vec3(0.5, 0.2, 0.7);
vec3 specular = vec3(0.7);
float power = 5.0;
vec3 ambient = vec3(0.2, 0.2, 0.2);

out vec3 one_color;
out vec3 N;

void main()
{
	//N = mat3(transpose(inverse(mv_matrix))) * normal;
    //gl_Position =  mvp_matrix * vec4( 0.05 *position.x,  0.05 *position.y, 0.05 *position.z, 1.0); 
    //one_color = vec3(1.0f,0.0f,0.0f);

	vec4 P = mv_matrix * vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);
	vec4 light = light_matrix * vec4(light_pos, 1.0);
	vec3 N = mat3(mv_matrix) * normal;
	vec3 L = light.xyz - P.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	vec3 R = reflect(-L, N);

	vec3 diffuse_comp = max(dot(N, L), 0.0) * diffuse;
	vec3 specular_comp = pow(max(dot(R, V), 0.0), power) * specular;

	one_color = (( ambient + diffuse_comp ) * vec3(1.0, 0.0, 0.0))+ specular_comp;

	gl_Position = mvp_matrix * vec4(0.05 *position.x, 0.05 *position.y, 0.05 *position.z, 1.0);

}