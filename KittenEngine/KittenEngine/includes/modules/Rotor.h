#pragma once

#include "Common.h"

namespace Kitten {
	struct Rotor {
		glm::vec3 q;	// The bivector part laid out in the { y^z, -x^z, x^y } basis
		float w;		// The scaler part

		static Rotor angleAxis(float rad, glm::vec3 axis) {
			rad /= 2;
			axis.y = -axis.y;
			return Rotor(sin(rad) * axis, cos(rad));
		}

		static Rotor eulerAngles(vec3 rad) {
			rad /= 2;
			vec3 c = cos(rad);
			vec3 s = sin(rad);
			return Rotor(vec3(0, 0, s.z), c.z) * Rotor(vec3(0, -s.y, 0), c.y) * Rotor(vec3(s.x, 0, 0), c.x);
		}

		Rotor() : q(vec3(0)), w(1) {}

		Rotor(glm::vec3 q, float w) : q(q), w(w) {}

		Rotor inverse() const {
			return Rotor(-q, w);
		}

		Rotor operator- () const {
			return inverse();
		}

		glm::vec3 rotate(glm::vec3 v) const {
			// Calculate v * ab
			vec3 a = w * v + cross(q, v);	// The vector
			float c = dot(v, q);			// The trivector

			// Calculate (w - q) * (a + c). Ignoring the scaler-trivector parts
			return w * a		// The scaler-vector product
				+ cross(q, a)	// The bivector-vector product
				+ c * q;		// The bivector-trivector product
		}

		glm::mat3 matrix() {
			mat3 cm = crossMatrix(q);
			return abT(q, q) + mat3(w * w) + 2 * w * cm + cm * cm;
		}

		glm::vec3 euler() {
			return vec3(
				atan2(2 * (w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y)),
				asin(2 * (q.x * q.z - w * q.y)),
				atan2(2 * (w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))
			);
		}

		friend glm::vec3 operator*(Rotor lhs, const glm::vec3& rhs) {
			return lhs.rotate(rhs);
		}

		friend Rotor operator*(Rotor lhs, const Rotor& rhs) {
			return Rotor(lhs.w * rhs.q + rhs.w * lhs.q + cross(lhs.q, rhs.q),
				lhs.w * rhs.w - dot(lhs.q, rhs.q));
		}

		friend Rotor operator*(float lhs, const Rotor& rhs) {
			if (rhs.w == 1) return Rotor();
			float na = lhs * acos(rhs.w);	// New angle
			float nw = cos(na);				// New cosine
			float s = sqrt((1 - nw * nw) / length2(rhs.q));
			if (fract(na * 0.1591549430918954) > 0.5) s = -s;
			return Rotor(rhs.q * s, nw);
		}

		explicit operator glm::vec4() const {
			return vec4(q, w);
		}
	};

	inline Rotor mix(Rotor a, Rotor b, float t) {
		return (t * (b * a.inverse())) * a;
	}
}