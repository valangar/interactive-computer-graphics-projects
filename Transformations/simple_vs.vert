#version 330 core

attribute mat4 _ModelViewProjectionMatrix;
attribute vec4 _Vertex;

void main(void)
{
   gl_Position = 0.05 *  _ModelViewProjectionMatrix * _Vertex;

}