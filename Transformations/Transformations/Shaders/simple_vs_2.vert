#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 mvp_matrix;

out vec4 one_color;

void main()
{
    gl_Position =  mvp_matrix * vec4( 0.05 *position.x,  0.05 *position.y, 0.05 *position.z, 1.0); 
    one_color = vec4(1.0f,0.0f,0.0f,1.0f);
}