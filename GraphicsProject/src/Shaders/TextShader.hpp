#pragma once
#include "ShaderProgram.hpp"

#include <glm/glm.hpp>
#include <map>
#include <freetype/ft2build.h>
#include FT_FREETYPE_H 

struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

class TextShader : public ShaderProgram {
    std::string fontFile;
    std::map<char, Character> characters;

	GLuint m_vao_text;
    float windowH;
    float windowW;

    public:
        // only support 1 font for now
        TextShader(std::string fontFile, float wH, float wW);
        void initData();
        void renderText(std::string text, float x, float y, float scale, glm::vec4 colour);
};