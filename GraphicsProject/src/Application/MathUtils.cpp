#include "MathUtils.hpp"
#include <cmath>

float randRange(float lo, float hi) {
	return ((hi - lo)*(float)rand() / RAND_MAX) + lo;
}

int randRangeInt(int lo, int hi) {
	return floor(randRange(lo, hi));
}