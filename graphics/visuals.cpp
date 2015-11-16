#include <iostream>
#include <thread>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "visuals.h"
#include "shader.hpp"


namespace ttl {
	std::string file2str(const std::string &filename);
}

void lex(const std::string source,
	std::vector<std::pair<float, float>> &vector);

GLuint cubebuffer, cubebuffercolor, circlebuffer, circlebuffer_color, arrowbuffer, arrowbuffer_color;
GLuint programID_1;
long activeIndex = 0;
std::vector<std::pair<float, float>> white, red;
int last_time, current_time;
GLuint MatrixID; // Handler Matrix for moving the cam
glm::mat4 MVP, Projection, View, oldView, Model;

int mouse_x = 800/2;
int mouse_y = 600/2;
int mouse_button =  GLUT_LEFT_BUTTON;
int mouse_state = GLUT_UP;
int sp_key = 0;

float counter = 0;
glm::vec3 position = glm::vec3(5.63757, 1.7351, -2.19067 );
float horizontalAngle = -1.07947;
float verticalAngle = -0.369055;
float initialFoV = 45.0f;
glm::vec3 direction = glm::vec3(0,0,0) - position;
glm::vec3 right = glm::vec3(0,0,1) ;
glm::vec3 up = glm::vec3(0,1,0);

void MouseGL(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		mouse_state = state;
	else
		mouse_state = GLUT_UP;
}

void Mouse_active(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void Idle()
{
	current_time = glutGet(GLUT_ELAPSED_TIME);
	int dt = current_time - last_time;
	last_time = current_time;
}

void ReshapeGL( int w, int h )
{
	std::cout << "ReshapGL(" << w << ", " << h << ");" << std::endl;
	g_iWindowWidth = w;
	g_iWindowHeight = h;
	glViewport(0, 0, 800, 600);
}

void Specialkey(int c, int x, int y)
{
	sp_key = c;
}

void KeyboardGL(unsigned char c, int x, int y)
{
	unsigned char currentScene = g_eCurrentScene;
	std::cout << c << std::endl;

	std::pair<float, float> *active;
	if (activeIndex < white.size()) {
		active = &white.at(activeIndex);
	} else {
		active = &red.at(activeIndex - white.size());
	}

	switch (c)
	{
		case 'h': active->first -= 1; break;
		case 'j': active->second += 1; break;
		case 'k': active->second -= 1; break;
		case 'l': active->first += 1; break;
		case '1':
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );  // Black background
			g_eCurrentScene = 1;
		break;
		case '2':
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );  // Black background
			g_eCurrentScene = 2;
		break;
		case '\t':
			++activeIndex;
			if (activeIndex >= white.size() + red.size())
				activeIndex = 0;
		break;
		case 's':
		case 'S':
			std::cout << "Shade Model: GL_SMOOTH" << std::endl;
			glShadeModel(GL_SMOOTH);
		break;
		case 'f':
		case 'F':
			std::cout << "Shade Model: GL_FLAT" << std::endl;
			glShadeModel( GL_FLAT );
		break;
		case '\033': case '\015': case 'Q': case 'q': Cleanup(0); break;
	}

	if (currentScene != g_eCurrentScene)
			std::cout << "Changed Render Scene: " << int( g_eCurrentScene ) << std::endl;

	glutPostRedisplay();
}


void Cleanup( int errorCode, bool bExit )
{
	if (g_iGLUTWindowHandle != 0)
	{
		glutDestroyWindow(g_iGLUTWindowHandle);
		g_iGLUTWindowHandle = 0;
	}

	if (bExit)
		std::exit(errorCode);
}


void DisplayGL()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(33));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (g_eCurrentScene)
	{
		case 1: RenderScene1(); break;
		case 2: RenderScene2(); break;
	}

	glutSwapBuffers();
}

void RenderScene2()
{
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix

	View = glm::lookAt(
		glm::vec3(8.f, -2.f, 20.f), // Camera is at (4,3,-3), in World Space
		glm::vec3(8.f, 8.f, 0.f), // and looks at the origin
		glm::vec3(0.f, 1.f, 0.f)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	oldView = View;
	for (int i = 0; i < 8; ++i) {
		View = oldView;
		View = glm::translate(View, glm::vec3(i * 2.f, 0.f, 0.f));
		for (int j = 0; j < 5; ++j) {
			Model = glm::rotate(Model, 90.f, glm::vec3(0.f, 1.f, 0.f));
			View = glm::translate(View, glm::vec3(0.f, 2.f, 0.f));
			MVP = Projection * View * Model;

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cubebuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cubebuffercolor);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangles
			glDrawArrays(GL_TRIANGLES, 0, 6*2*3); // 12*3 indices starting at 0 -> 12 triangles
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
	}

	Model = glm::scale(glm::mat4(1.f), glm::vec3(0.7f, 0.7f, 1.f));
	Model = glm::rotate(Model, 90.f, glm::vec3(0.f, 1.f, 0.f));
	Model = glm::rotate(Model, 90.f, glm::vec3(0.f, 0.f, 1.f));
	for (std::pair<float, float> out : red) {
		View = oldView;

		View = glm::translate(View, glm::vec3(0.f, 2.f, 2.f));
		View = glm::translate(View, glm::vec3(out.first * 2.f / 100 - 1.f,
			8.f - (out.second * 2.f / 100 - 1.f), 0.f));

		MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cubebuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cubebuffercolor);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, 6*2*3); // 12*3 indices starting at 0 -> 12 triangles
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}


	Model = glm::scale(glm::mat4(1.f), glm::vec3(0.7f, 0.7f, 1.f));
	for (std::pair<float, float> out : white) {
		View = oldView;

		View = glm::translate(View, glm::vec3(0.f, 2.f, 2.f));
		View = glm::translate(View, glm::vec3(out.first * 2.f / 100 - 1.f,
			8.f - (out.second * 2.f / 100 - 1.f), 0.f));

		MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, circlebuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, circlebuffer_color);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLE_FAN, 0, 42); // 12*3 indices starting at 0 -> 12 triangles
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	std::pair<float, float> active;
	if (activeIndex < white.size()) {
		active = white.at(activeIndex);
	} else {
		active = red.at(activeIndex - white.size());
	}

	{
		View = oldView;

		View = glm::translate(View, glm::vec3(0.f, 3.f, 4.f));
		View = glm::translate(View, glm::vec3(active.first * 2.f / 100 - 1.f,
			8.f - (active.second * 2.f / 100 - 1.f), 0.f));

		MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, arrowbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, arrowbuffer_color);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}


void RenderScene1()
{    // Disable depth test
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, 4.f), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	Model = glm::rotate(Model, static_cast<float>(current_time / 60),
		glm::vec3(0.f, 1.f, 0.f));
	Model = glm::rotate(Model, static_cast<float>(current_time / 20),
		glm::vec3(0.f, 0.f, 1.f));
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, cubebuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, cubebuffercolor);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 6*2*3); // 12*3 indices starting at 0 -> 12 triangles
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


void SetupGL()
{
	//Parameter handling
	glShadeModel(GL_SMOOTH);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//Set up light source
	GLfloat light_position[] = {0.0, 30.0, -50.0, 0.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black background

	// Register GLUT callbacks
	glutDisplayFunc(DisplayGL);
	glutKeyboardFunc(KeyboardGL);
	glutSpecialFunc(Specialkey);
	glutReshapeFunc(ReshapeGL);
	glutMouseFunc(MouseGL);
	glutMotionFunc(Mouse_active);

	//Call to the drawing function
	glutIdleFunc(Idle);
	last_time = glutGet(GLUT_ELAPSED_TIME);

	// Setup initial GL State
	glClearDepth(1.0f);

	//
	// Init GLEW

	// Setup initial GL State
	glClearDepth(1.0f);

	if ( glewInit() != GLEW_OK )
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		exit(-1);
	}
	if (!GLEW_VERSION_1_1)
	{
		std::cerr << "Failed to set the correct version" << std::endl;
		exit(1);
	}
	std::cout << "Initialise OpenGL: Success!" << std::endl;

	//VAO
	//        glGenVertexArrays(1, &VertexArrayID);
	//        glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	std::string vertex = ttl::file2str("TransformVertexShader.vs");
	std::string fragment = ttl::file2str("ColorFragmentShader.fs");

	programID_1 = LoadShaders(vertex.c_str(), fragment.c_str());

	static const GLfloat g_vertex_buffer_data_cube[] = {
		// Front face
		1.f, -1.f, 1.f,
		-1.f, 1.f, 1.f,
		-1.f, -1.f, 1.f,
		1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,
		-1.f, 1.f, 1.f,

		// Head
		1.f, 1.f, -1.f,
		-1.f, 1.f, 1.f,
		-1.f, 1.f, -1.f,
		1.f, 1.f, -1.f,
		1.f, 1.f, 1.f,
		-1.f, 1.f, 1.f,

		// Bottom
		1.f, -1.f, -1.f,
		-1.f, -1.f, 1.f,
		-1.f, -1.f, -1.f,
		1.f, -1.f, -1.f,
		1.f, -1.f, 1.f,
		-1.f, -1.f, 1.f,

		// Left
		-1.f, 1.f, 1.f,
		-1.f, 1.f, -1.f,
		-1.f, -1.f, -1.f,
		-1.f, -1.f, -1.f,
		-1.f, -1.f, 1.f,
		-1.f, 1.f, 1.f,
		// Right
		1.f, 1.f, 1.f,
		1.f, 1.f, -1.f,
		1.f, -1.f, -1.f,
		1.f, -1.f, -1.f,
		1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,

		// Back face
		-1.f, -1.f, -1.f,
		-1.f, 1.f, -1.f,
		1.f, -1.f, -1.f,
		-1.f, 1.f, -1.f,
		1.f, 1.f, -1.f,
		1.f, -1.f, -1.f,
	};

	static const GLfloat g_vertex_buffer_data_cube_color[] = {
		// Red side
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,

		// Blue side
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,

		// Blue side
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,

		// Blue side
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,

		// Blue side
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,

		// Back (red) face
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
	};

	static const GLfloat g_vertex_buffer_data_circle[] = {
		0.f, 0.f, 1.f,
		1.00000000000000f, 0.000000000000000f, 0.f,
		0.987688340595138f, 0.156434465040231f, 0.f,
		0.951056516295154f, 0.309016994374947f, 0.f,
		0.891006524188368f, 0.453990499739547f, 0.f,
		0.809016994374947f, 0.587785252292473f, 0.f,
		0.707106781186548f, 0.707106781186548f, 0.f,
		0.587785252292473f, 0.809016994374947f, 0.f,
		0.453990499739547f, 0.891006524188368f, 0.f,
		0.309016994374947f, 0.951056516295154f, 0.f,
		0.156434465040231f, 0.987688340595138f, 0.f,
		0.000000000000000f, 1.00000000000000f, 0.f,
		-0.156434465040231f, 0.987688340595138f, 0.f,
		-0.309016994374947f, 0.951056516295154f, 0.f,
		-0.453990499739547f, 0.891006524188368f, 0.f,
		-0.587785252292473f, 0.809016994374947f, 0.f,
		-0.707106781186548f, 0.707106781186548f, 0.f,
		-0.809016994374947f, 0.587785252292473f, 0.f,
		-0.891006524188368f, 0.453990499739547f, 0.f,
		-0.951056516295154f, 0.309016994374947f, 0.f,
		-0.987688340595138f, 0.156434465040231f, 0.f,
		-1.00000000000000f, 0.000000000000000f, 0.f,
		-0.987688340595138f, -0.156434465040231f, 0.f,
		-0.951056516295154f, -0.309016994374947f, 0.f,
		-0.891006524188368f, -0.453990499739546f, 0.f,
		-0.809016994374947f, -0.587785252292473f, 0.f,
		-0.707106781186548f, -0.707106781186548f, 0.f,
		-0.587785252292473f, -0.809016994374947f, 0.f,
		-0.453990499739547f, -0.891006524188368f, 0.f,
		-0.309016994374947f, -0.951056516295154f, 0.f,
		-0.156434465040231f, -0.987688340595138f, 0.f,
		0.000000000000000f, -1.00000000000000f, 0.f,
		0.156434465040231f, -0.987688340595138f, 0.f,
		0.309016994374947f, -0.951056516295154f, 0.f,
		0.453990499739547f, -0.891006524188368f, 0.f,
		0.587785252292473f, -0.809016994374947f, 0.f,
		0.707106781186548f, -0.707106781186548f, 0.f,
		0.809016994374947f, -0.587785252292473f, 0.f,
		0.891006524188368f, -0.453990499739547f, 0.f,
		0.951056516295154f, -0.309016994374947f, 0.f,
		0.987688340595138f, -0.156434465040231f, 0.f,
		1.f, 0.f, 0.f,
	};

	static const GLfloat g_vertex_buffer_data_circle_color[] = {
		0.f, 0.f, 1.f,
		1.00000000000000f, 0.000000000000000f, 0.f,
		0.987688340595138f, 0.156434465040231f, 0.f,
		0.951056516295154f, 0.309016994374947f, 0.f,
		0.891006524188368f, 0.453990499739547f, 0.f,
		0.809016994374947f, 0.587785252292473f, 0.f,
		0.707106781186548f, 0.707106781186548f, 0.f,
		0.587785252292473f, 0.809016994374947f, 0.f,
		0.453990499739547f, 0.891006524188368f, 0.f,
		0.309016994374947f, 0.951056516295154f, 0.f,
		0.156434465040231f, 0.987688340595138f, 0.f,
		0.000000000000000f, 1.00000000000000f, 0.f,
		-0.156434465040231f, 0.987688340595138f, 0.f,
		-0.309016994374947f, 0.951056516295154f, 0.f,
		-0.453990499739547f, 0.891006524188368f, 0.f,
		-0.587785252292473f, 0.809016994374947f, 0.f,
		-0.707106781186548f, 0.707106781186548f, 0.f,
		-0.809016994374947f, 0.587785252292473f, 0.f,
		-0.891006524188368f, 0.453990499739547f, 0.f,
		-0.951056516295154f, 0.309016994374947f, 0.f,
		-0.987688340595138f, 0.156434465040231f, 0.f,
		-1.00000000000000f, 0.000000000000000f, 0.f,
		-0.987688340595138f, -0.156434465040231f, 0.f,
		-0.951056516295154f, -0.309016994374947f, 0.f,
		-0.891006524188368f, -0.453990499739546f, 0.f,
		-0.809016994374947f, -0.587785252292473f, 0.f,
		-0.707106781186548f, -0.707106781186548f, 0.f,
		-0.587785252292473f, -0.809016994374947f, 0.f,
		-0.453990499739547f, -0.891006524188368f, 0.f,
		-0.309016994374947f, -0.951056516295154f, 0.f,
		-0.156434465040231f, -0.987688340595138f, 0.f,
		0.000000000000000f, -1.00000000000000f, 0.f,
		0.156434465040231f, -0.987688340595138f, 0.f,
		0.309016994374947f, -0.951056516295154f, 0.f,
		0.453990499739547f, -0.891006524188368f, 0.f,
		0.587785252292473f, -0.809016994374947f, 0.f,
		0.707106781186548f, -0.707106781186548f, 0.f,
		0.809016994374947f, -0.587785252292473f, 0.f,
		0.891006524188368f, -0.453990499739547f, 0.f,
		0.951056516295154f, -0.309016994374947f, 0.f,
		0.987688340595138f, -0.156434465040231f, 0.f,
		1.f, 0.f, 0.f,
	};

	static const GLfloat g_vertex_buffer_data_arrow[] = {
		0.f, -1.f, 0.f,
		-0.5f, 1.f, 0.f,
		0.5f, 1.f, 0.f,
	};

	static const GLfloat g_vertex_buffer_data_arrow_color[] = {
		1.f, 0.f, 1.f,
		1.f, 0.f, 1.f,
		1.f, 0.f, 1.f,
	};

	// Set up the buffer data for the cube
	glGenBuffers(1, &cubebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubebuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_cube),
		g_vertex_buffer_data_cube, GL_STATIC_DRAW);

	glGenBuffers(1, &cubebuffercolor);
	glBindBuffer(GL_ARRAY_BUFFER, cubebuffercolor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_cube_color),
		g_vertex_buffer_data_cube_color, GL_STATIC_DRAW);

	glGenBuffers(1, &circlebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, circlebuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_circle),
		g_vertex_buffer_data_circle, GL_STATIC_DRAW);

	glGenBuffers(1, &circlebuffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, circlebuffer_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_circle_color),
		g_vertex_buffer_data_circle_color, GL_STATIC_DRAW);

	glGenBuffers(1, &arrowbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, arrowbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_arrow),
		g_vertex_buffer_data_arrow, GL_STATIC_DRAW);

	glGenBuffers(1, &arrowbuffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, arrowbuffer_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_arrow_color),
		g_vertex_buffer_data_arrow_color, GL_STATIC_DRAW);

	// May as well just add the vertex data.
	lex(ttl::file2str("../processing/checker11"), red);
	lex(ttl::file2str("../processing/checker12"), white);
}
