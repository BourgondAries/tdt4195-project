#include <iostream>
#include <thread>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "visuals.h"
#include "shader.hpp"

void RenderScene6();

namespace ttl {
	std::string file2str(const std::string &filename);
}

#define PI 3.14159265359

#define mouseSpeed 0.00005f

#define cursor_speed 0.0018f
// units per second


// This will be used with shader
//GLuint VertexArrayID;
GLuint vertexbuffer, colorbuffer, legbuffer, legcolorbuffer, hipcolorbuffer;
GLuint objvertexbuffer; // for obj
GLuint programID_1, programID_2;

int last_time, current_time;

GLuint MatrixID; // Handler Matrix for moving the cam
glm::mat4 MVP; // FInal Homogeneous Matrix

glm::mat4 MVP1,MVP2,MVP3,MVP4,MVP5, MODEL_EVERYTHING, MODEL_LEG_1, MODEL_LEG_2;
glm::mat4 MVPi[10];
glm::mat4 Projection,View,Model;

// Variables for moving camera with mouse
int mouse_x = 800/2;
int mouse_y = 600/2;
int mouse_button =  GLUT_LEFT_BUTTON;
int mouse_state = GLUT_UP;
int sp_key = 0;

float counter = 0;
// Initial position : on +Z
glm::vec3 position = glm::vec3(5.63757, 1.7351, -2.19067 );
// Initial horizontal angle : toward -Z
float horizontalAngle = -1.07947;
// Initial vertical angle : none
float verticalAngle = -0.369055;
// Initial Field of View
float initialFoV = 45.0f;
glm::vec3 direction = glm::vec3(0,0,0) - position;
glm::vec3 right = glm::vec3(0,0,1) ;
glm::vec3 up = glm::vec3(0,1,0);

void MouseGL(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) // if key is pressed
	{
		mouse_state=state;
		// glutWarpPointer(800/2, 600/2);
	}
	else
	{
		mouse_state=GLUT_UP;
	}
}

void Mouse_active(int x, int y)
{
	//save values in global memory
	mouse_x = x;
	mouse_y = y;
}

void Idle()
{
	current_time = glutGet(GLUT_ELAPSED_TIME);
	int dt = current_time - last_time;
	if (g_eCurrentScene >=3)
	{
		if(mouse_state ==  GLUT_DOWN)
		{
			horizontalAngle += mouseSpeed * float(800/2 - mouse_x); // 800 = window width
			verticalAngle += mouseSpeed * float(600/2 - mouse_y);// 600 = window height
			// Direction : Spherical coordinates to Cartesian coordinates conversion
			direction= glm::vec3(
				cos(verticalAngle) * sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle) * cos(horizontalAngle)
			);
			// Right vector
			right = glm::vec3(
				sin(horizontalAngle - PI/2.0f),
				0,
				cos(horizontalAngle - PI/2.0f)
			);
			// Up vector
			up = glm::cross( right, direction );
		}
		//            // Move forward
		if (sp_key == GLUT_KEY_UP)
		{
			position += direction * (dt * cursor_speed);
		}
		// Move backward
		if (sp_key == GLUT_KEY_DOWN)
		{
			position -= direction * (dt * cursor_speed);
		}
		// Strafe right
		if (sp_key == GLUT_KEY_RIGHT)
		{
			position += right * (dt * cursor_speed);
		}
		// Strafe left
		if (sp_key== GLUT_KEY_LEFT)
		{
			position -= right * (dt * cursor_speed);
		}
		sp_key =0;
		// Camera matrix
		if (g_eCurrentScene != 6)
		{
			View = glm::lookAt(
				position, // Camera is at position, in World Space
				position+direction, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
		}
		glutPostRedisplay();
		// std::cout << position.x << " " << position.y <<  " " << position.z << std::endl;
	}
	if (g_eCurrentScene == 5)
	{
		counter = counter + 0.002 * dt;
		MODEL_EVERYTHING = glm::translate(MODEL_EVERYTHING, glm::vec3(0, 0, 0.0013 * counter));
		MODEL_LEG_1 = glm::rotate(MODEL_LEG_1, float(cos(counter)), glm::vec3(1, 0, 0));
		MODEL_LEG_2 = glm::rotate(MODEL_LEG_2, float(-cos(counter)), glm::vec3(1, 0, 0));
	}
	if (g_eCurrentScene == 6)
	{
		counter += 0.002 * dt;
		constexpr const float CONSTANT_INC = 0.0013f;
		View = glm::rotate(View, static_cast<float>(cos(counter)), glm::vec3(1, 0, 0));
		View = glm::translate(View, glm::vec3(0, 0, -CONSTANT_INC * counter));
		MODEL_EVERYTHING = glm::translate(MODEL_EVERYTHING, glm::vec3(0, 0, CONSTANT_INC * counter));
		MODEL_LEG_1 = glm::rotate(MODEL_LEG_1, float(cos(counter)), glm::vec3(1, 0, 0));
		MODEL_LEG_2 = glm::rotate(MODEL_LEG_2, float(-cos(counter)), glm::vec3(1, 0, 0));
	}
	last_time = current_time; // update when the last timer;
}

void ReshapeGL( int w, int h )
{
	std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

	if (h == 0)                                        // Prevent a divide-by-zero error
	{
		h = 1;                                      // Making Height Equal One
	}
	g_iWindowWidth = w;
	g_iWindowHeight = h;
	glViewport( 0, 0, 800, 600 );

}

void Specialkey(int c, int x, int y)
{
	sp_key = c;
}

void KeyboardGL( unsigned char c, int x, int y )
{
	// Store the current scene so we can test if it has changed later.
	unsigned char currentScene = g_eCurrentScene;

	switch ( c )
	{
		case '1':
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );                         // Black background
			g_eCurrentScene = 1;
		}
		break;
		case '2':
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );                         // Black background
			g_eCurrentScene = 2;
		}
		break;
		case '3':
		{
			glClearColor( 0.27f, 0.27f, 0.27f, 1.0f );                      // Dark-Gray background
			g_eCurrentScene = 3;
		}
		break;
		case '4':
		{
			glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );                      // Light-Gray background
			g_eCurrentScene = 4;
		}
		break;
		case '5':
		{
			glClearColor( 0.7f, 0.7f, 0.7f, 1.0f );                      // Light-Gray background
			g_eCurrentScene = 5;
			// THIS MODEL WILL BE APPLIED TO THE WHOLE SCENE 5
			MODEL_EVERYTHING=glm::mat4(1.0f); // Identity
			// THIS MODEL WILL BE APPLIED TO LEG 1
			MODEL_LEG_1=glm::mat4(1.0f);
			// THIS MODEL WILL BE APPLIED TO LEG 2
			MODEL_LEG_2=glm::mat4(1.0f);
		}
		break;
		case '6':
		{
			glClearColor( 0.7f, 0.7f, 0.7f, 1.0f );                      // Light-Gray background
			g_eCurrentScene = 6;
			View = glm::lookAt(
				glm::vec3(5.63757, -0.7351, 0), // Camera is at (4,3,-3), in World Space
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
			// THIS MODEL WILL BE APPLIED TO THE WHOLE SCENE 5
			MODEL_EVERYTHING=glm::mat4(1.0f); // Identity
			// THIS MODEL WILL BE APPLIED TO LEG 1
			MODEL_LEG_1=glm::mat4(1.0f);
			// THIS MODEL WILL BE APPLIED TO LEG 2
			MODEL_LEG_2=glm::mat4(1.0f);
		}
		break;
		case 's':
		case 'S':
		{
			std::cout << "Shade Model: GL_SMOOTH" << std::endl;
			// Switch to smooth shading model
			glShadeModel( GL_SMOOTH );
		}
		break;
		case 'f':
		case 'F':
		{
			std::cout << "Shade Model: GL_FLAT" << std::endl;
			// Switch to flat shading model
			glShadeModel( GL_FLAT );
		}
		break;
		case '\033': // escape quits
		case '\015': // Enter quits
		case 'Q':    // Q quits
		case 'q':    // q (or escape) quits
		{
			// Cleanup up and quit
			Cleanup(0);
		}
		break;

		case '\72': //arrow up
		{

		}
		break;
	}


	if ( currentScene != g_eCurrentScene )
	{
			std::cout << "Changed Render Scene: " << int( g_eCurrentScene ) << std::endl;
	}

	glutPostRedisplay();
}


void Cleanup( int errorCode, bool bExit )
{
	if ( g_iGLUTWindowHandle != 0 )
	{
		glutDestroyWindow( g_iGLUTWindowHandle );
		g_iGLUTWindowHandle = 0;
	}

	if ( bExit )
	{
		exit( errorCode );
	}
}


void DisplayGL()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(33));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
	// and the depth buffer

	switch ( g_eCurrentScene )
	{
		case 1: RenderScene1(); break;
		case 2: RenderScene2(); break;
		case 3: RenderScene3(); break;
		case 4: RenderScene4(); break;
		case 5: RenderScene5(); break;
		case 6: RenderScene6(); break;
	}

	glutSwapBuffers();
	// All drawing commands applied to the
	// hidden buffer, so now, bring forward
	// the hidden buffer and hide the visible one

}

void RenderScene1()
{    // Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	View = glm::lookAt(
		glm::vec3(5.63757, 1.7351, -2.19067), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderScene2()
{
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	View = glm::lookAt(
		glm::vec3(5.63757, 1.7351, -2.19067), // Camera is at (4,3,-3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// Use our shader
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderScene3()
{
	// Disable Back Face Culling
	glDisable(GL_CULL_FACE);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 1000.0f);
	// Model matrix : an identity matrix (model will be at the origin)
	Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
void RenderScene4()
{
	// Enable bac-face culling
	glEnable(GL_CULL_FACE);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 1000.0f);
	// Model matrix : an identity matrix (model will be at the origin)
	Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Use our shader
	glUseProgram(programID_1);
	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderScene5()
{
	// THIS MODEL IS APPLIED TO THE WHOLE SCENE 5
	//    MODEL_EVERYTHING = IT is initalized as identity matrix

	// THIS MODEL WILL BE APPLIED TO LEG 1
	//    MODEL_LEG_1= IT is initalized as identity matrix

	// THIS MODEL WILL BE APPLIED TO LEG 2
	//    MODEL_LEG_2= IT is initalized as identity matrix

	//Enable back-Face culling
	glEnable(GL_CULL_FACE);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 1000.0f);
	// =================================================================================
	// ========== HIP
	// =================================================================================
	// Transform CUBE
	Model      = glm::scale(glm::mat4(1.0f),glm::vec3(1,0.1,0.1)); //cube --> parallelepiped
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP1        = Projection * View* MODEL_EVERYTHING * Model ;
	// Use our shader
	glUseProgram(programID_1);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== HIP JOINT 1
	// =================================================================================
	//transform the cube
	glBindBuffer(GL_ARRAY_BUFFER, hipcolorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	Model      = glm::translate(glm::mat4(1.0f),glm::vec3(1,0,0));
	Model      = glm::scale(Model,glm::vec3(0.2,0.2,0.2));
	// MVP
	MVP2        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_2 * Model;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== HIP JOINT 2
	// =================================================================================
	// Transform the cube
	Model = glm::translate(glm::mat4(1.0f),glm::vec3(-1,0,0));
	Model = glm::scale(Model,glm::vec3(0.2,0.2,0.2));

	// MVP
	MVP3        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_1 * Model;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== LEG 1
	// =================================================================================
	glBindBuffer(GL_ARRAY_BUFFER, legbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, legcolorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	Model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 1, 1));
	Model = glm::translate(Model,glm::vec3(-1, -1, 1));
	// MVP
	MVP4        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_1 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== FOOT 1
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 0.5, 1));
	Model = glm::translate(Model,glm::vec3(-1, 1, 0));
	// MVP
	MVPi[6] = MVP4 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPi[6][0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	// =================================================================================
	// ========== LEG 2
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 1, 1));
	Model = glm::translate(Model,glm::vec3(1, -1, 1));
	// MVP
	MVP5        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_2 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP5[0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 2*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== FOOT 2
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), -90.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 0.5, 1));
	Model = glm::translate(Model,glm::vec3(1, 1, 0));
	// MVP
	MVPi[7] = MVP5 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPi[7][0][0]);
	// Draw the trinagles
	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	//END
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void RenderScene6()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 1000.0f);
	// =================================================================================
	// ========== HIP
	// =================================================================================
	Model      = glm::scale(glm::mat4(1.0f),glm::vec3(1,0.1,0.1)); //cube --> parallelepiped
	MVP1        = Projection * View* MODEL_EVERYTHING * Model ;
	glUseProgram(programID_1);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== HIP JOINT 1
	// =================================================================================
	glBindBuffer(GL_ARRAY_BUFFER, hipcolorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	Model      = glm::translate(glm::mat4(1.0f),glm::vec3(1,0,0));
	Model      = glm::scale(Model,glm::vec3(0.2,0.2,0.2));
	MVP2        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_2 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== HIP JOINT 2
	// =================================================================================
	Model = glm::translate(glm::mat4(1.0f),glm::vec3(-1,0,0));
	Model = glm::scale(Model,glm::vec3(0.2,0.2,0.2));
	MVP3        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_1 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== LEG 1
	// =================================================================================
	glBindBuffer(GL_ARRAY_BUFFER, legbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, legcolorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	Model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 1, 1));
	Model = glm::translate(Model,glm::vec3(-1, -1, 1));
	MVP4        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_1 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== FOOT 1
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 0.5, 1));
	Model = glm::translate(Model,glm::vec3(-1, 1, 0));
	MVPi[6] = MVP4 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPi[6][0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	// =================================================================================
	// ========== LEG 2
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 1, 1));
	Model = glm::translate(Model,glm::vec3(1, -1, 1));
	MVP5        = Projection * View * MODEL_EVERYTHING * MODEL_LEG_2 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP5[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 2*3); // 12*3 indices starting at 0 -> 12 triangles
	// =================================================================================
	// ========== FOOT 2
	// =================================================================================
	Model = glm::rotate(glm::mat4(1.0f), -90.0f, glm::vec3(0, 0, 1));
	Model = glm::scale(Model, glm::vec3(1, 0.5, 1));
	Model = glm::translate(Model,glm::vec3(1, 1, 0));
	MVPi[7] = MVP5 * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPi[7][0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	//END
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void SetupGL() //
{

	//Parameter handling
	glShadeModel (GL_SMOOTH);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	//Set up light source
	GLfloat light_position[] = { 0.0, 30.0,-50.0,0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Black background
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );   // Black background

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
	glClearDepth( 1.0f );

	//
	// Init GLEW

	// Setup initial GL State
	glClearDepth( 1.0f );

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

	static const GLfloat g_vertex_buffer_data_leg[] = {
		0.f, -1.f, 0.f,
		0.f, 1.f, -1.f,
		0.f, -1.f, -1.f,

		0.f, -1.f, -1.f,
		0.f, 1.f, -1.f,
		0.f, -1.f, 0.f,
	};
	static const GLfloat g_vertex_buffer_data_leg_color[] = {
		1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 0.f,
	};
	//VBO -- VERTEX
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	static const GLfloat g_color_buffer_data[] = {
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
		0.f, 0.f, 1.f,
	};
	static const GLfloat g_color_buffer_data_hip[] = {
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 0.f,
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &colorbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &hipcolorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hipcolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_hip), g_color_buffer_data_hip, GL_STATIC_DRAW);

	glGenBuffers(1, &legbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, legbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_leg), g_vertex_buffer_data_leg, GL_STATIC_DRAW);

	glGenBuffers(1, &legcolorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, legcolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_leg_color), g_vertex_buffer_data_leg_color, GL_STATIC_DRAW);

}
