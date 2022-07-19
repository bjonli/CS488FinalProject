
#version 330 core
in vec3 pos;        
in vec4 color;
in vec4 offset;     // offset[3] stores size 

out vec4 fColor;

uniform mat4 View;
uniform mat4 Perspective;
uniform vec3 viewPos;

void main()
{
    // make sure the particles always face you
    // CR-someday: the particle now always faces you, (normal parallel to viewDir)
    // but other rotation axes are wonky. Should not matter as they are just particles
    vec3 newZ = normalize(viewPos-vec3(offset));
    vec3 newX = cross(vec3(0, 1, 1), newZ);
    vec3 newY = cross(newZ, newX);
    mat4 rotation = mat4(
        vec4(newX, 0), 
        vec4(newY, 0), 
        vec4(newZ, 0), 
        vec4(0, 0, 0, 1));

    mat4 translation = mat4(1);
    translation[3] = vec4(vec3(offset), 1);
    gl_Position =  Perspective * View * translation 
        * rotation * vec4(pos*offset[3], 1.0);

    fColor = color;
}