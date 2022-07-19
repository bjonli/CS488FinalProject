#include "Player.hpp"
#include "../Application/MathUtils.hpp"
#include "Scene.hpp"
#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

// CONSTANTS
const float MAX_FORWARD_SPEED = 5;
const float MAX_BACK_SPEED = 5;
const float MAX_SIDE_SPEED = 5;
const float MAX_VERTICAL_SPEED = 5;
const float TURN_SENSITIVITY = 0.002;
const float HEIGHT = 2.5;
const float WIDTH = 1;
const glm::vec3 EYE_OFFSET = glm::vec3(0, 1, 0);			// distance from position
const float EPSILON = 0.0001;
const float Y_ACCELERATION = -13;	// accel due to gravity
const float JUMP_SPEED = 8;

// will walk up onto a higher surface if the gradient is low enough
float STEP_UP_LIMIT = 0.2;

Player::Player(glm::vec3 pos, float yA, float pA, PlayerMode m, SoundManager* soundManager)
	: position(pos), yawAngle(yA), pitchAngle(pA), mode(m),
	velocity(glm::vec3(0)) {
	walkingSound = soundManager->getWalkingSound();
	walkingSound->setVolume(0.5);
}

void Player::turn(float deltaY, float deltaP) {
    yawAngle += deltaY*TURN_SENSITIVITY;
    // make sure the angle is between -PI and PI
    if (yawAngle < -PI) { yawAngle += 2*PI; }
    if (yawAngle > PI) { yawAngle -= 2*PI; }
    // cap the pitch between -PI/2 to PI/2
    pitchAngle += deltaP*TURN_SENSITIVITY;
    pitchAngle = std::max(std::min((double)pitchAngle, PI*0.5), -0.5*PI);
}

void Player::setVelocity(float deltaForward, float deltaSide, float deltaUp) {
	// extract directions based on where player is facing
    glm::mat4 yawM = glm::rotate(glm::mat4(1), yawAngle, glm::vec3(0, 1, 0));
    glm::vec3 forwardDir = glm::vec3(yawM*glm::vec4(0, 0, -1, 0));
    glm::vec3 rightDir = glm::vec3(yawM*glm::vec4(1, 0, 0, 0));
    glm::vec3 upDir = glm::vec3(yawM*glm::vec4(0, 1, 0, 0));

	velocity = forwardDir*deltaForward + rightDir*deltaSide + upDir*deltaUp;
}

void Player::setVelocityDescrete(int forward, int side, int up) {
	float deltaForward = 0;
	if (forward > 0) { deltaForward = MAX_FORWARD_SPEED;  }
	else if (forward < 0) { deltaForward = -MAX_BACK_SPEED;  }
	float deltaSide = 0;
	if (side > 0) { deltaSide = MAX_SIDE_SPEED; }
	else if (side < 0) { deltaSide = -MAX_SIDE_SPEED; }
	float deltaUp = 0;
	if (up > 0) { deltaUp = MAX_VERTICAL_SPEED;  }
	else if (up < 0) { deltaUp = -MAX_VERTICAL_SPEED;  }

	setVelocity(deltaForward, deltaSide, deltaUp);
}

// no deltaUp given, preserves current y velocity
void Player::setVelocity(float deltaForward, float deltaSide) {
	setVelocity(deltaForward, deltaSide, velocity.y);
}

void Player::setVelocityDescrete(int forward, int side) {
	float deltaForward = 0;
	if (forward > 0) { deltaForward = MAX_FORWARD_SPEED; }
	else if (forward < 0) { deltaForward = -MAX_BACK_SPEED; }
	float deltaSide = 0;
	if (side > 0) { deltaSide = MAX_SIDE_SPEED; }
	else if (side < 0) { deltaSide = -MAX_SIDE_SPEED; }
	float deltaUp = 0;

	setVelocity(deltaForward, deltaSide);
}


bool Player::tryMoveComponent(Scene* scene, glm::vec3 v) {
	glm::vec3 newPos = position + v;
	AABB aabb(
		newPos.x - WIDTH * 0.5, newPos.x + WIDTH * 0.5,
		newPos.y - HEIGHT * 0.5, newPos.y + HEIGHT * 0.5,
		newPos.z - WIDTH * 0.5, newPos.z + WIDTH * 0.5
	);
	if (scene->checkIntersect(aabb) == nullptr) {
		position = newPos;
		return true;
	}
	return false;
}

bool Player::tryMoveComponentWithYLeniency(Scene* scene, glm::vec3 v) {
	glm::vec3 newPos = position + v;
	AABB aabb(
		newPos.x - WIDTH * 0.5, newPos.x + WIDTH * 0.5,
		newPos.y - HEIGHT * 0.5, newPos.y + HEIGHT * 0.5,
		newPos.z - WIDTH * 0.5, newPos.z + WIDTH * 0.5
	);
	GeometryNode* possibleIntersect = scene->checkIntersect(aabb);
	// if not interecting with any object, move is successful
	if (possibleIntersect == nullptr) {
		position = newPos;
		return true;
	}
	float yDiff = possibleIntersect->transformedAABB.maxY - aabb.minY + EPSILON;
	// if we cannot step over to the colliding object, we cannot move through it
	if (yDiff < 0 || yDiff > STEP_UP_LIMIT) {
		return false;
	}
	aabb.maxY += yDiff;
	aabb.minY += yDiff;
	GeometryNode* possibleStillIntersect = scene->checkIntersect(aabb);
	// if we can walk if we were to move slightly upward, then move slightly upward
	if (possibleStillIntersect == nullptr) {
		position = newPos + glm::vec3(0, yDiff, 0);
		return true;
	}
	return false;
}

void Player::tick(float elapsedTime) {
	// start walking
	bool isWalking = isMovingLaterally && !isInAir && mode == PlayerMode::WALK;
	// pause/continue walking sound instance if suddenly stopped/started walking
	if (isWalking && walkingSound->getIsPaused()) { walkingSound->setIsPaused(false); }
	else if (!isWalking && !walkingSound->getIsPaused()) { walkingSound->setIsPaused(true); }
}

bool Player::tryMove(float elapsedTime, Scene* scene) {
	bool hasMoved = false;
	// no collison detection if in ghost mode
	if (mode == PlayerMode::GHOST) {
		position += velocity * elapsedTime;
		hasMoved = true;
	}
	else if (mode == PlayerMode::FLY) {
		if (velocity.x != 0 && tryMoveComponentWithYLeniency(scene, glm::vec3(velocity.x*elapsedTime, 0, 0))) {
			hasMoved = true; 
		}
		if (velocity.z != 0 && tryMoveComponentWithYLeniency(scene, glm::vec3(0, 0, velocity.z*elapsedTime))) {
			hasMoved = true;
		}
		if (velocity.y != 0 && tryMoveComponent(scene, glm::vec3(0, velocity.y*elapsedTime, 0))) {
			hasMoved = true;
		}
	}
	else if (mode == PlayerMode::WALK) {
		isMovingLaterally = false;
		isInAir = false;
		if (velocity.x != 0 && tryMoveComponentWithYLeniency(scene, glm::vec3(velocity.x*elapsedTime, 0, 0))) {
			hasMoved = true;
			isMovingLaterally = true;
		}
		if (velocity.z != 0 && tryMoveComponentWithYLeniency(scene, glm::vec3(0, 0, velocity.z*elapsedTime))) {
			hasMoved = true;
			isMovingLaterally = true;
		}
		// update velocity based on acceleration
		velocity.y += Y_ACCELERATION * elapsedTime;
		if (tryMoveComponent(scene, glm::vec3(0, velocity.y*elapsedTime, 0))) {
			isInAir = true;
			hasMoved = true;
		} else {
			// hit the ground, reset the y value;
			isInAir = false;
			velocity.y = 0;
		}
	}
	return hasMoved;
}

glm::mat4 Player::getViewMatrix() {
    glm::mat4 pitchM = glm::rotate(glm::mat4(1), pitchAngle, glm::vec3(1, 0, 0));
    glm::mat4 yawM = glm::rotate(glm::mat4(1), yawAngle, glm::vec3(0, 1, 0));
    glm::mat4 transM = glm::translate(glm::mat4(1), getViewPos());
    return glm::inverse(transM*yawM*pitchM);
}

glm::vec3 Player::getViewPos() { return position + EYE_OFFSET; }

void Player::togglePlayerMode(PlayerMode m) { mode = m; }

PlayerMode Player::getPlayerMode() { return mode;  }

void Player::jump() {
	// cant jump if flying, a ghost, or in mid jump
	if (mode == PlayerMode::WALK && !isInAir) {
		velocity.y += JUMP_SPEED;
		isInAir = true;
	}
}