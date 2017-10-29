#version 330 core

in vec3 one_color;
in vec3 N;
  
out vec4 color;

void main()
{
	vec3 normalizedN = normalize(N);
    //color = vec4(normalizedN, 1.0f);
	color = vec4(one_color, 1.0f);
} 