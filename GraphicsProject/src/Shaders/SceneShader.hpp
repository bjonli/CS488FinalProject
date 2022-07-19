#pragma once
#include "ShaderProgram.hpp"
#include "../Application/MeshConsolidator.hpp"
#include "../Objects/Scene.hpp"

// Base class of all shaders that require drawing the scene tree
class SceneShader : public ShaderProgram {
	// helper function
    void drawSceneRecursive(glm::mat4 curT, SceneNode* node);

    protected:
		GLuint vao_meshData;
		BatchInfoMap* batchInfoMap;
		// returns true iff the geometry node should be rendered
        virtual bool loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT);

    public:
        SceneShader(BatchInfoMap* batchInfoMap, std::string vertexShader, std::string fragmentShader);
        virtual void initMeshData(const MeshConsolidator& MeshConsolidator);
        virtual void loadUniforms(glm::mat4& P);
        virtual void drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewPos);
};