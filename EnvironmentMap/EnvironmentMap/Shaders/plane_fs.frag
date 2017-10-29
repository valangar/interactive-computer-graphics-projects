#version 330 core

in vec3 Normal;
in vec3 Position;
in vec2 UV;
out vec4 color;	

uniform vec3 camera;
uniform samplerCube cubemap;
uniform sampler2DRect renderTex;
  
void main()
{	
	vec2 tex = vec2(gl_FragCoord.x, gl_FragCoord.y);
	vec3 I = normalize(Position - camera);
    vec3 R = reflect(I, normalize(Normal));
	vec4 cubemap_tex = texture(cubemap, R).rgba;
	vec4 render_tex = texture(renderTex, tex).rgba;
	color = cubemap_tex + render_tex;
} 
