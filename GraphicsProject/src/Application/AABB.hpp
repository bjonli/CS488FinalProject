#pragma once

#include <glm/glm.hpp>

class AABB {
public:
	float minX, maxX, minY, maxY, minZ, maxZ;
	AABB();
	AABB(float x, float X, float y, float Y, float z, float Z);

	// returns true iff intersection
	bool intersect(const AABB& other);
	AABB transform(glm::mat4 m);
	void print();
};