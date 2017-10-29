/*
CS6610 Project 2 - Transformations
AUTHOR: Varsha Alangar
Created on: 01/26/2017
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
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

std::vector<unsigned int> indices;

int window_width = 600;
int window_height = 600;
int prevMouseX = 0;
int prevMouseY = 0;
int curMouseX = 0;
int curMouseY = 0;

float x_pos = 0.0f;
float y_pos = -90.0f;
float z_pos = 100.0f;
float scale_factor = 25.0f;
float rotation_angle;
float light_rotation_angle;

bool zoom = false;
bool rotate = false;
bool flag = false;
bool ctrl = false;
bool ctrl_flag = false;

GLuint vao1;
GLuint vbo1;
GLuint buffer;
GLuint element_buffer;
GLuint normal_buffer;
GLuint normal_element_buffer;
GLuint vs_id;
GLuint fs_id;
GLuint program1;
GLuint mvpMatrixID;
GLuint mvMatrixID;
GLuint lightPosID;
GLuint lightMatrixID;

cy::TriMesh trimesh_obj;
cy::GLSLShader glsl_shader_obj;
cy::GLSLProgram glsl_program_obj;
cy::Matrix4<float> matrix4_obj;
cy::Matrix3<float> matrix3_obj;
cy::Point3<float> objTrans;

vmath::vec3 getArcballVector(int x, int y);
vmath::mat4 rotationMatrix = vmath::mat4::identity();
vmath::mat4 lightRotationMatrix = vmath::mat4::identity();
vmath::mat4 translationMatrix = vmath::mat4::identity();
vmath::mat4 projection;
vmath::vec3 axis_in_camera_coord;
vmath::vec3 light_axis_in_camera_coord;

void idle()
{
	glutPostRedisplay();
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
	if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
	{
		ctrl = true;
	}
	switch (key)
	{
		case GLUT_KEY_F6:	glsl_program_obj.BuildFiles("Shaders/simple_vs_2.vert", "Shaders/simple_fs_2.frag", NULL, NULL, NULL, &std::cout);
			break;
	}
}

void mouseButtonPress(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (ctrl)
		{
			ctrl_flag = true;
		}
		else
		{
			rotate = true;
			prevMouseX = curMouseX = x;
			prevMouseY = curMouseY = y;
		}
		
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
		ctrl = false;
		ctrl_flag = false;
	}
}

void mouseMove(int x, int y)
{
	if (rotate || zoom || ctrl_flag)
	{
		curMouseX = x;
		curMouseY = y;
	}
}

void initialize()
{
	projection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	rotationMatrix *= vmath::rotate(vmath::degrees(rotation_angle), axis_in_camera_coord);
	
	glClearColor(0.0, 0.0, 0.0, 1.0); //black background
	trimesh_obj.LoadFromFileObj("Objects/teapot.txt", true);
	trimesh_obj.ComputeNormals();

	for (int i = 0; i < trimesh_obj.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int index = trimesh_obj.F(i).v[j];
			indices.push_back(index);
		}
	}

	//now we generate and bind the vertex array object
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	//next, we generate a vertex buffer and set the vertices as its data
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)*trimesh_obj.NV(), &trimesh_obj.V(0), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)*trimesh_obj.NVN(), &trimesh_obj.VN(0), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void render()
{
	// MATRIX CALCULATIONS:
	rotation_angle = 0.0f;
	axis_in_camera_coord = vmath::vec3(0.0f, 1.0f, 0.0f);

	light_rotation_angle = 0.0f;
	light_axis_in_camera_coord = vmath::vec3(0.0f, 1.0f, 0.0f);

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
		if (ctrl_flag)
		{
			vmath::vec3 prev = getArcballVector(prevMouseX, prevMouseY);
			vmath::vec3 curr = getArcballVector(curMouseX, curMouseY);
			light_rotation_angle = acos(fmin(1.0f, vmath::dot(prev, curr)));
			light_axis_in_camera_coord = vmath::cross(prev, curr);
			light_axis_in_camera_coord = vmath::normalize(light_axis_in_camera_coord);
			prevMouseX = curMouseX;
			prevMouseY = curMouseY;
			lightRotationMatrix *= vmath::rotate(vmath::degrees(light_rotation_angle), light_axis_in_camera_coord);
		}
	}

	//cy::Point3<float> pos(0.0f, 0.0f, -1.0f);
	//cy::Point3<float> target(0.0f, 0.0f, 0.0f);
	//cy::Point3<float> up(0.0f, 1.0f, 0.0f);
	//cy::Point3<float> trans(0.0f, 50.0f, 100.0f);
	//cyMatrix4f persp = matrix4_obj.MatrixPerspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	//cyMatrix4f view = matrix4_obj.MatrixView(pos, target, up);
	//view *= matrix4_obj.MatrixTrans(trans);
	////view *= matrix4_obj.MatrixRotationX(100.0f);
	////view *= matrix4_obj.MatrixRotationY(50.0f);
	////view *= matrix4_obj.MatrixRotationZ(100.0f);
	//cyMatrix4f model = matrix4_obj.MatrixScale(0.05f);
	//cyMatrix4f mvp_matrix = persp * view * model;

	//glsl_program_obj.RegisterUniform(0, "mvp_matrix", &std::cout);
	//glsl_program_obj.SetUniformMatrix4(0, &mvp_matrix[0], GL_FALSE);

	vmath::mat4 perspective_matrix = projection;
	vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(x_pos, y_pos, z_pos), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	view_matrix *= translationMatrix;
	view_matrix *= rotationMatrix;
	vmath::mat4 model_matrix = vmath::scale(scale_factor);

	vmath::mat4 mvp = perspective_matrix * view_matrix * model_matrix;
	vmath::mat4 mv = view_matrix * model_matrix;

	vmath::mat4 light_view_matrix = vmath::lookat(vmath::vec3(70.0, 50.0, 0.0), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	//light_view_matrix *= translationMatrix;
	light_view_matrix *= lightRotationMatrix;
	vmath::mat4 light_model_matrix = vmath::scale(scale_factor);

	vmath::mat4 mv_light = light_view_matrix * light_model_matrix;

	cy::Point3<float> light_pos(100.0, 0.0, 50.0);
	//light_pos *= matrix3_obj.MatrixRotationX(100.0f);

	//Sending the mvp matrix to the vertex shader:
	mvpMatrixID = glGetUniformLocation(program1, "mvp_matrix");
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	mvMatrixID = glGetUniformLocation(program1, "mv_matrix");
	glUniformMatrix4fv(mvMatrixID, 1, GL_FALSE, &mv[0][0]);

	lightMatrixID = glGetUniformLocation(program1, "light_matrix");
	glUniformMatrix4fv(lightMatrixID, 1, GL_FALSE, &mv_light[0][0]);

	lightPosID = glGetUniformLocation(program1, "light_pos");
	glUniform3f(lightPosID, light_pos[0], light_pos[1], light_pos[2]);

	//-------------------------------------------------------------------------------------------------------------------------------------------
	
	glEnable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
	
	glEnable(GL_DEPTH_TEST);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao1);
	glsl_program_obj.Bind();

	glBindBuffer(GL_ARRAY_BUFFER, buffer);	
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv); //to initialize GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(window_height, window_width);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CS6610 Project 3 - Shading");
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
