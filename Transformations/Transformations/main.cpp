/*
CS6610 Project 2 - Transformations
AUTHOR: Varsha Alangar
Created on: 01/26/2017
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <GL/glfw3.h>
#include <cyCodeBase/cyMatrix.h>
#include <cyCodeBase/cyGL.h>
#include <cyCodeBase/cyTriMesh.h>
#include <iostream>
#include <cmath>
#include <GL/vmath.h>
#include <ctype.h>
#include <string.h>
#include <vector>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

int window_width = 600;
int window_height = 600;
int prevMouseX = 0;
int prevMouseY = 0;
int curMouseX = 0;
int curMouseY = 0;

float x_pos = 0.0f; 
float y_pos = 30.0f;
float z_pos = 100.0f;
float scale_factor = 15.0f;
float rotation_angle;

bool zoom = false;
bool rotate = false;
bool flag = false;

GLuint vao1;
GLuint vbo1;
GLuint buffer;
GLuint vs_id;
GLuint fs_id;
GLuint program1;
GLuint MatrixID;

cy::TriMesh trimesh_obj;
cy::GLSLShader glsl_shader_obj;
cy::GLSLProgram glsl_program_obj;
cy::Matrix4<float> matrix4_obj;
cy::Point3<float> objTrans;

vmath::vec3 getArcballVector(int x, int y);
vmath::mat4 rotationMatrix = vmath::mat4::identity();
vmath::mat4 translationMatrix = vmath::mat4::identity();
vmath::mat4 projection;
vmath::vec3 axis_in_camera_coord;


void idle()
{
	glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'p':
		case 'P': if (flag)
				  {
					 projection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
					 flag = false;
				  }
				  else
				  {
					// projection = vmath::ortho(0, window_width, window_height, 0, 0, 1000.0f);
					 flag = true;
				  }
			break;

		case 27: glutLeaveMainLoop();
			break;
	}
}

void special_keys(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_F6:	glsl_program_obj.BuildFiles("Shaders/simple_vs_2.vert", "Shaders/simple_fs_2.frag", NULL, NULL, NULL, &std::cout);
				break;
	}
}

void mouseButtonPress(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
			rotate = true;
			prevMouseX = curMouseX = x;
			prevMouseY = curMouseY = y;		
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
			zoom = true;
			prevMouseX = curMouseX = x;
			prevMouseY = curMouseY = y;
	}
	if (state == GLUT_UP)
	{
		rotate = false;
		zoom = false;
	}
}

void mouseMove(int x, int y)
{
	if (rotate || zoom)
	{
		curMouseX = x;
		curMouseY = y;
	}
}

void initialize()
{
	projection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	rotationMatrix *= vmath::rotate(vmath::degrees(rotation_angle), axis_in_camera_coord);

	//trimesh_obj.ComputeBoundingBox();
	//objTrans = (trimesh_obj.GetBoundMax() + trimesh_obj.GetBoundMin()) * 0.5f;

	glClearColor(0.0, 0.0, 0.0, 1.0); //black background
	trimesh_obj.LoadFromFileObj("Objects/teapot.txt", true);

	//now we generate and bind the vertex array object
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	//next, we generate a vertex buffer and set the vertices as its data
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)*trimesh_obj.NV(), &trimesh_obj.V(0), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	
}

void setShaders()
{
	glsl_program_obj.BuildFiles("Shaders/simple_vs_2.vert", "Shaders/simple_fs_2.frag", NULL, NULL, NULL, &std::cout);
	vs_id = glsl_shader_obj.GetID();
	fs_id = glsl_shader_obj.GetID();
	program1 = glsl_program_obj.GetID();
}

// Referred tutorial from the following website for Arcball:
// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
//Also referred to Superbible book examples

vmath::vec3 getArcballVector(int x, int y) {
	vmath::vec3 vecP = vmath::vec3(1.0f * x / window_width * 2 - 1.0f, 1.0f * y / window_height * 2 - 1.0f, 0.0f);
	vecP[1] = -vecP[1];
	float vecPsquared = vecP[0] * vecP[0] + vecP[1] * vecP[1];
	if (vecPsquared <= 1)
		vecP[2] = sqrt(1 - vecPsquared);
	else
		vecP = vmath::normalize(vecP);
	return vecP;
}

void render()
{
	rotation_angle = 0.0f;
	axis_in_camera_coord = vmath::vec3(0.0f, 1.0f, 0.0f);
	
	if (curMouseX != prevMouseX || curMouseY != prevMouseY) {
		if (rotate) {
			//Superbible book reference:
			vmath::vec3 prev = getArcballVector(prevMouseX, prevMouseY);
			vmath::vec3 curr = getArcballVector(curMouseX, curMouseY);
			rotation_angle = acos(fmin(1.0f, vmath::dot(prev, curr)));
			axis_in_camera_coord = vmath::cross(prev, curr);
			axis_in_camera_coord = vmath::normalize(axis_in_camera_coord);
			prevMouseX = curMouseX;
			prevMouseY = curMouseY;
			rotationMatrix *= vmath::rotate(vmath::degrees(rotation_angle), axis_in_camera_coord);
		}
		if (zoom) {
			z_pos += (curMouseY - prevMouseY);
			if (z_pos > 1000)
			{
				z_pos = 1000;
			}
			else if (z_pos < 10)
			{
				z_pos = 10;
			}
			prevMouseX = curMouseX;
			prevMouseY = curMouseY;
		}
	}

	vmath::mat4 perspective_matrix = projection;
	vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(x_pos, y_pos, z_pos), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	view_matrix *= translationMatrix;
	view_matrix *= rotationMatrix;
	vmath::mat4 model_matrix = vmath::scale(scale_factor);

	vmath::mat4 mvp = perspective_matrix * view_matrix * model_matrix;

	//Sending the mvp matrix to the vertex shader:
	MatrixID = glGetUniformLocation(program1, "mvp_matrix");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
	
	glEnableVertexAttribArray(0);
	glBindVertexArray(vao1);
	glsl_program_obj.Bind();
	glDrawArrays(GL_POINTS, 0, sizeof(cyPoint3f)*trimesh_obj.NV());
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv); //to initialize GLUT
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(window_height, window_width);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CS6610 Project 2 - Transformations");
	glewInit();
	glutDisplayFunc(render);
	glutKeyboardFunc(keys);
	glutSpecialFunc(special_keys);
	glutMouseFunc(mouseButtonPress);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);
	initialize();
	setShaders();
	glutMainLoop();
	return 0;
}
