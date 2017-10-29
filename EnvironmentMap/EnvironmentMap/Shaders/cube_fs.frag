#version 330 core

in vec3 texCoord;
out vec4 color;

uniform samplerCube enviMap;

void main()
{
	color = texture(enviMap, texCoord);
}
