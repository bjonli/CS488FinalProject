#version 330 core
in vec2 pos; 
out vec2 TexCoords;

uniform mat4 M;
uniform mat4 P;

void main()
{
    gl_Position = P*M*vec4(pos, 0.0, 1.0);
    TexCoords = vec2(pos.x, 1-pos.y);
} 