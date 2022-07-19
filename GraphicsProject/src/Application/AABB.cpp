#include "AABB.hpp"
#include <iostream>

AABB::AABB(float x, float X, float y, float Y, float z, float Z):
	minX(x), maxX(X), minY(y), maxY(Y), minZ(z), maxZ(Z) {}

AABB::AABB() :
	minX(0), maxX(0), minY(0), maxY(0), minZ(0), maxZ(0) {}

AABB AABB::transform(glm::mat4 m) {
	// transform each point in the box
	glm::vec4 points[8] = {
		glm::vec4(minX, minY, minZ, 1),
		glm::vec4(minX, minY, maxZ, 1),
		glm::vec4(minX, maxY, minZ, 1),
		glm::vec4(minX, maxY, maxZ, 1),
		glm::vec4(maxX, minY, minZ, 1),
		glm::vec4(maxX, minY, maxZ, 1),
		glm::vec4(maxX, maxY, minZ, 1),
		glm::vec4(maxX, maxY, maxZ, 1)
	};

	glm::vec4 curMin = m * points[0];
	glm::vec4 curMax = curMin;
	for (int i = 1; i < 8; i++) {
		glm::vec4 res = m * points[i];
		// min and max should be component-wise
		curMin = glm::min(res, curMin);
		curMax = glm::max(res, curMax);
	}

	return AABB(
		curMin.x, curMax.x,
		curMin.y, curMax.y,
		curMin.z, curMax.z
	);
}

bool AABB::intersect(const AABB& other) {
	return (minX <= other.maxX && maxX >= other.minX) &&
		(minY <= other.maxY && maxY >= other.minY) &&
		(minZ <= other.maxZ && maxZ >= other.minZ);
}

void AABB::print() {
	std::cout <<
		"x[" << minX << ", " << maxX << "]," <<
		"y[" << minY << ", " << maxY << "]," <<
		"z[" << minZ << ", " << maxZ << "],";
}