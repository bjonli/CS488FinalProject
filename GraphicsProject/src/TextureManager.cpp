#include "TextureManager.hpp"
#include "Application/CS488Window.hpp"
#include "stb_image.h"

#include <iostream>

unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		bool parsable = false;
		// infer data format by the number of channels
		switch (nrChannels) {
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
			std::cout << "Error: Unparsable Texture format " << path << std::endl;
		}

		if (parsable) {
			// add image to texture container
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

void TextureManager::assignTexture(std::string name, std::string file) {
	if (textures.find(name) == textures.end()) {
		textures[name] = loadTexture(
			CS488Window::getAssetFilePath(("Textures/" + file).c_str()).c_str()
		);
	}
}

TextureManager::TextureManager() {
	// assign all available textures
	assignTexture("Crate", "crate.jpg");
	assignTexture("TableWood", "table-wood.jpg");
	assignTexture("FloorWood", "floor-wood.jpg");
	assignTexture("WallWood", "wall-wood.jpg");
	assignTexture("Painting1", "painting.jpg");
	assignTexture("Stone", "rocks.png");
	assignTexture("A4", "a4.png");
	assignTexture("FireIcon", "fire.png");
	assignTexture("CloudIcon", "cloud.png");
	assignTexture("SnowIcon", "snowflake.png");
	assignTexture("SunIcon", "sun.png");
}

GLuint TextureManager::getTextureId(std::string name) {
	if (textures.find(name) == textures.end()) {
		std::cout << "Error: Cannot find texture with name " << name << std::endl;
	}
	return textures[name];
}