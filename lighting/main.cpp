#ifdef __APPLE__
#include <GL/glew.h>   // Always first
#include <OpenGL/gl.h> // Then the main OpenGL header
#include <GLUT/glut.h> // Then GLUT or other windowing headers
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "Vector3.h"
#include "Matrix4.h"
#include <algorithm>

using namespace std;

// --- Data types ---------------------------------------------------------------------------------
/// A simple structure for holding a vertex and its texture coordinates
struct Vertex
{
	Vector3f position;
	Vector3f normal;
};

/// A simple structure to handle a moving camera (perspective projection)
struct Camera
{
	Vector3f position; ///< the position of the camera
	Vector3f target;   ///< the direction the camera is looking at
	Vector3f up;	   ///< the up vector of the camera

	float fov; ///< camera field of view
	float ar;  ///< camera aspect ratio

	float zNear, zFar; ///< depth of the near and far plane

	float zoom; ///< an additional scaling parameter
};

// TODO: Now materials and light properties are hard coded in display(), I
// would suggest you to create a struct/class and methods to store and set them
// on the shaders.

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);
void special(int, int, int);

// --- Other methods ------------------------------------------------------------------------------
void initBuffers();
bool initShaders();
Matrix4f computeCameraTransform(const Camera &);
string readTextFile(const string &);

// --- Global variables ---------------------------------------------------------------------------
// Shader program
GLuint ShaderProgram = 0;
GLint TrLoc = -1;
GLint CameraPositionLoc = -1;
GLint DLightDirLoc = -1;
GLint DLightAColorLoc = -1;
GLint DLightDColorLoc = -1;
GLint DLightSColorLoc = -1;
GLint DLightAIntensityLoc = -1;
GLint DLightDIntensityLoc = -1;
GLint DLightSIntensityLoc = -1;
GLint MaterialAColorLoc = -1;
GLint MaterialDColorLoc = -1;
GLint MaterialSColorLoc = -1;
GLint MaterialShineLoc = -1;
// TODO: add the ones for the headlight

// Model of the pyramid
const int PYRAMID_VERTS_NUM = 5;
const int PYRAMID_TRIS_NUM = 6;
GLuint PyramidVBO = 0;
GLuint PyramidIBO = 0;
// Model of the grass
const int GRASS_VERTS_NUM = 9;
const int GRASS_TRIS_NUM = 8;
GLuint GrassVBO = 0;
GLuint GrassIBO = 0;
// Model of the wall
const int WALL_SIDE_VERTS_NUM = 16;
const int WALL_VERTS_NUM = WALL_SIDE_VERTS_NUM * WALL_SIDE_VERTS_NUM;
const int WALL_TRIS_NUM = (WALL_SIDE_VERTS_NUM - 1) * (WALL_SIDE_VERTS_NUM - 1) * 2;
GLuint WallVBO = 0;
GLuint WallIBO = 0;

// Mouse control
int MouseX, MouseY;
int MouseButton;

// Camera
Camera Cam;

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
	glutSpecialFunc(special);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cerr << "Error initializing glew: \n"
			 << reinterpret_cast<const char *>(glewGetErrorString(res)) << endl;
		cout << "\nPress Enter to exit..." << endl;
		getchar();
		return -1;
	}

	// Initialize program variables
	// Camera
	Cam.position.set(0.f, 0.f, 0.f);
	Cam.target.set(0.f, 0.f, -1.f);
	Cam.up.set(0.f, 1.f, 0.f);
	Cam.fov = 30.f;
	Cam.ar = 1.f; // will be correctly initialized in the "display()" method
	Cam.zNear = 0.1f;
	Cam.zFar = 100.f;
	Cam.zoom = 1.f;

	// OpenGL
	glutSetCursor(GLUT_CURSOR_CROSSHAIR); // hide the cursor
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f);
	initBuffers();
	if (!initShaders())
	{
		cout << "Press Enter to exit..." << endl;
		getchar();
		return -1;
	}

	// Start the main event loop
	glutMainLoop();

	return 0;
}

// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************
/// Called whenever the scene has to be drawn
void display()
{
	// Prepare the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	// Set the camera position and transformation
	Cam.ar = (1.0f * width) / height;
	Matrix4f transformation = computeCameraTransform(Cam);

	glUniform3fv(CameraPositionLoc, 1, Cam.position.get());
	glUniformMatrix4fv(TrLoc, 1, GL_FALSE, transformation.get());

	// Set the light parameters
	glUniform3f(DLightDirLoc, 0.5f, -0.5f, -1.0f);
	glUniform3f(DLightAColorLoc, 0.05f, 0.03f, 0.0f);
	glUniform3f(DLightDColorLoc, 0.5f, 0.4f, 0.3f);
	glUniform3f(DLightSColorLoc, 0.6f, 0.6f, 0.7f);
	glUniform1f(DLightAIntensityLoc, 1.0f);
	glUniform1f(DLightDIntensityLoc, 1.0f);
	glUniform1f(DLightSIntensityLoc, 1.0f);

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Set the material parameters for the grass
	glUniform3f(MaterialAColorLoc, 0.9f, 1.0f, 0.9f);
	glUniform3f(MaterialDColorLoc, 0.3f, 1.0f, 0.3f);
	glUniform3f(MaterialSColorLoc, 0.1f, 0.1f, 0.1f);
	glUniform1f(MaterialShineLoc, 10.0f);

	// Draw the grass
	glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // glVertexAttribPointer needs to be repeated for every mesh!
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(sizeof(Vector3f)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GrassIBO);
	glDrawElements(GL_TRIANGLES, 3 * GRASS_TRIS_NUM, GL_UNSIGNED_INT, 0);

	// Set the material parameters for the pyramid
	glUniform3f(MaterialAColorLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(MaterialDColorLoc, 1.0f, 0.8f, 0.8f);
	glUniform3f(MaterialSColorLoc, 0.5f, 0.5f, 0.5f);
	glUniform1f(MaterialShineLoc, 20.0f);

	// Draw the pyramid
	glBindBuffer(GL_ARRAY_BUFFER, PyramidVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PyramidIBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // glVertexAttribPointer needs to be repeated for every mesh!
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(sizeof(Vector3f)));
	glDrawElements(GL_TRIANGLES, 3 * PYRAMID_TRIS_NUM, GL_UNSIGNED_INT, 0);

	// Set the material parameters for the wall
	glUniform3f(MaterialAColorLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(MaterialDColorLoc, 0.6f, 0.6f, 0.6f);
	glUniform3f(MaterialSColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform1f(MaterialShineLoc, 50.0f);

	// Draw the wall
	glBindBuffer(GL_ARRAY_BUFFER, WallVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WallIBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // glVertexAttribPointer needs to be repeated for every mesh!
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(Vertex), reinterpret_cast<const GLvoid *>(sizeof(Vector3f)));
	glDrawElements(GL_TRIANGLES, 3 * WALL_TRIS_NUM, GL_UNSIGNED_INT, 0);

	// clean-up
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);

	// Lock the mouse at the center of the screen
	glutWarpPointer(MouseX, MouseY);

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
	Vector3f right;

	switch (tolower(key))
	{
		// --- camera movements ---
	case 'w':
		Cam.position += Cam.target * 0.1f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.1f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.1f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.1f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.1f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.1f;
		break;
	case 'r': // Reset camera status
		Cam.position.set(0.f, 0.f, 0.f);
		Cam.target.set(0.f, 0.f, -1.f);
		Cam.up.set(0.f, 1.f, 0.f);
		Cam.fov = 30.f;
		Cam.ar = 1.f;
		Cam.zNear = 0.1f;
		Cam.zFar = 100.f;
		Cam.zoom = 1.f;
		break;

		// --- utilities ---
	case 'p': // change to wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'o': // change to polygon rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'l': // reload shaders
		cout << "Re-loading shaders..." << endl;
		if (initShaders())
		{
			cout << "> done." << endl;
			glutPostRedisplay();
		}
		break;
	case 'q': // terminate the application
		exit(0);
		break;
	}
	// redraw
	glutPostRedisplay();
}

// Called whenever a special keyboard button is pressed
void special(int key, int x, int y)
{
	Vector3f right;

	switch (key)
	{
		// --- camera movements ---
	case GLUT_KEY_PAGE_UP: // Increase field of view
		Cam.fov = min(Cam.fov + 1.f, 179.f);
		break;
	case GLUT_KEY_PAGE_DOWN: // Decrease field of view
		Cam.fov = max(Cam.fov - 1.f, 1.f);
		break;

		// --- utilities ---
	case GLUT_KEY_F5: // Reload shaders
		cout << "Re-loading shaders..." << endl;
		if (initShaders())
		{
			cout << "> done." << endl;
		}
		break;
	}
	// redraw
	glutPostRedisplay();
}

/// Called whenever a mouse event occur (press or release)
void mouse(int button, int state, int x, int y)
{
	// Store the current mouse status
	MouseButton = button;

	// Instead of updating the mouse position, lock it at the center of the screen
	MouseX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	MouseY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	glutWarpPointer(MouseX, MouseY);
}

/// Called whenever the mouse is moving while a button is pressed
void motion(int x, int y)
{

	if (MouseButton == GLUT_RIGHT_BUTTON)
	{
		Cam.position += Cam.target * 0.003f * (MouseY - y);
		Cam.position += Cam.target.cross(Cam.up) * 0.003f * (x - MouseX);
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON)
	{
		Cam.zoom = max(0.001f, Cam.zoom + 0.003f * (y - MouseY));
	}
	if (MouseButton == GLUT_LEFT_BUTTON)
	{
		Matrix4f ry, rr;

		// "horizontal" rotation
		ry.rotate(0.1f * (MouseX - x), Vector3f(0, 1, 0));
		Cam.target = ry * Cam.target;
		Cam.up = ry * Cam.up;

		// "vertical" rotation
		rr.rotate(0.1f * (MouseY - y), Cam.target.cross(Cam.up));
		Cam.up = rr * Cam.up;
		Cam.target = rr * Cam.target;
	}

	// Redraw the scene
	glutPostRedisplay();
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
void initBuffers()
{
	// Prepare the vertices of the pyramid
	Vertex pyramidVerts[PYRAMID_VERTS_NUM];
	pyramidVerts[0].position.set(-0.5f, -0.5f, -5.0f);
	pyramidVerts[1].position.set(0.5f, -0.5f, -5.0f);
	pyramidVerts[2].position.set(-0.5f, -0.5f, -4.0f);
	pyramidVerts[3].position.set(0.5f, -0.5f, -4.0f);
	pyramidVerts[4].position.set(0.0f, 0.5f, -4.5f);
	pyramidVerts[0].normal.set(-0.5f, -0.25f, -0.5f);
	pyramidVerts[1].normal.set(0.5f, -0.25f, -0.5f);
	pyramidVerts[2].normal.set(-0.5f, -0.25f, 0.5f);
	pyramidVerts[3].normal.set(0.5f, -0.25f, 0.5f);
	pyramidVerts[4].normal.set(0.0f, 1.0f, 0.0f);

	// Generate a VBO
	glGenBuffers(1, &PyramidVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PyramidVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 PYRAMID_VERTS_NUM * sizeof(Vertex),
				 pyramidVerts,
				 GL_STATIC_DRAW);

	// Create an array of indices representing the triangles (faces of the cube)
	unsigned int pyramidTris[3 * PYRAMID_TRIS_NUM] = {
		0, 1, 2, // bottom
		2, 1, 3,
		0, 2, 4, // left face
		1, 4, 3, // right face
		2, 3, 4, // front face
		1, 0, 4, // back face
	};

	// Create an IBO
	glGenBuffers(1, &PyramidIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PyramidIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 3 * PYRAMID_TRIS_NUM * sizeof(unsigned int),
				 pyramidTris,
				 GL_STATIC_DRAW);

	// Prepare the vertices of the grass
	Vertex grassVerts[GRASS_VERTS_NUM];
	grassVerts[0].position.set(-10.f, -0.5f, -10.f);
	grassVerts[1].position.set(0.f, -0.5f, -10.f);
	grassVerts[2].position.set(10.f, -0.5f, -10.f);
	grassVerts[3].position.set(-10.f, -0.5f, 0.f);
	grassVerts[4].position.set(0.f, -0.5f, 0.f);
	grassVerts[5].position.set(10.f, -0.5f, 0.f);
	grassVerts[6].position.set(-10.f, -0.5f, 10.f);
	grassVerts[7].position.set(0.f, -0.5f, 10.f);
	grassVerts[8].position.set(10.f, -0.5f, 10.f);
	grassVerts[0].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[1].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[2].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[3].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[4].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[5].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[6].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[7].normal.set(0.0f, 1.0f, 0.0f);
	grassVerts[8].normal.set(0.0f, 1.0f, 0.0f);

	// Generate a VBO
	glGenBuffers(1, &GrassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, GrassVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 GRASS_VERTS_NUM * sizeof(Vertex),
				 grassVerts,
				 GL_STATIC_DRAW);

	// Create an array of indices representing the triangles
	unsigned int grassTris[3 * GRASS_TRIS_NUM] = {
		0, 3, 4,
		0, 4, 1,
		1, 4, 5,
		1, 5, 2,
		3, 6, 7,
		3, 7, 4,
		4, 7, 8,
		4, 8, 5};

	// Create an IBO
	glGenBuffers(1, &GrassIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GrassIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 3 * GRASS_TRIS_NUM * sizeof(unsigned int),
				 grassTris,
				 GL_STATIC_DRAW);

	// Prepare the vertices of the wall
	Vertex wallVerts[WALL_VERTS_NUM];
	for (int r = 0; r < WALL_SIDE_VERTS_NUM; ++r)
	{
		for (int c = 0; c < WALL_SIDE_VERTS_NUM; ++c)
		{
			const int ID = r * WALL_SIDE_VERTS_NUM + c;
			wallVerts[ID].position.set(
				-5.0f + (10.0f * r) / (WALL_SIDE_VERTS_NUM - 1.0f),
				-0.5f + (10.0f * c) / (WALL_SIDE_VERTS_NUM - 1.0f),
				-10.0f);
			wallVerts[ID].normal.set(0.0f, 0.0f, 1.0f);
		}
	}

	// Generate a VBO
	glGenBuffers(1, &WallVBO);
	glBindBuffer(GL_ARRAY_BUFFER, WallVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 WALL_VERTS_NUM * sizeof(Vertex),
				 wallVerts,
				 GL_STATIC_DRAW);

	// Create an array of indices representing the triangles of the wall
	unsigned int wallTris[3 * WALL_TRIS_NUM];
	for (int r = 0; r < WALL_SIDE_VERTS_NUM - 1; ++r)
	{
		for (int c = 0; c < WALL_SIDE_VERTS_NUM - 1; ++c)
		{
			// the id of the first vertex of the triangle
			const int VERT_ID = r * WALL_SIDE_VERTS_NUM + c;

			// 2 * 3 indices are created at each iteration
			const int TRI_ID = 2 * 3 *
							   (r * (WALL_SIDE_VERTS_NUM - 1) + c);

			wallTris[TRI_ID] = VERT_ID; // first triangle
			wallTris[TRI_ID + 1] = VERT_ID + 1;
			wallTris[TRI_ID + 2] = VERT_ID + WALL_SIDE_VERTS_NUM;

			wallTris[TRI_ID + 3] = VERT_ID + 1; // second triangle
			wallTris[TRI_ID + 4] = VERT_ID + WALL_SIDE_VERTS_NUM + 1;
			wallTris[TRI_ID + 5] = VERT_ID + WALL_SIDE_VERTS_NUM;
		}
	}

	// Create an IBO
	glGenBuffers(1, &WallIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WallIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 3 * WALL_TRIS_NUM * sizeof(unsigned int),
				 wallTris,
				 GL_STATIC_DRAW);
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

	// Get the location of the uniform variables
	TrLoc = glGetUniformLocation(ShaderProgram, "transformation");
	assert(TrLoc != -1); // check for errors (variable not found)

	CameraPositionLoc = glGetUniformLocation(ShaderProgram, "camera_position");
	DLightDirLoc = glGetUniformLocation(ShaderProgram, "d_light_direction");
	DLightAColorLoc = glGetUniformLocation(ShaderProgram, "d_light_a_color");
	DLightDColorLoc = glGetUniformLocation(ShaderProgram, "d_light_d_color");
	DLightSColorLoc = glGetUniformLocation(ShaderProgram, "d_light_s_color");
	DLightAIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_a_intensity");
	DLightDIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_d_intensity");
	DLightSIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_s_intensity");
	MaterialAColorLoc = glGetUniformLocation(ShaderProgram, "material_a_color");
	MaterialDColorLoc = glGetUniformLocation(ShaderProgram, "material_d_color");
	MaterialSColorLoc = glGetUniformLocation(ShaderProgram, "material_s_color");
	MaterialShineLoc = glGetUniformLocation(ShaderProgram, "material_shininess");

	// Shaders can be deleted now
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
} /* initShaders() */

/// Return the transformation matrix corresponding to the specified camera
Matrix4f computeCameraTransform(const Camera &cam)
{
	// camera rotation
	Vector3f t = cam.target.getNormalized();
	Vector3f u = cam.up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,
				  u.x(), u.y(), u.z(), 0.f,
				  -t.x(), -t.y(), -t.z(), 0.f,
				  0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-cam.position);

	// perspective projection
	Matrix4f prj = Matrix4f::createPerspectivePrj(cam.fov, cam.ar, cam.zNear, cam.zFar);

	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(cam.zoom, cam.zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR * camT;

} /* computeCameraTransform() */

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
		bool a = fileIn.bad();
		bool b = fileIn.fail();
		if (fileIn.bad() || (fileIn.fail() && !fileIn.eof()))
		{
			cerr << "Warning: problems reading file " << pathAndFileName.c_str()
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