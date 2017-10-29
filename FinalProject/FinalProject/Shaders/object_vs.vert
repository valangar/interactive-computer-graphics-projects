#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat4 light_matrix;
uniform vec3 light_pos;
uniform int gooch;

float diffuse_cool = 0.3;
float diffuse_warm = 0.3;
vec3 cool = vec3(0.0, 0.0, 0.6);
vec3 warm = vec3(0.8, 0.8, 0.0);
vec3 color = vec3(0.3);
float power = 5.0;

out vec3 one_color;

void main()
{
	vec4 P = mv_matrix * vec4(0.05 * position.x, 0.05 * position.y, 0.05 * position.z, 1.0);
	vec4 light = vec4(light_pos, 1.0);
	vec3 N = mat3(mv_matrix) * normal;
	vec3 L = light.xyz - P.xyz;
	vec3 V = -P.xyz;

	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	vec3 R = reflect(-L, N);

	float intensity = dot(L, N);

	float diffuse_comp = max(dot(N, L), 0.0);
	float specular_comp = pow(max(dot(R, V), 0.0), power);

	vec3 min_cool = min(cool + diffuse_cool, 1.0);
	vec3 min_warm = min(warm + diffuse_warm, 1.0);
	vec3 final = mix(min_cool, min_warm, diffuse_comp);

	if(gooch == 0)
	{
		if(intensity > 0.9)
			one_color = vec3(1.0, 0.5, 0.5);
		else if (intensity > 0.5)
			one_color = vec3(0.6, 0.3, 0.3);
		else if (intensity > 0.2)
			one_color = vec3(0.4, 0.2, 0.2);
		else
			one_color = vec3(0.2,0.1,0.1);
	}
	else
	{
		one_color = vec3(min(final + specular_comp, 1.0));
	}

	gl_Position = mvp_matrix * vec4(0.05 * position.x, 0.05 * position.y, 0.05 * position.z, 1.0);

}