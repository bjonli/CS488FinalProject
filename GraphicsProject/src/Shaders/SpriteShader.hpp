#pragma once

#include "ShaderProgram.hpp"

class SpriteShader : public ShaderProgram {
	GLuint vao;
	float windowH;
	float windowW;

public:
	SpriteShader(float wH, float wW);
	virtual void initData();
	virtual void loadUniforms();

	void draw(float left, float bot,
		float w, float h, GLuint texture);
};