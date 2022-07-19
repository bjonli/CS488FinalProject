#include "SoundManager.hpp"
#include "Application/CS488Window.hpp"

#include <iostream>

SoundManager::SoundManager(bool enabled) {
    soundEngine = irrklang::createIrrKlangDevice();

    startLanternSoundFile = 
        CS488Window::getAssetFilePath("Sounds/match_strike.wav");
	ambientLanternSoundFile =
		CS488Window::getAssetFilePath("Sounds/fire.flac");
	extinguishLanternSoundFile = 
		CS488Window::getAssetFilePath("Sounds/extinguish.wav");
	stepsSoundFile =
		CS488Window::getAssetFilePath("Sounds/steps.wav");

    if (!soundEngine) {
        std::cout << "Error loading sound manager" << std::endl;
    }

	setIsEnabled(enabled);
}   

SoundManager::~SoundManager() {
    delete soundEngine;
}

void SoundManager::setIsEnabled(bool b) {
	isEnabled = b;
	soundEngine->setSoundVolume(b);
}

bool SoundManager::getIsEnabled() { return isEnabled;  }

irrklang::ISound* SoundManager::startLanternSound(glm::vec3 pos) {
	// light the lantern sound
	soundEngine ->
		play2D(startLanternSoundFile.c_str(), 0, 0, false);
	// ambient fire sound
    return soundEngine -> 
        play3D(ambientLanternSoundFile.c_str(), 
            irrklang::vec3df(pos.x, pos.y, pos.z), true, 0, true);
}

void SoundManager::extinguishLanternSound() {
	soundEngine ->
		play2D(extinguishLanternSoundFile.c_str(), 0, 0, false);
}

irrklang::ISound* SoundManager::getWalkingSound() {
	// we want it started pause and looped
	return soundEngine ->
		play2D(stepsSoundFile.c_str(), 1, 1, true);
}