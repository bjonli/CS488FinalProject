#version 330 core
in vec2 position; 
in vec2 texCoords;
out vec2 texCoords_fs;

uniform mat4 M;
uniform mat4 P;

void main()
{
    gl_Position = P*M*vec4(position, 0.0, 1.0);
	texCoords_fs = texCoords;
} 
