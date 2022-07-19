#pragma once

#include "TextureManager.hpp"
#include "OpenGLImport.hpp"

#include <string>
#include <vector>

// we could make this an enum, but let's 
// make it consistent with the fragment shader
#define LANTERN_NORMAL 0
#define LANTERN_GRAYSCALE 1
#define LANTERN_CELSHADING 2
#define LANTERN_INVERT 3

class Flame {
    public:
        Flame();
        Flame(glm::vec4 oC, glm::vec4 iC, std::string n, int i, GLint sId);
        glm::vec4 outerColour;
        glm::vec4 innerColour;
        std::string name;
		GLuint spriteId;
        int id;
};

class FlameManager {
    std::vector<Flame> allFlames;
    
    public:
        FlameManager(TextureManager* textureManager);
        Flame getNextFlame(Flame f);
        Flame getPrevFlame(Flame f);
        Flame getFlameById(int id);
};