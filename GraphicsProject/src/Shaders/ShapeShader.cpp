#include "ShapeShader.hpp"
#include "../Application/GlErrorCheck.hpp"
#include "../Application/MathUtils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// number of points that make the circle
const int NUM_OPEN_CIRCLE_PTS = 48;
const int NUM_FILL_CIRCLE_TRIANGLES = 48;
const int NUM_OPEN_RECT_PTS = 4;
const int NUM_FILL_RECT_TRIANGLES = 2;


ShapeShader::ShapeShader(float wH, float wW) : ShaderProgram(), windowH(wH), windowW(wW)
{
    generateProgramObject();
	attachVertexShader( "Shape.vs" );
	attachFragmentShader( "Shape.fs" );
	link();
}

void ShapeShader::initData() {
    //-- Enable input slots for all shapes
	{
        glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Enable the vertex shader attribute location for "position" when rendering.
        GLint positionAttribLocation = getAttribLocation("position");
		glEnableVertexAttribArray(positionAttribLocation);
		CHECK_GL_ERRORS;

        // Generate VBO to store the shape
		GLuint vbo_shapes;
		glGenBuffers( 1, &vbo_shapes );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_shapes );

        int curIndex = 0;
        // 2 coords per point
        int totalNumPoints = NUM_OPEN_CIRCLE_PTS    // open circle 
            + 3*NUM_FILL_CIRCLE_TRIANGLES           // fill circle
            + NUM_OPEN_RECT_PTS                     // rectangle
            + 3*NUM_FILL_RECT_TRIANGLES;

        // load circle data into the shape array
        // fix it from 0 to 1
        openCircleIndex = curIndex;
        std::vector<glm::vec2> allPoints(totalNumPoints);
        float coef = 2.0*PI/NUM_OPEN_CIRCLE_PTS;
        for (int i=0; i<NUM_OPEN_CIRCLE_PTS; i++) {
            float ang = coef*float(i);
            allPoints[curIndex++] = glm::vec2(
                cos(ang)*0.5+0.5, sin(ang)*0.5+0.5); 
        }

        // fill circle
        fillCircleIndex = curIndex;
        coef = 2.0*PI/NUM_FILL_CIRCLE_TRIANGLES;
        for (int i=0; i<NUM_FILL_CIRCLE_TRIANGLES; i++) {
            float fromA = coef*float(i);
            float toA = coef*float(i+1);
            // point origin
            allPoints[curIndex++] = glm::vec2(0.5, 0.5);
            // point from
            allPoints[curIndex++] = glm::vec2(cos(fromA)*0.5+0.5, sin(fromA)*0.5+0.5);
            // point to
            allPoints[curIndex++] = glm::vec2(cos(toA)*0.5+0.5, sin(toA)*0.5+0.5);
        }

        // open rect  
        openRectIndex = curIndex;
        float openRectV[4][2] = {{ 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }};
        for (int i=0; i<4; i++) {
            allPoints[curIndex++] = glm::vec2(openRectV[i][0], openRectV[i][1]);
        }

        // fill rect
        fillRectIndex = curIndex;
        float fillRectV[6][2] = { 
            { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f },
            { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }           
        };
        for (int i=0; i<6; i++) {
            allPoints[curIndex++] = glm::vec2(fillRectV[i][0], fillRectV[i][1]);
        }

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*totalNumPoints, &allPoints[0], GL_STATIC_DRAW);
        // Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
        // "position" vertex attribute location for any bound vertex shader program.
        glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        enable();
        {
            GLint location = getUniformLocation("P");
            glm::mat4 P = glm::ortho(0.0f, windowW, 0.0f, windowH);
            glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P));
        }
        disable();
	}

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void ShapeShader::loadUniforms() {}

void ShapeShader::drawCircle(float centerX, float centerY, float r, glm::vec3 colour, bool fill) {
	glBindVertexArray(vao);
	enable();
	{
		float scale = 2*r;
		float xpos = centerX-r;
		float ypos = centerY-r;
		glm::mat4 tM = glm::translate(glm::mat4(1), glm::vec3(xpos, ypos, 0));
		glm::mat4 M = glm::scale(tM, glm::vec3(scale, scale, 1.0));

		glUniformMatrix4fv(getUniformLocation("M"), 1, GL_FALSE, value_ptr(M));
		glUniform3fv(getUniformLocation("colour"), 1, value_ptr(colour));
    
		if (fill) {        
			glDrawArrays(GL_TRIANGLES, fillCircleIndex, NUM_FILL_CIRCLE_TRIANGLES*3); 
		} else {
			glDrawArrays(GL_LINE_LOOP, openCircleIndex, NUM_OPEN_CIRCLE_PTS); 
		}
	}
	disable();
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void ShapeShader::drawRect(float left, float bot, float w, float h, glm::vec3 colour, bool fill) {
	glBindVertexArray(vao);
	enable();
	{
		glm::mat4 tM = glm::translate(glm::mat4(1), glm::vec3(left, bot, 0));
		glm::mat4 M = glm::scale(tM, glm::vec3(w, h, 1.0));  

		glUniformMatrix4fv(getUniformLocation("M"), 1, GL_FALSE, value_ptr(M));
		glUniform3fv(getUniformLocation("colour"), 1, value_ptr(colour));

		if (fill) {        
			glDrawArrays(GL_TRIANGLES, fillRectIndex, NUM_FILL_RECT_TRIANGLES*3); 
		} else {
			glDrawArrays(GL_LINE_LOOP, openRectIndex, NUM_OPEN_RECT_PTS); 
		}
	}
	disable();
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

