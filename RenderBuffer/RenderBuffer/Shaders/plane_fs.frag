#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderTex;

void main()
{
	color = vec4(texture( renderTex, UV).rgb, 1.0) + vec4(0.1, 0.1, 0.1, 0.5);
}
