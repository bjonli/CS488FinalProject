#pragma once
#include "ShaderProgram.hpp"

class ShapeShader : public ShaderProgram {
	GLuint vao;
    float windowH;
    float windowW;

    int openCircleIndex;        // where in the buffer the open circle points are  
    int fillCircleIndex;
    int openRectIndex;
    int fillRectIndex;

    public:
        ShapeShader(float wH, float wW);
        virtual void initData();
        virtual void loadUniforms();

		void drawCircle(float centerX, float centerY,
			float r, glm::vec3 colour, bool fill);
		void drawRect(float left, float bot,
			float w, float h, glm::vec3 colour, bool fill);
};