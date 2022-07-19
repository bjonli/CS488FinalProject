#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

class TextureManager {
	std::map<std::string, GLuint> textures;

public:
	TextureManager();
	void assignTexture(std::string name, std::string file);
	GLuint getTextureId(std::string name);
};