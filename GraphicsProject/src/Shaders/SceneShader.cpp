#include "SceneShader.hpp"
#include "../Application/CS488Window.hpp"
#include "../Application/GlErrorCheck.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <string>
#include <algorithm>

SceneShader::SceneShader(BatchInfoMap* batchInfoMap, std::string vertexShader, std::string fragmentShader)
    : ShaderProgram(), batchInfoMap(batchInfoMap) 
{
    generateProgramObject();
	attachVertexShader( vertexShader );
	attachFragmentShader( fragmentShader );
	link();
}

void SceneShader::initMeshData(const MeshConsolidator & meshConsolidator) {
	// generate vertex array 
	glGenVertexArrays(1, &vao_meshData);
	glBindVertexArray(vao_meshData);
	
	// create buffer for storing positions
	GLuint vertexPositions;
	glGenBuffers(1, &vertexPositions);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositions);
	glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
		meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

	// Enable the vertex shader attribute location for "position" when rendering.
	GLint positionAttribLocation = getAttribLocation("position");
	glEnableVertexAttribArray(positionAttribLocation);
	// Tell GL how to map data from the vertex buffer "vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Restore defaults
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool SceneShader::loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) {
    //-- Set ModelView matrix:
    GLint location = getUniformLocation("Model");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(fullT));
	CHECK_GL_ERRORS;

	return true;
}

void SceneShader::drawSceneRecursive(glm::mat4 curT, SceneNode* node) {
    if (node == nullptr) { return; }
    // update transformation
	glm::mat4 fullT = curT*node->trans;

    // draw the mesh if it is a geometry node
    if (node->m_nodeType == NodeType::GeometryNode) {
		GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
        enable();
        { 
			if (loadGeometryNodeData(geometryNode, fullT)) {
				// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
				BatchInfo batchInfo = (*batchInfoMap)[geometryNode->meshId];
				glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			}
		}
		disable();
	}

    // iterate through the tree
    for (SceneNode* child : node->children) {
        drawSceneRecursive(fullT, child);
    }
}

void SceneShader::loadUniforms(glm::mat4& P) {
    enable();
    {
        // Perspective matrix
        glUniformMatrix4fv(getUniformLocation("Perspective"), 1, GL_FALSE, value_ptr(P));
        CHECK_GL_ERRORS;
    }
    disable();
}

void SceneShader::drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewPos) {    
    enable();
    {
        // view matrix
        glUniformMatrix4fv(getUniformLocation("View"), 1, GL_FALSE, value_ptr(V));
        CHECK_GL_ERRORS;
    }
    disable();

    glBindVertexArray(vao_meshData);
    drawSceneRecursive(glm::mat4(1.0), scene->getRoot());
    glBindVertexArray(0);
}

