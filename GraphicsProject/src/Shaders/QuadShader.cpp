#include "QuadShader.hpp"
#include "../Application/CS488Window.hpp"
#include "../Application/GlErrorCheck.hpp"

QuadShader::QuadShader(ShadowShader* ss) {
    shadowShader = ss;
    generateProgramObject();
	attachVertexShader( "Quad.vs" );
	attachFragmentShader("Quad.fs" );
	link();
}

void QuadShader::draw()
{
    enable();
    {
        glUniform1f(getUniformLocation("near_plane"), SHADOW_NEAR_PLANE);
        glUniform1f(getUniformLocation("far_plane"), SHADOW_FAR_PLANE);
        glUniform1f(getUniformLocation("depthMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 
            shadowShader -> getDirectionalDepthMap());
        CHECK_GL_ERRORS;
    }


    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        CHECK_GL_ERRORS;

        GLint offsetLocation = getAttribLocation("aPos");
        glEnableVertexAttribArray(offsetLocation);
        glVertexAttribPointer(offsetLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        GLint texLocation = getAttribLocation("aTexCoords");
        glEnableVertexAttribArray(texLocation);
        glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        CHECK_GL_ERRORS;
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    disable();
    glBindVertexArray(0);
}