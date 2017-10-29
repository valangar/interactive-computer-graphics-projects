#version 330 core
	
uniform vec3 edge_color;

out vec4 color;

void main()
{
	color = vec4(edge_color, 1.0);
} 