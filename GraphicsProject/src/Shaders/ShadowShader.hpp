#pragma once
#include "SceneShader.hpp"
#include "../Application/MeshConsolidator.hpp"
#include "../Objects/Scene.hpp"

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// we only calculate shadows around some distance about the user
// that distance is proportional to the SHADOW_SIZE
const float SHADOW_SIZE = 35.0f;
const float SHADOW_NEAR_PLANE = 0.1;
const float SHADOW_FAR_PLANE = 40;

class ShadowShader : public SceneShader {
    GLuint directionalDepthMap;
    GLuint directionalDepthMapFBO;

    float windowW, windowH;
    glm::mat4 dirLightPerspective;
	bool isEnabled;
	bool transparencyEnabled;

    protected:
        bool loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) override;

    public:
		ShadowShader(BatchInfoMap* batchInfoMap_, float wW, float wH,
			bool isEnabled, bool transparencyEnabled);
		virtual void initMeshData(const MeshConsolidator& MeshConsolidator) override;
		virtual void drawScene(Scene* scene, glm::vec3 viewPos);

		// Getters
        glm::mat4 getDirLightPerspectiveMatrix();
        glm::vec3 getDirLightPosition(glm::vec3 viewPos, glm::vec3 lightDirection);
        GLint getDirectionalDepthMap();

		// Setters
		void setIsEnabled(bool b);
		void setTransparencyEnabled(bool b);
};