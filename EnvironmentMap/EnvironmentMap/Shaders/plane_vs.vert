#version 330 core
layout (location = 0) in vec3 position;
vec3 normal = vec3(0.0f, 0.0f, 1.0f);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 Position;
out vec2 UV;

void main()
{
	vec4 pos = vec4(0.05 * position.x, 0.05 * position.y, 0.05 * position.z, 1.0);
	
	vec2 uv = (pos.xy + vec2(1, 1)) / 2.0;
	UV = vec2(uv.x, 1.0 - uv.y);
	Normal = mat3(transpose(inverse(view * model))) * normal;
	Position = vec3(view * model * pos);
	gl_Position = projection * view * model * pos;
}
