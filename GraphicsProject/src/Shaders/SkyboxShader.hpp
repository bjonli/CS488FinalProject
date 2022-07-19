#pragma once

#include "ShaderProgram.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>

class SkyboxShader : public ShaderProgram {
    GLuint skyboxTextureID;
    GLuint skyboxVAO;
	bool isEnabled;
    
    public:
        SkyboxShader(std::string folder, std::string ext);
        void loadUniforms(glm::mat4& P);
        void draw(glm::mat4 V, glm::vec3 viewPos);
		bool getIsEnabled();
		void setIsEnabled(bool b);
};