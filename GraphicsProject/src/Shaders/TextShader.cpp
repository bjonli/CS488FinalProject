#include "TextShader.hpp"
#include "../Application/CS488Window.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int FONT_SIZE = 48;

TextShader::TextShader(std::string fontFile_, float wH, float wW)
    : ShaderProgram(),  windowH(wH), windowW(wW) {
    generateProgramObject();
	attachVertexShader( "Text.vs" );
	attachFragmentShader( "Text.fs" );
	link();

	fontFile = CS488Window::getAssetFilePath(
		("Fonts/" + fontFile_).c_str()
	).c_str();
}

void TextShader::initData() {
    // first try to load library and load the font file
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Error: Cannot Init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontFile.c_str(), 0, &face))
    {
        std::cout << "Error: Cannot load font " << fontFile << std::endl;  
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, FONT_SIZE); 

    // load the texture of all 128 character (no unicode) to the map
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int) (face->glyph->advance.x)
        };
        characters[c] = character;
    }

    // done processing, free resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

	// initialize the vba and vao, same as the spriteShader
    float vertices[6][2] = {                               
        { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }           
    };
    glGenVertexArrays(1, &m_vao_text);
    glBindVertexArray(m_vao_text);
	GLuint m_vbo_text;
	glGenBuffers(1, &m_vbo_text);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, &vertices[0], GL_STATIC_DRAW);
    GLint posLocation = getAttribLocation("pos");
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
	// set the projection matrix
    enable();
    {
        GLint location = getUniformLocation("P");
        glm::mat4 P = glm::ortho(0.0f, windowW, 0.0f, windowH);
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P));
    }
    disable();
    
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  
}

void TextShader::renderText(
    std::string text, float x, float y, float scale, glm::vec4 colour    
) {
    enable();
    {
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_vao_text);

        GLint location = getUniformLocation("textColor");
        glUniform4fv(location, 1, value_ptr(colour));
        
        float widthScaleFactor = scale;
        float heightScaleFactor = scale; 
        float adv = x;						// position of next char to draw
		// draw character by character
        for (int i=0; i<text.length(); i++) {
            Character ch = characters[text[i]];
            float xScale = ch.size.x*widthScaleFactor;
            float yScale = ch.size.y*heightScaleFactor;

            float xpos = adv + ch.bearing.x*widthScaleFactor;
            float ypos = y - (ch.size.y - ch.bearing.y)*heightScaleFactor;

            glm::mat4 tM = glm::translate(glm::mat4(1), glm::vec3(xpos, ypos, 0));
            glm::mat4 M = glm::scale(tM, glm::vec3(xScale, yScale, 1.0));

            GLint location = getUniformLocation("M");
            glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(M));
        
            glBindTexture(GL_TEXTURE_2D, ch.textureID);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            adv += (ch.advance >> 6)*widthScaleFactor;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    disable();
}