#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
#include "Vector3.h"

using namespace std; // to avoid specifying std:: before methods and classes of
					 // the C++ Standard library

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void keyboard(unsigned char, int, int);

// --- Other methods ------------------------------------------------------------------------------
void initBuffers();

// --- Global variables ---------------------------------------------------------------------------
const int NUMBER_OF_VERTICES = 4;  ///< The number of vertices to draw
const int NUMBER_OF_TRIANGLES = 2; ///< The number of triangles to draw

GLuint VBO; ///< A vertex buffer object
GLuint IBO; ///< An index buffer object

// --- main() -------------------------------------------------------------------------------------
/// The entry point of the application
int main(int argc, char **argv)
{

	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("OpenGL Tutorial");

	// Initialize OpenGL callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cout << "Error initializing glew: \n"
			 << reinterpret_cast<const char *>(glewGetErrorString(res)) << endl;
		return 1;
	}

	// Initialize program variables
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f); // window background
	initBuffers();						  // buffer objects

	// Start the main event loop
	glutMainLoop();

	return 0;
}

// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************
/// Called whenever the scene has to be drawn
void display()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable the "position" vertex attribute (location 0 in the fixed pipeline)
	glEnableVertexAttribArray(0);

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Specify how the data should be interpreted
	glVertexAttribPointer(
		0,		  // the attribute we are referring to
		3,		  // the number of components (x, y, z)
		GL_FLOAT, // the basic type of data
		GL_FALSE, // should the data be normalized?
		0,		  // number of bytes between 2 values
		0);		  // offset of the first byte

	// Bind the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,			 // the type of primitive to produce
		3 * NUMBER_OF_TRIANGLES, // the number of indices
		GL_UNSIGNED_INT,		 // the type of the indices
		0);						 // offset of the first index

	// Disable the "position" vertex attribute (not necessary, but recommended)
	glDisableVertexAttribArray(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called whenever a keyboard button is pressed (only ASCII characters)
void keyboard(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'q': // terminate the application
		exit(0);
		break;
	}
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
void initBuffers()
{
	// Create an array of vectors representing the vertices
	Vector3f vertices[NUMBER_OF_VERTICES];
	vertices[0] = Vector3f(-1.0f, 0.0f, 0.0f);
	vertices[1] = Vector3f(1.0f, 0.0f, 0.0f);
	vertices[2] = Vector3f(0.0f, -1.0f, 0.0f);
	vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);

	// Generate a VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,						// the target
				 NUMBER_OF_VERTICES * sizeof(Vector3f), // the size of the data in bytes
				 vertices,								// pointer to the data
				 GL_STATIC_DRAW);						// static vs dynamic

	// Create an array of indices representing the triangles
	unsigned int indices[3 * NUMBER_OF_TRIANGLES] = {
		0, 1, 2,  // first triangle
		0, 3, 1}; // second triangle

	// Create a buffer
	glGenBuffers(1, &IBO);

	// Set it as a buffer for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Set the data for the current IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,						 // the target
				 3 * NUMBER_OF_TRIANGLES * sizeof(unsigned int), // the size of the data
				 indices,										 // pointer to the data
				 GL_STATIC_DRAW);								 // static vs dynamic
}
