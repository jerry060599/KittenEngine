#pragma once
// Jerry Hsu, 2021

#include "Common.h"
using namespace glm;

namespace Kitten {
	template<int dim = 3, typename Real = float>
	struct Bound {
		vec<dim, Real, defaultp> min;
		vec<dim, Real, defaultp> max;

		Bound() {}
		Bound(vec<dim, Real, defaultp> center) : min(center), max(center) {}
		Bound(vec<dim, Real, defaultp> min, vec<dim, Real, defaultp> max) : min(min), max(max) {}

		inline vec<dim, Real, defaultp> center() {
			return (min + max) * 0.5f;
		}

		inline Bound<dim, Real> absorb(Bound<dim, Real>& b) {
			return Bound<dim, Real>{ glm::min(min, b.min), glm::max(max, b.max) };
		}

		inline Bound<dim, Real> absorb(vec<dim, Real, defaultp> b) {
			return Bound<dim, Real>{ glm::min(min, b), glm::max(max, b) };
		}

		inline bool contains(vec<dim, Real, defaultp> point) {
			return all(lessThanEqual(min, point)) && all(greaterThanEqual(max, point));
		}

		inline bool contains(Bound<dim, Real>& b) {
			return all(lessThanEqual(min, b.min)) && all(greaterThanEqual(max, b.max));
		}

		inline bool intersects(Bound<dim, Real>& b) {
			return !(any(lessThanEqual(max, b.min)) || any(greaterThanEqual(min, b.max)));
		}

		inline Bound<dim, Real> pad(Real padding) {
			return Bound<dim, Real>{ min - vec<dim, Real, defaultp>(padding), max + vec<dim, Real, defaultp>(padding) };
		}

		inline Real volume() {
			vec<dim, Real, defaultp> diff = max - min;
			Real v = diff.x;
			for (int i = 1; i < dim; i++) v *= diff[i];
			return v;
		}

		inline vec<dim, Real, defaultp> normCoord(vec<dim, Real, defaultp> pos) {
			return (pos - min) / (max - min);
		}

		inline vec<dim, Real, defaultp> interp(vec<dim, Real, defaultp> coord) {
			vec<dim, Real, defaultp> pos;
			vec<dim, Real, defaultp> diff = max - min;
			for (int i = 0; i < dim; i++)
				pos[i] = min[i] + diff[i] * coord[i];
			return pos;
		}
	};

	template<typename Real>
	struct Bound<1, Real> {
		Real min;
		Real max;

		Bound() {}
		Bound(Real center) : min(center), max(center) {}
		Bound(Real min, Real max) : min(min), max(max) {}

		inline Real center() {
			return (min + max) * 0.5f;
		}

		inline Bound<1, Real> absorb(Bound<1, Real>& b) {
			return Bound<1, Real>{ glm::min(min, b.min), glm::max(max, b.max) };
		}

		inline Bound<1, Real> absorb(Real b) {
			return Bound<1, Real>{ glm::min(min, b), glm::max(max, b) };
		}

		inline bool contains(Real point) {
			return min <= point && point <= max;
		}

		inline bool contains(Bound<1, Real>& b) {
			return min <= b.min && b.max <= max;
		}

		inline bool intersects(Bound<1, Real>& b) {
			return max > b.min && min < b.max;
		}

		inline Bound<1, Real> pad(Real padding) {
			return Bound<1, Real>{ min - padding, max + padding };
		}

		inline Real volume() {
			return max - min;
		}

		inline Real normCoord(Real pos) {
			return (pos - min) / (max - min);
		}

		inline Real interp(Real coord) {
			return min + (max - min) * coord;
		}
	};

	typedef Bound<1, float> Range;
}