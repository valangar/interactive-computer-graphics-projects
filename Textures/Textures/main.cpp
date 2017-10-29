/*
CS6610 Project 4 - Textures
AUTHOR: Varsha Alangar
Created on: 02/9/2017
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <cyCodeBase/cyMatrix.h>
#include <cyCodeBase/cyGL.h>
#include <cyCodeBase/cyTriMesh.h>
#include <imageLoader/lodepng.h>
#include <iostream>
#include <cmath>
#include <GL/vmath.h>
#include <ctype.h>
#include <string.h>
#include <vector>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

std::vector<cy::Point3f> v_indices;
std::vector<cy::Point3f> n_indices;
std::vector<cy::Point3f> t_indices;
std::vector<float> diffuse_Kd;
std::vector<float> specular_Ks;
std::vector<unsigned char> image;
std::vector<unsigned char> image1;

unsigned img_width = 512;
unsigned img_height = 512;

int window_width = 600;
int window_height = 600;
int prevMouseX = 0;
int prevMouseY = 0;
int curMouseX = 0;
int curMouseY = 0;

float x_pos = 0.0f;
float y_pos = -90.0f;
float z_pos = 100.0f;
float scale_factor = 30.0f;
float rotation_angle;
float light_rotation_angle;

bool zoom = false;
bool rotate = false;
bool flag = false;
bool ctrl = false;
bool ctrl_flag = false;

GLuint vao1;
GLuint tex_obj1;
GLuint tex_obj2;
GLuint v_buffer;
GLuint n_buffer;
GLuint t_buffer;
GLuint vs_id;
GLuint fs_id;
GLuint program1;
GLuint locationID;

cy::TriMesh trimesh_obj;
cy::GLSLShader glsl_shader_obj;
cy::GLSLProgram glsl_program_obj;
cy::Matrix4<float> matrix4_obj;
cy::Matrix3<float> matrix3_obj;

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

void load_img(char *filename)
{
	std::cout << filename;
	std::string file(filename);

	unsigned error = lodepng::decode(image, img_width, img_height, "Objects/Teapot/" + file);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

}

void initialize()
{
	projection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	rotationMatrix *= vmath::rotate(vmath::degrees(rotation_angle), axis_in_camera_coord);

	glClearColor(0.0, 0.0, 0.0, 1.0); //black background
	trimesh_obj.LoadFromFileObj("Objects/Teapot/teapot.obj", true, &std::cout);
	trimesh_obj.ComputeNormals();

	for (int i = 0; i < trimesh_obj.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int index = trimesh_obj.F(i).v[j];
			cy::Point3f vertex = trimesh_obj.V(index);
			v_indices.push_back(vertex);
		}
	}
	for (int i = 0; i < trimesh_obj.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int index = trimesh_obj.FN(i).v[j];
			cy::Point3f vertex = trimesh_obj.VN(index);
			n_indices.push_back(vertex);
		}
	}
	for (int i = 0; i < trimesh_obj.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int index = trimesh_obj.FT(i).v[j];
			cy::Point3f vertex = trimesh_obj.VT(index);
			t_indices.push_back(vertex);
		}
	}
	for (int j = 0; j < 3; j++)
	{
		float Kd = trimesh_obj.M(0).Kd[j];
		float Ks = trimesh_obj.M(0).Ks[j];
		diffuse_Kd.push_back(Kd);
		specular_Ks.push_back(Ks);
	}
	std::cout << diffuse_Kd[0];
	std::cout << "nvn()" << trimesh_obj.NVN() << "NVT()" << trimesh_obj.NVT();
	//now we generate and bind the vertex array object
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	//next, we generate a vertex buffer and set the vertices as its data
	glGenBuffers(1, &v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)*v_indices.size(), &v_indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &n_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)* n_indices.size(), &n_indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &t_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, t_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cyPoint3f)* t_indices.size(), &t_indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//load_img(trimesh_obj.M(0).map_Kd, diffuse_image);
	std::string file1(trimesh_obj.M(0).map_Kd);
	unsigned error = lodepng::decode(image, img_width, img_height, "Objects/Teapot/" + file1);
	glGenTextures(1, &tex_obj1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//load_img(trimesh_obj.M(0).map_Ks, specular_image);
	std::string file2(trimesh_obj.M(0).map_Ks);
	error = lodepng::decode(image1, img_width, img_height, "Objects/Teapot/" + file2);
	glGenTextures(1, &tex_obj2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_obj2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

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
	locationID = glGetUniformLocation(program1, "mvp_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mvp[0][0]);

	locationID = glGetUniformLocation(program1, "mv_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv[0][0]);

	locationID = glGetUniformLocation(program1, "light_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv_light[0][0]);

	locationID = glGetUniformLocation(program1, "light_pos");
	glUniform3f(locationID, light_pos[0], light_pos[1], light_pos[2]);

	locationID = glGetUniformLocation(program1, "Kd");
	glUniform3f(locationID, diffuse_Kd[0], diffuse_Kd[1], diffuse_Kd[2]);

	locationID = glGetUniformLocation(program1, "Ks");
	glUniform3f(locationID, specular_Ks[0], specular_Ks[1], specular_Ks[2]);

	locationID = glGetUniformLocation(program1, "tex_d");
	glUniform1i(locationID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj1);
	
	locationID = glGetUniformLocation(program1, "tex_s");
	glUniform1i(locationID, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_obj2);
	

	//-------------------------------------------------------------------------------------------------------------------------------------------

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(vao1);
	glsl_program_obj.Bind();

	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, t_buffer);

	glDrawArrays(GL_TRIANGLES, 0, v_indices.size());

	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv); //to initialize GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(window_height, window_width);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CS6610 Project 4 - Textures");
	glewInit();
	glutDisplayFunc(render);
	glutKeyboardFunc(keys);
	glutSpecialFunc(special_keys);
	glutMouseFunc(mouseButtonPress);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);
	initialize();
	setShaders();
	setPlaneShader();
	glutMainLoop();
	return 0;
}
