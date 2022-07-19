#include "Lantern.hpp"
#include <algorithm>
#include "../Application/MathUtils.hpp"

// all lanterns will have the same meshid (for now?)
const std::string MESH_ID = "lantern1";
const float GROW_RATE = 3;
const glm::vec3 LANTERN_LIGHT_OFFSET = glm::vec3(0, 1.8, 0);
const Material GRAY(glm::vec4(0.3, 0.3, 0.3, 1), glm::vec3(0.3, 0.3, 0.3), 10.0);
const glm::vec4 LANTERN_BORDER_COLOUR = glm::vec4(1, 1, 1, 1);
const glm::vec3 MAX_INTENSITY = glm::vec3(1, 1, 0.8);
const float TIME_TO_MAX_INTENSITY = 3;
const glm::vec3 DELTA_INTENSITY = MAX_INTENSITY / TIME_TO_MAX_INTENSITY;
const glm::vec3 LANTERN_ATTENUTATION_VALUES = glm::vec3(1, 0, 0.005);
// the larger the scale value, the faster the flame sound decreases in volume as you move farther away
const float SOUND_SCALE_VALUE = 1;	

Lantern::Lantern(std::string name, float maxR, AABB aabb_)
    : GeometryNode(MESH_ID, name, aabb_), maxRadius(maxR),
    activated(false), radius(0), flameSound(nullptr)
{
	// all lanterns same size, material, etc.
    scale(glm::vec3(0.3, 0.3, 0.3));
    objType = ObjectType::Lantern;
    material = GRAY;
	lanternLight.attenuationVals = LANTERN_ATTENUTATION_VALUES;
}

// GETTERS
bool Lantern::getIsActivated() { return activated; }
float Lantern::getRadius() { return radius; }
Flame Lantern::getFlame() { return flame; }
PointLight* Lantern::getLightSource() { return &lanternLight;  }

void Lantern::tick(float elapsedTime, ParticleShader* particleManager, glm::vec3 playerPos) {
    // move ROI up and down if activating/deactivating
    // normal flames do not have a ROI
    if (activated && radius < maxRadius && flame.id != LANTERN_NORMAL) {
        radius = std::min(maxRadius, (float)(radius + GROW_RATE * elapsedTime));
    } else if (!activated && radius > 0) {
        radius = std::max(0.0f, (float)(radius - GROW_RATE * elapsedTime));
    }

	// create border particles
    glm::vec3 lightPos = getLightGlobalPos();
    if (radius > 0.1) {
        for (int i=0; i<4; i++) {
            float x = randRange(-1, 1);
            float y = randRange(-1, 1);
            float z = randRange(-1, 1);
            glm::vec3 offSet = glm::normalize(glm::vec3(x, y, z))*radius;
            glm::vec3 v = glm::vec3(0, 1, 0)*randRange(0.01, 0.1);
            float s = randRange(0.005, 0.01);
            float l = randRange(2, 4);
            particleManager -> addParticle(lightPos+offSet, LANTERN_BORDER_COLOUR,
                glm::vec4(0), v, s, l);
        }
    }

    // create some flame particles
    if (activated) {
        // spawn an outer colour particle
        for (int i=0; i<5; i++) {
            float vX = randRange(-0.1, 0.1);
            float vZ = randRange(-0.1, 0.1);
            float speed = randRange(0.01, 1);
            glm::vec3 v = glm::normalize(glm::vec3(vX, 1, vZ))*speed;
            float r = randRange(0, 0.35);
            float theta = randRange(0, 2*PI);
            float y = randRange(0, 0.1);
            glm::vec3 offSet = glm::normalize(glm::vec3(sin(theta), y, cos(theta)))*r;
            glm::vec4 f = glm::vec4(0, 0, 0, randRange(-0.8, -0.4));
            float s = randRange(0.03, 0.05);
            float l = randRange(1, 1.5);
            particleManager -> addParticle(lightPos+offSet, flame.outerColour, f, v, s, l);
        }
        // spawn an inner colour particle
        for (int i=0; i<4; i++) {
            float vX = randRange(-0.06, 0.06);
            float vZ = randRange(-0.06, 0.06);
            float speed = randRange(0.01, 0.5);
            glm::vec3 v = glm::normalize(glm::vec3(vX, 1, vZ))*speed;
            float r = randRange(0, 0.3);
            float theta = randRange(0, 2*PI);
            float y = randRange(0, 0.1);
            glm::vec3 offSet = glm::normalize(glm::vec3(sin(theta), y, cos(theta)))*r;
            glm::vec4 f = glm::vec4(0, 0, 0, randRange(-0.8, -0.4));
            float l = randRange(1, 1.8);
            float s = randRange(0.03, 0.04);
            particleManager -> addParticle(lightPos+offSet, flame.innerColour, f, v, s, l);
        }
    }

	// update the volume of the ambient lantern flame sound based on the position of the player
	if (activated && flameSound != nullptr) {
		glm::vec3 dis = getSoundDistance(playerPos);
		flameSound->setPosition(irrklang::vec3df(
			dis.x, dis.y, dis.y
		));
	}

	// update the intensity/position of the flame
	if (activated) { lanternLight.rgbIntensity += DELTA_INTENSITY*elapsedTime;}
	else { lanternLight.rgbIntensity -= DELTA_INTENSITY*elapsedTime; }
	lanternLight.rgbIntensity = glm::clamp(lanternLight.rgbIntensity, glm::vec3(0, 0, 0), MAX_INTENSITY);
	lanternLight.position = getLightGlobalPos();
}

glm::vec3 Lantern::getSoundDistance(glm::vec3 playerPos) {
	return (getLightGlobalPos() - playerPos)*SOUND_SCALE_VALUE;
}

void Lantern::extinguish(SoundManager* soundManager) {
    activated = false;
	if (flameSound != nullptr) {
		flameSound -> stop();
		flameSound = nullptr;
		soundManager->extinguishLanternSound();
	}
}

void Lantern::light(Flame f, SoundManager* soundManager, glm::vec3 playerPos) {
    // do not light while it is extinguishing
    if (radius <= 0.001) {
        activated = true;
        flame = f;
		flameSound = soundManager->startLanternSound(getSoundDistance(playerPos));
    }
}

glm::vec3 Lantern::getLightGlobalPos() {
    // CR-someday: may not be true if rotated (for now, assume it isnt)
    // perhaps model as another SceneNode
    return getGlobalPos() + LANTERN_LIGHT_OFFSET;
}