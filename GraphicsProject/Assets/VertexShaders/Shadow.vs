#version 330 core
in vec3 position;

uniform mat4 Perspective;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    gl_Position = Perspective * View * Model * vec4(position, 1.0);
}