/*
CS6610 FINAL PROJECT - NON-PHOTOREALISTIC RENDERING
AUTHOR: Varsha Alangar
Created on: 03/29/2017
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
std::vector<unsigned char> image1;
std::vector<unsigned char> image2;
std::vector<unsigned char> image3;
std::vector<unsigned char> image4;
std::vector<unsigned char> image5;
std::vector<unsigned char> image6;

unsigned img_width = 256;
unsigned img_height = 256;

int window_width = 600;
int window_height = 600;
int prevMouseX = 0;
int prevMouseY = 0;
int curMouseX = 0;
int curMouseY = 0;
int gooch_shading = 0;

float x_pos = 0.0f;
float y_pos = 10.0f;
float z_pos = 100.0f;
float scale_factor = 15.0f;
float rotation_angle;
float light_rotation_angle;
float plane_x_pos = 0.0f;
float plane_y_pos = 10.0f;
float plane_z_pos = 100.0f;
float plane_scale_factor = 15.0f;

bool zoom = false;
bool rotate = false;
bool flag = false;
bool ctrl = false;
bool ctrl_flag = false;
bool toon_shading = true;
bool stanford_bunny = false;
bool stanford_dragon = false;

GLuint tex_obj_1;
GLuint tex_obj_2;
GLuint tex_obj_3;
GLuint tex_obj_4;
GLuint tex_obj_5;
GLuint tex_obj_6;
GLuint vao;
GLuint plane_vao;
GLuint quad_buffer;
GLuint v_buffer;
GLuint n_buffer;
GLuint t_buffer;
GLuint vs_id;
GLuint fs_id;
GLuint program1;
GLuint program2;
GLuint silhouette_program;
GLuint phong_program;
GLuint locationID;

cy::TriMesh trimesh_obj;
cy::GLSLShader glsl_shader_obj;
cy::GLSLProgram glsl_program_obj;
cy::GLSLProgram glsl_silhouette_program_obj;
cy::GLSLProgram glsl_plane_program_obj;
cy::GLSLProgram glsl_phong_program_obj;
cy::GLRenderBuffer glrb_obj;
cy::GLTexture<GL_TEXTURE_2D> texture_obj;

vmath::mat4 rotationMatrix = vmath::mat4::identity();
vmath::mat4 lightRotationMatrix = vmath::mat4::identity();
vmath::mat4 translationMatrix = vmath::mat4::identity();
vmath::mat4 projection;
vmath::mat4 planeProjection;

vmath::vec3 getArcballVector(int x, int y);
vmath::vec3 axis_in_camera_coord;
vmath::vec3 light_axis_in_camera_coord;
vmath::mat4 mvp;
vmath::mat4 mv;
vmath::mat4 mv_light;
cy::Point3<float> light_pos(100.0, 0.0, 50.0);

void idle()
{
	glutPostRedisplay();
}

void setShaders()
{
	glsl_program_obj.BuildFiles("Shaders/object_vs.vert", "Shaders/object_fs.frag", NULL, NULL, NULL, &std::cout);
	program1 = glsl_program_obj.GetID();
}

void setPlaneShaders()
{
	glsl_plane_program_obj.BuildFiles("Shaders/plane_vs.vert", "Shaders/plane_fs.frag", NULL, NULL, NULL, &std::cout);
	program2 = glsl_plane_program_obj.GetID();
}

void setSilhouetteShaders()
{
	glsl_silhouette_program_obj.BuildFiles("Shaders/silhouette_vs.vert", "Shaders/silhouette_fs.frag", NULL, NULL, NULL, &std::cout);
	silhouette_program = glsl_silhouette_program_obj.GetID();
}

void setBlinnPhongShaders()
{
	glsl_phong_program_obj.BuildFiles("Shaders/phong_vs.vert", "Shaders/phong_fs.frag", NULL, NULL, NULL, &std::cout);
	phong_program = glsl_phong_program_obj.GetID();
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
		case 'H':
		case 'h':	if (toon_shading)
					{
						toon_shading = false;
					}
					else
					{
						toon_shading = true;
					}
					break;
		case 't':
		case 'T':	if (gooch_shading == 0)
					{
						gooch_shading = 1;
					}
					else
					{
						gooch_shading = 0;
					}
					break;
		case 27:	glutLeaveMainLoop();
					break;
	}

}

void special_keys(int key, int x, int y)
{
	if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
	{
		ctrl = true;
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

void createRenderTarget()
{
	//CREATING THE RENDER TARGET:
	GLsizei rb_width = 600;
	GLsizei rb_height = 600;
	cyGL::Type type = cyGL::TYPE_UBYTE;

	bool check_init = glrb_obj.Initialize(true, 3, window_width, window_height, type);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not okay!";

	glrb_obj.BuildTextureMipmaps();
	glrb_obj.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR); //BILINEAR FILTERING FOR  MAGNIFICATION
	glrb_obj.SetTextureMaxAnisotropy();
}

void createQuad()
{
	//CREATING QUAD TO USE THE RENDERED TEXTURE:
	static const GLfloat quad_vertex[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};
	glGenVertexArrays(1, &plane_vao);
	glBindVertexArray(plane_vao);

	glGenBuffers(1, &quad_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex), quad_vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

void initializeTeapot()
{
	projection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
	rotationMatrix *= vmath::rotate(vmath::degrees(rotation_angle), axis_in_camera_coord);

	planeProjection = vmath::perspective(20.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);

	glClearColor(0.761, 0.769, 0.78, 1.0); //black background

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
	//now we generate and bind the vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

	std::string file1("hatch_0.png");
	unsigned error = lodepng::decode(image1, img_width, img_height, "Objects/HatchingTextures/" + file1);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	glGenTextures(1, &tex_obj_1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj_1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::string file2("hatch_1.png");
	error = lodepng::decode(image2, img_width, img_height, "Objects/HatchingTextures/" + file2);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	glGenTextures(1, &tex_obj_2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_obj_2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	std::string file3("hatch_2.png");
	error = lodepng::decode(image3, img_width, img_height, "Objects/HatchingTextures/" + file3);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	glGenTextures(1, &tex_obj_3);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_obj_3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image3[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::string file4("hatch_3.png");
	error = lodepng::decode(image4, img_width, img_height, "Objects/HatchingTextures/" + file4);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	glGenTextures(1, &tex_obj_4);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_obj_4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image4[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::string file5("hatch_4.png");
	error = lodepng::decode(image5, img_width, img_height, "Objects/HatchingTextures/" + file5);
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}
	glGenTextures(1, &tex_obj_5);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tex_obj_5);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image5[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindVertexArray(0);
	
	createRenderTarget();
	createQuad();
}

void matrixCalculations()
{
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

	vmath::mat4 perspective_matrix = projection;
	vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(x_pos, y_pos, z_pos), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	view_matrix *= translationMatrix;
	view_matrix *= rotationMatrix;
	vmath::mat4 model_matrix = vmath::scale(scale_factor);
	
	mvp = perspective_matrix * view_matrix * model_matrix;
	mv = view_matrix * model_matrix;

	vmath::mat4 light_view_matrix = vmath::lookat(vmath::vec3(70.0, 50.0, 0.0), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	//light_view_matrix *= translationMatrix;
	light_view_matrix *= lightRotationMatrix;
	vmath::mat4 light_model_matrix = vmath::scale(scale_factor);

	mv_light = light_view_matrix * light_model_matrix;
}

void toonShading()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color

	glEnable(GL_TEXTURE_2D);
	
	vmath::mat4 plane_perspective_matrix = planeProjection;
	vmath::mat4 plane_view_matrix = vmath::lookat(vmath::vec3(plane_x_pos, plane_y_pos, plane_z_pos), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f)); //setting up camera position and viewing direction
	vmath::mat4 plane_model_matrix = vmath::scale(plane_scale_factor);

	vmath::mat4 plane_mvp = plane_perspective_matrix * plane_view_matrix * plane_model_matrix;
	vmath::mat4 plane_mv = plane_view_matrix * plane_model_matrix;

	//---------TEAPOT SILHOUETTE---------------------
	glrb_obj.Bind(); //Binding framebuffer to render into it.

	glDisable(GL_DEPTH_TEST);

	setSilhouetteShaders();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color

	glsl_silhouette_program_obj.Bind();


	locationID = glGetUniformLocation(silhouette_program, "mvp_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mvp[0][0]);
	locationID = glGetUniformLocation(silhouette_program, "silhouette_color");
	glUniform3f(locationID, 0.0f, 0.0f, 0.0f);
	locationID = glGetUniformLocation(silhouette_program, "normal_offset");
	glUniform1f(locationID, 0.01f);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, n_buffer);

	glDrawArrays(GL_TRIANGLES, 0, v_indices.size());

	//------------------------------------------
	locationID = glGetUniformLocation(silhouette_program, "silhouette_color");
	glUniform3f(locationID, 1.0f, 1.0f, 1.0f);
	locationID = glGetUniformLocation(silhouette_program, "normal_offset");
	glUniform1f(locationID, 0.0f);

	glDrawArrays(GL_TRIANGLES, 0, v_indices.size());

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	
	//-----------TEAPOT-------------------------

	setShaders();

	glEnable(GL_DEPTH_TEST);

	glsl_program_obj.Bind();

	//Sending the mvp matrix to the vertex shader:
	locationID = glGetUniformLocation(program1, "mvp_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mvp[0][0]);

	locationID = glGetUniformLocation(program1, "mv_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv[0][0]);

	locationID = glGetUniformLocation(program1, "light_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv_light[0][0]);

	locationID = glGetUniformLocation(program1, "light_pos");
	glUniform3f(locationID, light_pos[0], light_pos[1], light_pos[2]);

	locationID = glGetUniformLocation(program1, "gooch");
	glUniform1i(locationID, gooch_shading);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, n_buffer);

	glDrawArrays(GL_TRIANGLES, 0, v_indices.size());

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);

	//----------PLANE--------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setPlaneShaders();
	glsl_plane_program_obj.Bind();

	GLuint planeTexID = glGetUniformLocation(program2, "renderTex");
	glUniform1i(planeTexID, 0);
	glUniform1i(glGetUniformLocation(program2, "search_radius"), 2);
	glUniform1i(glGetUniformLocation(program2, "width"), 512);
	glrb_obj.BindTexture(0);
	
	locationID = glGetUniformLocation(program2, "plane_mvp_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &plane_mvp[0][0]);

	glBindVertexArray(plane_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

void hatching()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
	setBlinnPhongShaders();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glsl_phong_program_obj.Bind();

	//Sending the mvp matrix to the vertex shader:
	locationID = glGetUniformLocation(phong_program, "mvp_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mvp[0][0]);

	locationID = glGetUniformLocation(phong_program, "mv_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv[0][0]);

	locationID = glGetUniformLocation(phong_program, "light_matrix");
	glUniformMatrix4fv(locationID, 1, GL_FALSE, &mv_light[0][0]);

	locationID = glGetUniformLocation(phong_program, "light_pos");
	glUniform3f(locationID, light_pos[0], light_pos[1], light_pos[2]);

	
	locationID = glGetUniformLocation(phong_program, "tex_obj_1");
	glUniform1i(locationID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_obj_1);

	locationID = glGetUniformLocation(phong_program, "tex_obj_2");
	glUniform1i(locationID, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_obj_2);

	locationID = glGetUniformLocation(phong_program, "tex_obj_3");
	glUniform1i(locationID, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_obj_3);

	locationID = glGetUniformLocation(phong_program, "tex_obj_4");
	glUniform1i(locationID, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_obj_4);

	locationID = glGetUniformLocation(phong_program, "tex_obj_5");
	glUniform1i(locationID, 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, tex_obj_5);
		
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, t_buffer);
	
	glDrawArrays(GL_TRIANGLES, 0, v_indices.size());

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	glutSwapBuffers();

}

void render()
{
	matrixCalculations();
	if (toon_shading)
	{
		toonShading();
	}
	else
	{
		hatching();
	}
	
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv); //to initialize GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(window_height, window_width);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CS6610 Final Project");
	glewInit();
	glutDisplayFunc(render);
	glutKeyboardFunc(keys);
	glutSpecialFunc(special_keys);
	glutMouseFunc(mouseButtonPress);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);
	initializeTeapot();
	glutMainLoop();
	return 0;
}
