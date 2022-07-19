#include "ClassicShader.hpp"
#include "../Application/CS488Window.hpp"
#include "../Application/GlErrorCheck.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

const glm::vec4 DEFAULT_TEXTURE_KD = glm::vec4(1, 1, 1, 1);
const glm::vec3 DEFAULT_TEXTURE_KS = glm::vec3(0.5, 0.5, 0.5);
const float DEFAULT_TEXTURE_SHININESS = 10;
const glm::vec3 AMBIENT_INTENSITY(0.1, 0.1, 0.1);

ClassicShader::ClassicShader(BatchInfoMap* batchInfoMap_, ShadowShader* shadowShader_, bool enableTextures, bool enabledTransparency)
    : SceneShader(batchInfoMap_, "Phong.vs", "Phong.fs"), shadowShader(shadowShader_), texturesEnabled(enableTextures), 
	transparencyEnabled(enabledTransparency)
{}

void ClassicShader::initMeshData(const MeshConsolidator & meshConsolidator) {
    SceneShader::initMeshData(meshConsolidator);

	glBindVertexArray(vao_meshData);

	// Gen buffer for normals and load data
	GLuint vbo_vertexNormals;
	glGenBuffers(1, &vbo_vertexNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexNormals);
	glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
		meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);
	// Enable the vertex shader attribute location for "normal" when rendering.
	GLint normalAttribLocation = getAttribLocation("normal");
	glEnableVertexAttribArray(normalAttribLocation);
	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;

	// setup textures
	GLuint vbo_vertexTextures;
	glGenBuffers(1, &vbo_vertexTextures);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexTextures);
	glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumUVCoordBytes(),
		meshConsolidator.getUVCoordDataPtr(), GL_STATIC_DRAW);
	GLint textureAttribLocation = getAttribLocation("texCoords");
	glEnableVertexAttribArray(textureAttribLocation);
	glVertexAttribPointer(textureAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	CHECK_GL_ERRORS;

	// Restore defaults
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool ClassicShader::loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) {
	// set material
	if (geometryNode->materialType == MaterialType::Plain) {
		// we draw all the opaque objects first
		if (geometryNode->material.kd.a < 1 && drawOnlyOpaque) {
			float dist = glm::length2(glm::vec3(fullT[3])-viewPos);
			transparentObjects.push_back(TransparencySortData (geometryNode, dist, fullT));
			return false;
		}
		else {
			glm::vec4 kd = geometryNode->material.kd;							// diffuse
			if (!transparencyEnabled) { kd[3] = 1; }
			glUniform4fv(getUniformLocation("material.kd"), 1, value_ptr(kd));
			glm::vec3 ks = geometryNode->material.ks;							// specular
			glUniform3fv(getUniformLocation("material.ks"), 1, value_ptr(ks));
			float shininess = geometryNode->material.shininess;					// shininess
			glUniform1f(getUniformLocation("material.shininess"), shininess);
			glUniform1i(getUniformLocation("materialType"), MATERIAL_PLAIN);	// materialType
			CHECK_GL_ERRORS;
		}
	}
	// set texture
	else {
		if (texturesEnabled) {
			glUniform1i(getUniformLocation("colourTexture"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, geometryNode->textureId);
			glUniform1i(getUniformLocation("materialType"), MATERIAL_TEXTURE);
			CHECK_GL_ERRORS;
		}
		else {
			// default material if textures not enabled
			glUniform4fv(getUniformLocation("material.kd"), 1, value_ptr(DEFAULT_TEXTURE_KD));
			glUniform3fv(getUniformLocation("material.ks"), 1, value_ptr(DEFAULT_TEXTURE_KS));
			glUniform1f(getUniformLocation("material.shininess"), DEFAULT_TEXTURE_SHININESS);
			glUniform1i(getUniformLocation("materialType"), MATERIAL_PLAIN);
			CHECK_GL_ERRORS;
		}
	}

    return SceneShader::loadGeometryNodeData(geometryNode, fullT);
} 

// these we should not expect to change anytime soon
void ClassicShader::loadUniforms(glm::mat4& P, bool b) {
	setShadowsEnabled(b);
    enable();
    {
        glUniform3fv(getUniformLocation("ambientIntensity"), 1, value_ptr(AMBIENT_INTENSITY));
		glUniform1i(getUniformLocation("shadowsEnabled"), 1);
    }
    disable();
    SceneShader::loadUniforms(P);
}

void ClassicShader::setShadowsEnabled(bool b) {
	enable();
	{ glUniform1i(getUniformLocation("shadowsEnabled"), b); }
	disable();
}

void ClassicShader::setTexturesEnabled(bool b) { texturesEnabled = b; }

void ClassicShader::setTransparencyEnabled(bool b) { transparencyEnabled = b; }

bool ClassicShader::getTexturesEnabled() { return texturesEnabled;  }

void ClassicShader::drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewP) {
	viewPos = viewP;
    enable();
    {
        // light placements the same when rendering each node
		std::vector<PointLight*> lights = scene->getPointLights();
        int numLightsToDraw = std::min(10, (int)lights.size());
        for (int i=0; i<numLightsToDraw; i++) {
            std::string position = "pointLights[" + std::to_string(i) + "].position";
            GLint location = getUniformLocation(position.c_str());
            glUniform3fv(location, 1, value_ptr(lights[i] -> position));

            std::string intensity = "pointLights[" + std::to_string(i) + "].rgbIntensity";
            location = getUniformLocation(intensity.c_str());
            glUniform3fv(location, 1, value_ptr(lights[i] -> rgbIntensity));

            std::string attenuationVals = "pointLights[" + std::to_string(i) + "].attenuationVals";
            location = getUniformLocation(attenuationVals.c_str());
            glUniform3fv(location, 1, value_ptr(lights[i] -> attenuationVals));
        }
        GLint location = getUniformLocation("numPointLights");
        glUniform1i(location, numLightsToDraw);

        // directional light
        DirectionalLight dlight = scene -> getDirectionalLight();
        location = getUniformLocation("directionalLight.direction");
        glUniform3fv(location, 1, value_ptr(dlight.direction));
        location = getUniformLocation("directionalLight.rgbIntensity");
        glUniform3fv(location, 1, value_ptr(dlight.rgbIntensity));

        // setup shadow map calculations
        glm::vec3 dirLightPos = shadowShader->getDirLightPosition(viewPos, dlight.direction);
        glm::mat4 dirLightView = glm::lookAt(dirLightPos, viewPos, glm::vec3(0, 1, 0));
        glm::mat4 dirLightProjection = shadowShader->getDirLightPerspectiveMatrix();
        glm::mat4 dirLightSpaceMatrix = dirLightProjection*dirLightView;
        location = getUniformLocation("dirLightPos");
        glUniform3fv(location, 1, value_ptr(dirLightPos));
        location = getUniformLocation("dirLightSpaceMatrix");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(dirLightSpaceMatrix));
        
        glUniform1i(getUniformLocation("shadowMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowShader->getDirectionalDepthMap());

        // lantern placements
        std::vector<Lantern*> lanterns = scene->getLanterns();
        int numLanterns = std::min(10, (int)lanterns.size());;
        for (int i=0; i<numLanterns; i++) {
            std::string position = "lanterns[" + std::to_string(i) + "].position";
            GLint location = getUniformLocation(position.c_str());
            glm::vec3 globalPos = lanterns[i]->getLightGlobalPos();
            glUniform3fv(location, 1, value_ptr(globalPos));

            std::string radius = "lanterns[" + std::to_string(i) + "].radius";
            location = getUniformLocation(radius.c_str());
            glUniform1f(location, lanterns[i]->getRadius());

            std::string lanternType = "lanterns[" + std::to_string(i) + "].lanternType";
            location = getUniformLocation(lanternType.c_str());
            glUniform1i(location, lanterns[i]->getFlame().id);
        }
        location = getUniformLocation("numLanterns");
        glUniform1i(location, numLanterns);

        // view position
        location = getUniformLocation("viewPosition");
        glm::vec3 vP = viewPos;
        glUniform3fv(location, 1, value_ptr(vP));
        CHECK_GL_ERRORS;
    }
    disable();

	// only draw the opaque objects first
	transparentObjects.clear();
	drawOnlyOpaque = true;
    SceneShader::drawScene(scene, V, viewPos);
	drawOnlyOpaque = false;

	// draw the transparent objects
	sort(transparentObjects.begin(), transparentObjects.end());

	glBindVertexArray(vao_meshData);
	enable();
	for (int i = 0; i < transparentObjects.size(); i++) {
		GeometryNode* geometryNode = transparentObjects[i].node;
		if (loadGeometryNodeData(geometryNode, transparentObjects[i].fullT)) {
			// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
			BatchInfo batchInfo = (*batchInfoMap)[geometryNode->meshId];
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		}
	}
	disable();
	glBindVertexArray(0);
}
