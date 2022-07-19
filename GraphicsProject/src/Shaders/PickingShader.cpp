#include "PickingShader.hpp"
#include "../Application/GlErrorCheck.hpp"
#include <glm/gtc/type_ptr.hpp>

PickingShader::PickingShader(BatchInfoMap* batchInfoMap_)
    : SceneShader(batchInfoMap_, "Picking.vs", "Picking.fs")
{}

void PickingShader::initMeshData(const MeshConsolidator & meshConsolidator) {
    SceneShader::initMeshData(meshConsolidator);
}

bool PickingShader::loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) {
    //-- picking colour
    glm::vec3 c = idToColour(geometryNode->m_nodeId);
    glUniform3fv(getUniformLocation("colour"), 1, value_ptr(c));
    
    return SceneShader::loadGeometryNodeData(geometryNode, fullT);
} 

void PickingShader::loadUniforms(glm::mat4& P) {
    SceneShader::loadUniforms(P);
}

void PickingShader::drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewPos) {
    SceneShader::drawScene(scene, V, viewPos);
}

glm::vec3 PickingShader::idToColour(unsigned int id) {
    float r = float(id&0xff) / 255.0f;
    float g = float((id>>8)&0xff) / 255.0f;
    float b = float((id>>16)&0xff) / 255.0f;
    return glm::vec3(r, g, b);
}

unsigned int PickingShader::colourToId(GLubyte buffer[ 4 ]) {
    return buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
}