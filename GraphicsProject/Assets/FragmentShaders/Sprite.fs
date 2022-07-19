#version 330 core
in vec2 texCoords_fs;
out vec4 color;

uniform sampler2D spriteTexture;

void main() {    
    color = texture(spriteTexture, texCoords_fs);
}