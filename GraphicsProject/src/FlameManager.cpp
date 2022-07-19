#include "FlameManager.hpp"

Flame::Flame() : outerColour(glm::vec4(1)), innerColour(glm::vec4(1)), 
	name("UNSELECTED"), id(-1), spriteId(0) {}

Flame::Flame(glm::vec4 oC, glm::vec4 iC, std::string n, int i, GLint sId)
    : outerColour(oC), innerColour(iC), name(n), id(i), spriteId(sId) {}

// CR-someday: read from file?
FlameManager::FlameManager(TextureManager* textureManager) {
    // init all the values. Assumes id = index of the flame
    Flame normal = Flame(
        glm::vec4(100/255.0, 40/255.0, 6/255.0, 1.0), 
        glm::vec4(104/255.0, 55/255.0, 6/255.0, 1.0),
        "Normal Flame",
        LANTERN_NORMAL,
		textureManager->getTextureId("FireIcon")
    );
    allFlames.push_back(normal);

    Flame grayscale = Flame(
        glm::vec4(10/255.0, 10/255.0, 10/255.0, 1.0), 
        glm::vec4(40/255.0, 40/255.0, 40/255.0, 1.0),
        "Gloomy Flame",
        LANTERN_GRAYSCALE,
		textureManager->getTextureId("CloudIcon")
    );
    allFlames.push_back(grayscale);

    Flame celShading = Flame(
        glm::vec4(100/255.0, 100/255.0, 8/255.0, 1.0), 
        glm::vec4(140/255.0, 140/255.0, 10/255.0, 1.0),
        "Happy Flame",
        LANTERN_CELSHADING,
		textureManager->getTextureId("SunIcon")
    );
    allFlames.push_back(celShading);

    Flame invert = Flame(
        glm::vec4(8/255.0, 128/255.0, 128/255.0, 1.0), 
        glm::vec4(40/255.0, 40/255.0, 70/255.0, 1.0),
        "Cold Flame",
        LANTERN_INVERT,
		textureManager->getTextureId("SnowIcon")
    );
    allFlames.push_back(invert);
}

Flame FlameManager::getFlameById(int id) {
	// wrap around if OOB
    if (id < 0) { return allFlames[allFlames.size()-1]; }
    if (id >= allFlames.size()) { return allFlames[0]; }
    return allFlames[id];
}

Flame FlameManager::getNextFlame(Flame f) { return getFlameById(f.id + 1); }
Flame FlameManager::getPrevFlame(Flame f) { return getFlameById(f.id - 1); }