#pragma once

#include <glm/glm.hpp>

// directional lights do not have attenuation, meant to be only one light
class DirectionalLight {
	public:
		glm::vec3 direction;
		glm::vec3 rgbIntensity;
		DirectionalLight(glm::vec3 direction_, glm::vec3 rgbIntensity_);
		DirectionalLight();
};

class PointLight {
	public:
		glm::vec3 position;
		glm::vec3 rgbIntensity;
		glm::vec3 attenuationVals;
		PointLight();
		PointLight(glm::vec3 position_, glm::vec3 rgbIntensity_, glm::vec3 attenuationVals);
};