#include "SkyboxShader.hpp"
#include "../Application/CS488Window.hpp"
#include "../Application/GlErrorCheck.hpp"
#include "../stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), 
            &width, &height, &nrChannels, 0);
        
        GLenum format;              // extract format of data
        bool parsable = false;
		// infer format based on the number of channels
        switch(nrChannels) {
            case 1:
                format = GL_RED; parsable = true;
                break;
            case 3:
                format = GL_RGB; parsable = true;
                break;
            case 4:
                format = GL_RGBA; parsable = true;
                break;
            default:
                std::cout << "Error: Unparsable Texture format " << faces[i] << std::endl;
        }
        if (data && parsable)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
} 

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

SkyboxShader::SkyboxShader(std::string folder, std::string ext)
	: ShaderProgram(), isEnabled(true) 
{
    generateProgramObject();
    attachVertexShader( "Skybox.vs" );
	attachFragmentShader( "Skybox.fs" );
	link();

    // setup the skybox vbo and vao
    enable();
    glGenVertexArrays(1, &skyboxVAO);
	GLuint skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    
    GLint posLocation = getAttribLocation("position");
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    CHECK_GL_ERRORS;

    // grab the file names from the folder
    std::string path = CS488Window::getAssetFilePath(folder.c_str());
    std::vector<std::string> paths = {
        path + "/right." + ext,
        path + "/left." + ext,
        path + "/top." + ext,
        path + "/bottom." + ext,
        path + "/front." + ext,
        path + "/back." + ext
    };

    skyboxTextureID = loadCubemap(paths);
    disable();
}

void SkyboxShader::loadUniforms(glm::mat4& P) {
    enable();
    {
        glUniform1i(getUniformLocation("skybox"), 0);
        // Perspective matrix
        glUniformMatrix4fv(getUniformLocation("Perspective"), 1, GL_FALSE, value_ptr(P));
        CHECK_GL_ERRORS;
    }
    disable();
}

void SkyboxShader::draw(glm::mat4 V, glm::vec3 viewPos) {
	if (!isEnabled) { return; }
    glBindVertexArray(skyboxVAO);
    enable();
    {
        glm::mat4 VWithoutTranslation = glm::mat4(glm::mat3(V));
        glUniformMatrix4fv(getUniformLocation("View"), 1, GL_FALSE, value_ptr(VWithoutTranslation));

        // setup the texture, then draw cubemap
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    disable();
    glBindVertexArray(0);
}

bool SkyboxShader::getIsEnabled() { return isEnabled; }

void SkyboxShader::setIsEnabled(bool b) { isEnabled = b; }