#pragma once
#include "SceneShader.hpp"
#include "ShadowShader.hpp"
#include "../Application/MeshConsolidator.hpp"
#include "../Objects/Scene.hpp"
#include "../Objects/SceneNode.hpp"

// make sure these line up with the fragment shader
#define MATERIAL_PLAIN 0
#define MATERIAL_TEXTURE 1

// for sorting transparent objects from farthest to closest
struct TransparencySortData {
	float dist;
	GeometryNode* node;
	glm::mat4 fullT;
	TransparencySortData(GeometryNode* n, float d, glm::mat4 t)
		: dist(d), node(n), fullT(t) {}
	// sort such that farthest transparent objects are first
	bool operator<(TransparencySortData& other) const {
		return dist > other.dist;
	}
};

class ClassicShader : public SceneShader {
    ShadowShader* shadowShader;
	std::vector<TransparencySortData> transparentObjects;

	// state variables during drawing
	bool drawOnlyOpaque;
	glm::vec3 viewPos;

	// flag vars for enabling objectives
	bool texturesEnabled;
	bool transparencyEnabled;

    protected:
		// return true iff the node should be drawn
        bool loadGeometryNodeData(GeometryNode* geometryNode, glm::mat4& fullT) override;

    public:
        ClassicShader(BatchInfoMap* batchInfoMap_, ShadowShader* shadowShader_, 
			bool enabledTextures, bool enableTransparency);
        virtual void initMeshData(const MeshConsolidator& MeshConsolidator) override;
        virtual void loadUniforms(glm::mat4& P, bool shouldDrawShadows);
        virtual void drawScene(Scene* scene, glm::mat4 V, glm::vec3 viewPos) override;

		// GETTERS + SETTERS
		void setShadowsEnabled(bool b);
		void setTexturesEnabled(bool b);
		bool getTexturesEnabled();
		void setTransparencyEnabled(bool b);

};