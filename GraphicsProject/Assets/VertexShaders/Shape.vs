#version 330 core
in vec2 position; 

uniform mat4 M;
uniform mat4 P;

void main()
{
    gl_Position = P*M*vec4(position, 0.0, 1.0);
} 
