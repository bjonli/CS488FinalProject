#pragma once
#include "ClassicShader.hpp"
#include "../Application/MeshConsolidator.hpp"
#include "../Objects/Scene.hpp"

class PickingShader : public SceneShader {
    protected:
        bool loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) override;

    public:
        glm::vec3 idToColour(unsigned int id);
        unsigned int colourToId(GLubyte buffer[ 4 ]);

        PickingShader(BatchInfoMap* batchInfoMap_);
        virtual void initMeshData(const MeshConsolidator& MeshConsolidator) override;
        virtual void loadUniforms(glm::mat4& P) override;
        virtual void drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewPos) override;
};