/*
CS6610 Project 1 - Hello World
AUTHOR: Varsha Alangar
Created on: 01/21/2017
Requirements: freeglut, opengl32, gl32 libraries; appropriate dlls are available in the Debug folder;
Built on Visual Studio
*/

#include <GL/freeglut.h>
#include <GL/GL.h>
#include <iostream>
#include <cmath>

struct color
{
	float r, g, b, a;
};

void idle()
{
	glutPostRedisplay(); //CS6610 Requirement
}

void keys(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27: glutLeaveMainLoop(); //Requirement5: press esc to exit
				 break;
	}
}

color interpolate(color start, color finish, float time)
{
	color rgba;
	rgba.r = (1 - time)*start.r + time*finish.r;
	rgba.g = (1 - time)*start.g + time*finish.g;
	rgba.b = (1 - time)*start.b + time*finish.b;
	rgba.a = start.a * finish.a;
	return rgba;
}

void render()
{
	//interpolating between white and steelblue colors.
	color begin = { 1.0, 1.0, 1.0, 1.0 }; //White
	color end = { 0.137255, 0.419608, 0.556863 , 1.0}; //Steelblue

	//using trigonometric functions to determine the new color using interpolation
	const int current_t = glutGet(GLUT_ELAPSED_TIME);
	const float t = std::cos(float(current_t)*0.001)*0.5 + 0.5; //formula: float t = cos(x)*0.5+0.5 to scale it between 0 and 1.
	color current_c = interpolate(begin, end, t);

	glClearColor(current_c.r, current_c.g, current_c.b, current_c.a); //Setting the new color
	glClear(GL_COLOR_BUFFER_BIT); // Clearing the buffer with the preset color
	glutSwapBuffers();

}
void initialize()
{
	glClearColor(0.209f, 0.224f, 0.224f, 1.0f); //Requirement4: Setting background color of the window initially
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv); //to initialize GLUT
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(600, 600); //Requirement3: Specifying the size of the window
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CS6610 Project 1 - Hello World");
	glutDisplayFunc(render);
	glutKeyboardFunc(keys);
	glutIdleFunc(idle);
	//initialize();
	glutMainLoop();
	return 0;
}