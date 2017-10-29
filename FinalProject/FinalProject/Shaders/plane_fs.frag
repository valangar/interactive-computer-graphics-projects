#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderTex;
uniform int search_radius;
uniform int width;

float intensityFn(in vec4 color)
{
	return sqrt( (color.x * color.x) + (color.y * color.y) + (color.z * color.z) );
}

vec3 edgeDetection(in float step, in vec2 uv)
{
	float intensity = intensityFn(texture( renderTex, uv));
	int dark_count = 0;
	float max_intensity = intensity;
	for(int i = -search_radius; i <= search_radius; i++)
	{
		for(int j = -search_radius; j <= search_radius; j++)
		{
			vec2 loc = uv + vec2(i * step, j * step);
			float curr_intensity = intensityFn(texture( renderTex, loc));
			if(curr_intensity < intensity)
			{
				dark_count++;
			}
			if(curr_intensity > max_intensity)
			{
				max_intensity = curr_intensity;
			}
		}
	}
	if((max_intensity - intensity) > 0.01 * search_radius)
	{
		if(dark_count/(search_radius * search_radius) < ( 1 - (1/search_radius)))
		{
			return vec3(0.0, 0.0, 0.0); //contour detected, color it black
		}
	}
	return (texture( renderTex, uv).rgb + vec3(0.1, 0.1, 0.1)); //regular toon shading of object
}

void main()
{
	float step = 1.0 / width;
	color = vec4(edgeDetection(step, UV), 1.0);
}
