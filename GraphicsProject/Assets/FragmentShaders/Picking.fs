#version 330
// INPUTS/OUTPUTS -----------------------------------------
out vec4 fragColour;
uniform vec3 colour;

void main() {
    fragColour = vec4(colour.r, colour.g, colour.b, 1);
}
