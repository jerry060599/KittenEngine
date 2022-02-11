#pragma once
#include "Common.h"
using namespace glm;

namespace Kitten {
	template<int dim = 3>
	struct Bound {
		vec<dim, float, defaultp> min;
		vec<dim, float, defaultp> max;

		inline vec<dim, float, defaultp> center() {
			return (min + max) * 0.5f;
		}

		inline Bound<dim> absorb(Bound<dim>& b) {
			return Bound<dim>{ glm::min(min, b.min), glm::max(max, b.max) };
		}

		inline Bound absorb(vec<dim, float, defaultp> b) {
			return Bound<dim>{ glm::min(min, b), glm::max(max, b) };
		}

		inline bool contains(vec<dim, float, defaultp> point) {
			return all(lessThanEqual(min, point)) && all(greaterThanEqual(max, point));
		}

		inline bool contains(Bound<dim>& b) {
			return all(lessThanEqual(min, b.min)) && all(greaterThanEqual(max, b.max));
		}

		inline bool intersects(Bound<dim>& b) {
			return !(any(lessThanEqual(max, b.min)) || any(greaterThanEqual(min, b.max)));
		}

		inline Bound<dim> pad(float padding) {
			return Bound<dim>{ min - vec<dim, float, defaultp>(padding), max + vec<dim, float, defaultp>(padding) };
		}

		inline float volume() {
			vec<dim, float, defaultp> diff = max - min;
			float v = diff.x;
			for (int i = 1; i < dim; i++) v *= diff[i];
			return v;
		}

		inline vec<dim, float, defaultp> normCoord(vec<dim, float, defaultp> pos) {
			return (pos - min) / (max - min);
		}

		inline vec<dim, float, defaultp> interp(vec<dim, float, defaultp> coord) {
			vec<dim, float, defaultp> pos;
			vec<dim, float, defaultp> diff = max - min;
			for (int i = 0; i < dim; i++)
				pos[i] = min[i] + diff[i] * coord[i];
			return pos;
		}
	};

	template<>
	struct Bound<1> {
		float min;
		float max;

		inline float center() {
			return (min + max) * 0.5f;
		}

		inline Bound<1> absorb(Bound<1>& b) {
			return Bound<1>{ glm::min(min, b.min), glm::max(max, b.max) };
		}

		inline Bound<1> absorb(float b) {
			return Bound<1>{ glm::min(min, b), glm::max(max, b) };
		}

		inline bool contains(float point) {
			return min <= point && point <= max;
		}

		inline bool contains(Bound<1>& b) {
			return min <= b.min && b.max <= max;
		}

		inline bool intersects(Bound<1>& b) {
			return max > b.min && min < b.max;
		}

		inline Bound<1> pad(float padding) {
			return Bound<1>{ min - padding, max + padding };
		}

		inline float volume() {
			return max - min;
		}

		inline float normCoord(float pos) {
			return (pos - min) / (max - min);
		}

		inline float interp(float coord) {
			return min + (max - min) * coord;
		}
	};

	typedef Bound<1> Range;
}