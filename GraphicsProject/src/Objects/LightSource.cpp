#include "LightSource.hpp"

DirectionalLight::DirectionalLight()
    : direction(glm::vec3(0, 0, 0)), rgbIntensity(glm::vec3(0, 0, 0)) {}

DirectionalLight::DirectionalLight(glm::vec3 direction_, glm::vec3 rgbIntensity_)
    : direction(direction_), rgbIntensity(rgbIntensity_) {}

PointLight::PointLight() : position(glm::vec3(0, 0, 0)),
	rgbIntensity(glm::vec3(0, 0, 0)), attenuationVals(glm::vec3(0, 0, 0)) {}

PointLight::PointLight(glm::vec3 position_, glm::vec3 rgbIntensity_, glm::vec3 attenuationVals_)
    : position(position_), rgbIntensity(rgbIntensity_), attenuationVals(attenuationVals_) {}