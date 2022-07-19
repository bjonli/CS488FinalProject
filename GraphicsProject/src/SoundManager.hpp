#pragma once

#include <irrKlang/irrKlang.h>
#include <string>
#include <glm/glm.hpp>

class SoundManager {
    irrklang::ISoundEngine* soundEngine;
    std::string startLanternSoundFile;
	std::string ambientLanternSoundFile;
	std::string extinguishLanternSoundFile;
	std::string stepsSoundFile;
	bool isEnabled;

    public:
        SoundManager(bool enabled);
        ~SoundManager();

		// start a lantern sound from some position away
		// returns ambient sound instance of fire
        irrklang::ISound* startLanternSound(glm::vec3 pos);
		// plays extinguish sound
		void extinguishLanternSound();
		// returns ambient walking sound instance
		irrklang::ISound* getWalkingSound();

		bool getIsEnabled();
		void setIsEnabled(bool b);
};
