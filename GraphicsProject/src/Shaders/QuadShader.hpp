#pragma once
#include "ShaderProgram.hpp"
#include "ShadowShader.hpp"

// CR-someday: possibly remove 
class QuadShader: ShaderProgram {
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    ShadowShader* shadowShader;

    public:
        QuadShader(ShadowShader* ss);
        void draw();
} ;