#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <fstream>
#include <iostream>
#include <string>

#include "model_obj.h"
#include "Vector3.h"

using namespace std;

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);

// --- Other methods ------------------------------------------------------------------------------
bool initMesh();
bool initShaders();
string readTextFile(const string &);

// --- Global variables ---------------------------------------------------------------------------
// 3D model
ModelOBJ Model; ///< A 3D model
GLuint VBO = 0; ///< A vertex buffer object
GLuint IBO = 0; ///< An index buffer object

// Shaders
GLuint ShaderProgram = 0; ///< A shader program

// Vertex transformation
Vector3f Translation; ///< Translation
float Scaling;		  ///< Scaling

// Mouse interaction
int MouseX, MouseY; ///< The last position of the mouse
int MouseButton;	///< The last mouse button pressed or released

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
	// OpenGL
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);			  // enable depth ordering
	glEnable(GL_CULL_FACE);				  // enable back-face culling
	glFrontFace(GL_CCW);				  // vertex order for the front face
	glCullFace(GL_BACK);				  // back-faces should be removed

	glPolygonMode(GL_FRONT, GL_LINE); // draw polygons as wireframe

	// Transformation
	Translation.set(0.0f, 0.0f, 0.0f);
	Scaling = 1.0f;

	// Shaders & mesh
	if (!initShaders() || !initMesh())
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

	// Set translation and scaling
	GLint trULocation = glGetUniformLocation(ShaderProgram, "translation");
	GLint sULocation = glGetUniformLocation(ShaderProgram, "scaling");
	assert(trULocation != -1 && sULocation != -1);
	glUniform3fv(trULocation, 1, Translation.get());
	glUniform1f(sULocation, Scaling);

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  sizeof(ModelOBJ::Vertex),
						  reinterpret_cast<const GLvoid *>(0));

	/*
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2,	GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));*/

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,
		Model.getNumberOfIndices(),
		GL_UNSIGNED_INT,
		0);

	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called at regular intervals (can be used for animations)
void idle()
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
	if (MouseButton == GLUT_MIDDLE_BUTTON)
	{
		Scaling += 0.003f * (MouseY - y); // Accumulate scaling amount
		MouseX = x;						  // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_LEFT_BUTTON)
	{

		// Rotations will be explained in the next lecture
	}

	glutPostRedisplay(); // Specify that the scene needs to be updated
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
bool initMesh()
{
	// Load the OBJ model
	if (!Model.import("capsule/capsule.obj"))
	{
		cerr << "Error: cannot load model." << endl;
		return false;
	}

	// Notice that normals may not be stored in the model
	// This issue will be dealt with in the next lecture

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
				 Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex),
				 Model.getVertexBuffer(),
				 GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 3 * Model.getNumberOfTriangles() * sizeof(int),
				 Model.getIndexBuffer(),
				 GL_STATIC_DRAW);

	return true;
} /* initBuffers() */

/// Initialize shaders. Return false if initialization fail
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