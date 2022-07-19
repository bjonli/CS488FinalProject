#include "SpriteShader.hpp"
#include "../Application/GlErrorCheck.hpp"
#include "../Application/MathUtils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>

SpriteShader::SpriteShader(float wH, float wW) : ShaderProgram(),
	windowH(wH), windowW(wW)
{
	generateProgramObject();
	attachVertexShader("Sprite.vs");
	attachFragmentShader("Sprite.fs");
	link();
}

void SpriteShader::initData() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	std::vector<glm::vec2> posPoints = {
		glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
	};
	
	// Enable the vertex shader attribute location for "position" when rendering.
	GLint positionAttribLocation = getAttribLocation("position");
	glEnableVertexAttribArray(positionAttribLocation);
	// Generate VBO for positions
	GLuint vbo_positions;
	glGenBuffers(1, &vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*posPoints.size(), &posPoints[0], GL_STATIC_DRAW);
	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;

	// the y is inverted so that the sprite looks upright
	std::vector<glm::vec2> texPoints = {
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
	};

	// Generate VBO for textures
	GLuint vbo_textures;
	glGenBuffers(1, &vbo_textures);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_textures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*texPoints.size(), &texPoints[0], GL_STATIC_DRAW);
	// setup texutres now
	GLint textureAttribLocation = getAttribLocation("texCoords");
	glEnableVertexAttribArray(textureAttribLocation);
	glVertexAttribPointer(textureAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;

	// setup projection uniform
	enable();
	{
		GLint location = getUniformLocation("P");
		glm::mat4 P = glm::ortho(0.0f, windowW, 0.0f, windowH);
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P));

		glUniform1i(getUniformLocation("spriteTexture"), 0);
	}
	disable();

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void SpriteShader::loadUniforms() {}

void SpriteShader::draw(float left, float bot,
	float w, float h, GLuint textureId) {

	enable();
	{
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);

		// set the model transformation
		glm::mat4 tM = glm::translate(glm::mat4(1), glm::vec3(left, bot, 0));
		glm::mat4 M = glm::scale(tM, glm::vec3(w, h, 1.0));
		GLint location = getUniformLocation("M");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(M));

		// load the texture
		glBindTexture(GL_TEXTURE_2D, textureId);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	disable();
}