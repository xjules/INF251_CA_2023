#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <fstream>
#include <iostream>
#include <string>

#include "Vector3.h"

using namespace std;

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void reshape(int, int);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);

// --- Other methods ------------------------------------------------------------------------------
void initBuffers();
bool initShaders();
string readTextFile(const string &);

// --- Global variables ---------------------------------------------------------------------------
const int NUMBER_OF_VERTICES = 4;  ///< The number of vertices to draw
const int NUMBER_OF_TRIANGLES = 2; ///< The number of triangles to draw

GLuint VBO = 0; ///< A vertex buffer object
GLuint IBO = 0; ///< An index buffer object

GLuint ShaderProgram = 0; ///< A shader program

int MouseX, MouseY; ///< The last position of the mouse
int MouseButton;	///< The last mouse button pressed or released

Vector3f Translation; ///< The translation to apply to the vertices

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
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cerr << "Error initializing glew: \n"
			 << reinterpret_cast<const char *>(glewGetErrorString(res)) << endl;
		return -1;
	}

	// Initialize program variables
	Translation.set(0, 0, 0);
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f);
	initBuffers();
	if (!initShaders())
		return -1;

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

	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	// Enable the "position" vertex attribute (0 in the fixed pipeline) and set its format
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set the uniform variable for the translation
	GLint trUniformLocation = glGetUniformLocation(ShaderProgram, "translation");
	assert(trUniformLocation != -1); // check for errors (variable not found)
	glUniform3fv(trUniformLocation, 1, Translation.get());

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,			 // the type of primitive to produce
		3 * NUMBER_OF_TRIANGLES, // the number of indices
		GL_UNSIGNED_INT,		 // the type of the indices
		0);						 // offset of the first index

	// Disable the "position" vertex attribute (not necessary but recommended)
	glDisableVertexAttribArray(0);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called at regular intervals (can be used for animations)
void idle()
{
}

/// Called whenever the window is resized
void reshape(int width, int height)
{
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
	case 'r':
		cout << "Re-loading shaders..." << endl;
		if (initShaders())
		{
			cout << "> done." << endl;
			glutPostRedisplay();
		}
	}
}

/// Called whenever a mouse event occur (press or release)
void mouse(int button, int state, int x, int y)
{
	// Store the current mouse status
	MouseButton = button;
	MouseX = x;
	MouseY = y;
}

/// Called whenever the mouse is moving while a button is pressed
void motion(int x, int y)
{
	if (MouseButton == GLUT_RIGHT_BUTTON)
	{
		Translation.x() += 0.003f * (x - MouseX); // Accumulate translation amount
		Translation.y() += 0.003f * (MouseY - y);
		MouseX = x; // Store the current mouse position
		MouseY = y;
	}

	glutPostRedisplay(); // Specify that the scene needs to be updated
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
void initBuffers()
{
	// Create an array of vectors representing the vertices
	Vector3f vertices[NUMBER_OF_VERTICES];
	vertices[0] = Vector3f(-0.5f, 0.0f, 0.0f);
	vertices[1] = Vector3f(0.5f, 0.0f, 0.0f);
	vertices[2] = Vector3f(0.0f, -0.5f, 0.0f);
	vertices[3] = Vector3f(0.0f, 0.5f, 0.0f);

	// Generate a VBO as in the previous tutorial
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
				 NUMBER_OF_VERTICES * sizeof(Vector3f),
				 vertices,
				 GL_STATIC_DRAW);

	// Create an array of indices representing the triangles
	unsigned int indices[3 * NUMBER_OF_TRIANGLES] = {
		0, 1, 2,  // first triangle
		0, 3, 1}; // second triangle

	// Create a buffer
	glGenBuffers(1, &IBO);

	// Set it as a buffer for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Set the data for the current IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 3 * NUMBER_OF_TRIANGLES * sizeof(unsigned int),
				 indices,
				 GL_STATIC_DRAW);
} /* initBuffers() */

/// Initialize shaders. Return false if initialization fail.
bool initShaders()
{
	// Create the shader program and check for errors
	if (ShaderProgram != 0)
		glDeleteProgram(ShaderProgram);
	ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0)
	{
		cerr << "Error: cannot create shader program." << endl;
		return false;
	}

	// Create the shader objects and check for errors
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (vertShader == 0 || fragShader == 0)
	{
		cerr << "Error: cannot create shader objects." << endl;
		return false;
	}

	// Read and set the source code for the vertex shader
	string text = readTextFile("shader.v.glsl");
	const char *code = text.c_str();
	int length = static_cast<int>(text.length());
	if (length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile("shader.f.glsl");
	const char *code2 = text2.c_str();
	length = static_cast<int>(text2.length());
	if (length == 0)
		return false;
	glShaderSource(fragShader, 1, &code2, &length);

	// Compile the shaders
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	// Check for compilation error
	GLint success;
	GLchar errorLog[1024];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile vertex shader.\nError log:\n"
			 << errorLog << endl;
		return false;
	}
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile fragment shader.\nError log:\n"
			 << errorLog << endl;
		return false;
	}

	// Attach the shader to the program and link it
	glAttachShader(ShaderProgram, vertShader);
	glAttachShader(ShaderProgram, fragShader);
	glLinkProgram(ShaderProgram);

	// Check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n"
			 << errorLog << endl;
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram);

	// Check for validation error
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot validate shader program.\nError log:\n"
			 << errorLog << endl;
		return false;
	}

	// Shaders can be deleted now
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
} /* initShaders() */

/// Read the specified file and return its content
string readTextFile(const string &pathAndFileName)
{
	// Try to open the file
	ifstream fileIn(pathAndFileName);
	if (!fileIn.is_open())
	{
		cerr << "Error: cannot open file " << pathAndFileName.c_str();
		return "";
	}

	// Read the file
	string text = "";
	string line;
	while (!fileIn.eof())
	{
		getline(fileIn, line);
		text += line + "\n";
		bool bad = fileIn.bad();
		bool fail = fileIn.fail();
		if (fileIn.bad() || (fileIn.fail() && !fileIn.eof()))
		{
			cerr << "Warning: problems reading file " << pathAndFileName.c_str()
				 << "\nBad flag: " << bad << "\tFail flag: " << fail
				 << "\nText read: \n"
				 << text.c_str();
			fileIn.close();
			return text;
		}
	}
	// finalize
	fileIn.close();

	return text;
} /* readTextFile() */

/* --- eof main.cpp --- */