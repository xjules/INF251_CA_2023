#version 330	// GLSL version

// Translation to apply to vertices (constant for every vertex)
uniform vec3 translation;
uniform float scaling;

// The position of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 position; 

// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 color;

void main() {
	// translate the vertex
    gl_Position = vec4(scaling * position.x + translation.x, 
					   scaling * position.y + translation.y, 
					   scaling * position.z, 
					   1.0);
	
	// set the color of the vertex
	color.r = clamp(gl_Position.x, 0., 1.);
	color.g = clamp(gl_Position.y, 0., 1.);
	color.b = clamp(gl_Position.z, 0., 1.);
	color.a = 1.;	
}