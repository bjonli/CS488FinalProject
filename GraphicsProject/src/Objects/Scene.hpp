#pragma once

#include "SceneNode.hpp"
#include "LightSource.hpp"
#include "Player.hpp"
#include "Lantern.hpp"
#include "../TextureManager.hpp"
#include "../Application/MeshConsolidator.hpp"

#include <vector>

class Scene {
    SceneNode* root;
    Player* player;						// assume one player

    // a scene will only have one directional light if any (from sky)
    DirectionalLight directionalLight;
    std::vector<PointLight*> pointLights;
    std::vector<Lantern*> lanterns;
    ParticleShader* particleManager;

	// for generating the scene
	TextureManager* textureManager;
	BatchInfoMap batchInfoMap;
	SceneNode* generateRoomScene();
	SceneNode* generateShapeScene();
	SceneNode* generateRocksScene();
	SceneNode* createNodeRotateMesh(
		std::string meshId, std::string name,
		glm::vec3 T, glm::vec3 S, float Ry);


    public:
        Scene(ParticleShader* pm);
        ~Scene();
        void generateScene(TextureManager* manager, BatchInfoMap& batchInfoMap, SoundManager* soundManager);
        
		void updateGlobalPos();
		void tick(float elapsedTime);
		GeometryNode* checkIntersect(AABB& other);

		// GETTERS
		SceneNode* getRoot();
        Player* getPlayer();
        DirectionalLight getDirectionalLight();
        std::vector<PointLight*> getPointLights();
        std::vector<Lantern*> getLanterns();
        SceneNode* getNodeWithId(unsigned int id);
};