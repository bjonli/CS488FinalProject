#version 330 core
in vec3 position;

out vec3 TexCoords;

uniform mat4 Perspective;
uniform mat4 View;

void main()
{
    TexCoords = position;
    vec4 pos = Perspective * View * vec4(position, 1.0);
    gl_Position = pos.xyww;
}  