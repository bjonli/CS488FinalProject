#pragma once

const double PI = 3.14159265;

float randRange(float lo, float hi);		// rand float in [lo, hi)
int randRangeInt(int lo, int hi);			// rand int in [lo, hi)

//---------------------------------------------------------------------------------------
template <typename T>
inline T degreesToRadians (
		T angle
) {
	return angle * T(PI) / T(180.0);
}

//---------------------------------------------------------------------------------------
template <typename T>
inline T radiansToDegrees (
		T angle
) {
	return angle * T(180.0) / T(PI);
}

