#pragma once
#include "Common.h"
using namespace glm;

struct Bound {
	vec3 min;
	vec3 max;

	inline vec3 center() {
		return (min + max) * 0.5f;
	}

	inline Bound absorb(Bound& b) {
		return Bound{ glm::min(min, b.min), glm::max(max,b.max) };
	}

	inline Bound absorb(glm::vec3 b) {
		return Bound{ glm::min(min, b), glm::max(max,b) };
	}

	inline bool contains(vec3 point) {
		return all(lessThanEqual(min, point)) && all(greaterThanEqual(max, point));
	}

	inline bool contains(Bound& b) {
		return all(lessThanEqual(min, b.min)) && all(greaterThanEqual(max, b.max));
	}

	inline bool intersects(Bound& b) {
		return !(any(lessThanEqual(max, b.min)) || any(greaterThanEqual(min, b.max)));
	}

	inline Bound pad(float padding) {
		return Bound{ min - vec3(padding), max + vec3(padding) };
	}

	inline float volume() {
		vec3 diff = max - min;
		return diff.x * diff.y * diff.z;
	}
};

