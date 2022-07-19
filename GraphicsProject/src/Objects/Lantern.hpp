#pragma once
#include <glm/glm.hpp>
#include "GeometryNode.hpp"
#include "../Shaders/ParticleShader.hpp"
#include "../SoundManager.hpp"
#include "../FlameManager.hpp"
#include "LightSource.hpp"

class Lantern : public GeometryNode {
    float radius;
    float maxRadius;
    bool activated;
    Flame flame;
	irrklang::ISound* flameSound;
	PointLight lanternLight;

	glm::vec3 getSoundDistance(glm::vec3 playerPos);

    public: 
        Lantern(std::string name, float maxR, AABB aabb_);
        void tick(float elapsedTime, ParticleShader* particleManager, glm::vec3 playerPos);
        bool getIsActivated();
        float getRadius();
        Flame getFlame();
        void extinguish(SoundManager* soundManager);
        void light(Flame f, SoundManager* soundManager, glm::vec3 playerPos);
        glm::vec3 getLightGlobalPos();
		PointLight* getLightSource();
};