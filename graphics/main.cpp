#include "visuals.h"
#include "glm/glm.hpp"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
//--------- Global Variables
int g_iWindowWidth = 512;
int g_iWindowHeight = 512;
int g_iGLUTWindowHandle = 0;
int g_iErrorCode = 0;
float rot_angle;
unsigned char g_eCurrentScene = 1;

//---------- Main program
int main(int argc, char *argv[])
{
	glewExperimental = GL_TRUE;
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA |
		GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);

	g_iGLUTWindowHandle = glutCreateWindow("OpenGL");

	SetupGL();

	glutMainLoop();
	return 0;

}
//-----------------------------------------------------------


