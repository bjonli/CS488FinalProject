#pragma once
#include "../SoundManager.hpp"
#include <glm/glm.hpp>

class Scene;					// forward declaration

// player can fly, ghost through objects, or walk
enum class PlayerMode {
	FLY,
	WALK,
	GHOST
};

class Player {
	PlayerMode mode;
	glm::vec3 position;
    float yawAngle;
    float pitchAngle;
	glm::vec3 velocity;

	bool isMovingLaterally;
	bool isInAir;
	irrklang::ISound* walkingSound;

	// helper functions
	bool tryMoveComponent(Scene* scene, glm::vec3 v);
	bool tryMoveComponentWithYLeniency(Scene* scene, glm::vec3 v);

    public:
        Player(glm::vec3 pos, float y, float p, PlayerMode m, SoundManager* soundManager);
		
		// setting velocity and orientation
		void jump();
        void turn(float deltaY, float deltaP);
		void setVelocity(float deltaForward, float deltaSide, float deltaUp);
		// for flying where the velocity is set every time and uses the step function
		void setVelocityDescrete(int forward, int side, int up);
		// specifically for walking where y velocity is preserved
		void setVelocityDescrete(int forward, int side);
		void setVelocity(float deltaForward, float deltaSide);

		// update position
		void tick(float elapsedTime);
		bool tryMove(float elapsedTime, Scene* scene);

		void togglePlayerMode(PlayerMode m);

		// getters
		PlayerMode getPlayerMode();
		glm::mat4 getViewMatrix();
        glm::vec3 getViewPos();

};