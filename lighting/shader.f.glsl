#version 330	// GLSL version

// In this version, the illumination is computed in the vertex shader, the 
// resulting color is then interpolated and, in the fragment shader it is 
// assigned to the fragment. Note that you can also pass the normals from the 
// vertex to the fragment shader and compute the illumination in the fragment
// shader. It would be more computationally expensive, but the results look 
// far better.

// the color arriving from the vertex shader
in vec4 fcolor;

// Per-frgament output color
out vec4 FragColor;

void main() { 
	// Set the output color according to the input
    FragColor = fcolor;
	
}