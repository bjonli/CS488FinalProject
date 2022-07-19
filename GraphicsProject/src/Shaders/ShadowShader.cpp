#include "ShadowShader.hpp"
#include "../Application/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>

ShadowShader::ShadowShader(BatchInfoMap* batchInfoMap_, float wW, float wH, 
	bool enabled, bool transparencyEnabled_) : 
	SceneShader(batchInfoMap_, "Shadow.vs", "Shadow.fs"), 
	windowW(wW), windowH(wH), isEnabled(enabled), transparencyEnabled(transparencyEnabled_)
{}

void ShadowShader::initMeshData(const MeshConsolidator & meshConsolidator) {
    SceneShader::initMeshData(meshConsolidator);
    
    // initialize the texture
    glGenTextures(1, &directionalDepthMap);
    glBindTexture(GL_TEXTURE_2D, directionalDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 
             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // do not do GL_REPEAT, that copies the shadow once it is OOB
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    // initialize the depth buffers.
    glGenFramebuffers(1, &directionalDepthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthMapFBO);
    // now when we draw, it will update the depthMap
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
        directionalDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // update the perspective matrix
    dirLightPerspective = glm::ortho(-SHADOW_SIZE, SHADOW_SIZE, -SHADOW_SIZE, SHADOW_SIZE, 
        SHADOW_NEAR_PLANE, SHADOW_FAR_PLANE);
    loadUniforms(dirLightPerspective);
}

glm::mat4 ShadowShader::getDirLightPerspectiveMatrix() { return dirLightPerspective; }

bool ShadowShader::loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) {    
    // for now, translucent objects do not cast a shadow, so we do not draw
	if (geometryNode->materialType == MaterialType::Plain && 
		geometryNode->material.kd.a < 1 && transparencyEnabled) {
		return false;
	}
	return SceneShader::loadGeometryNodeData(geometryNode, fullT);
} 

glm::vec3 ShadowShader::getDirLightPosition(glm::vec3 viewPos, glm::vec3 lightDirection) {
    return lightDirection * (-10.0f) + viewPos;
}

void ShadowShader::drawScene(Scene* scene, glm::vec3 viewPos) {
	if (!isEnabled) { return;  }
    // change viewport to match the resolution of the depthmap
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    // bind frame buffer so when we draw, we update depth map instead
    glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthMapFBO);
    enable();
    {
        // clears the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // update uniforms
        glm::vec3 dirLightPos = getDirLightPosition(viewPos, scene->getDirectionalLight().direction);
        glm::mat4 dirLightView = glm::lookAt(dirLightPos, viewPos, glm::vec3(0, 1, 0));

        glCullFace(GL_FRONT);
        SceneShader::drawScene(scene, dirLightView, dirLightPos);
        glCullFace(GL_BACK);
    }
    disable();
    // bind to screen buffer, chnage back resolution, and reset depth/colour buffers
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
    glViewport(0, 0, windowW, windowH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLint ShadowShader::getDirectionalDepthMap() { return directionalDepthMap; }

void ShadowShader::setIsEnabled(bool b) { isEnabled = b;  }
void ShadowShader::setTransparencyEnabled(bool b) { transparencyEnabled = b;  }