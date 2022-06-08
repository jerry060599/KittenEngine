#pragma once

#include "Common.h"

namespace Kitten {
	struct Rotor {
		glm::vec3 q;	// The bivector part laid out in the { y^z, z^x, x^y } basis
		float w;		// The scaler part

		// Create a quaternion from an angle (in radians) and rotation axis
		KITTEN_FUNC_SPEC static Rotor angleAxis(float rad, glm::vec3 axis) {
			rad *= 0.5f;
			return Rotor(sin(rad) * axis, cos(rad));
		}

		// Create a quaternion from an angle (in degrees) and rotation axis
		KITTEN_FUNC_SPEC static Rotor angleAxisDeg(float deg, glm::vec3 axis) {
			deg *= 0.00872664625997165f;
			return Rotor(sin(deg) * axis, cos(deg));
		}

		// Create a quaternion from euler angles in radians
		KITTEN_FUNC_SPEC static Rotor eulerAngles(vec3 rad) {
			rad *= 0.5f;
			vec3 c = cos(rad);
			vec3 s = sin(rad);
			return Rotor(vec3(0, 0, s.z), c.z) * Rotor(vec3(0, s.y, 0), c.y) * Rotor(vec3(s.x, 0, 0), c.x);
		}

		// Create a quaternion from euler angles in radians
		KITTEN_FUNC_SPEC static Rotor eulerAngles(float x, float y, float z) {
			return eulerAngles(vec3(x, y, z));
		}

		// Create a quaternion from euler angles in degrees
		KITTEN_FUNC_SPEC static Rotor eulerAnglesDeg(vec3 deg) {
			return eulerAngles(deg * 0.0174532925199432958f);
		}

		// Create a quaternion from euler angles in degrees
		KITTEN_FUNC_SPEC static Rotor eulerAnglesDeg(float x, float y, float z) {
			return eulerAnglesDeg(vec3(x, y, z));
		}

		KITTEN_FUNC_SPEC static Rotor fromTo(vec3 from, vec3 to) {
			vec3 h = (from + to) / 2.f;
			float l = length2(h);
			if (l > 0) h *= inversesqrt(l);
			else h = orthoBasisX(from)[1];

			return Rotor(cross(from, h), dot(from, h));
		}

		// Returns the multiplicative identity rotor
		KITTEN_FUNC_SPEC static Rotor identity() {
			return Rotor();
		}

		KITTEN_FUNC_SPEC Rotor() : q(vec3(0)), w(1) {}

		KITTEN_FUNC_SPEC Rotor(glm::vec3 q, float w) : q(q), w(w) {}
		KITTEN_FUNC_SPEC Rotor(glm::vec4 v) : q(v), w(v.w) {}

		// Get the multiplicative inverse
		KITTEN_FUNC_SPEC Rotor inverse() const {
			return Rotor(-q, w);
		}

		KITTEN_FUNC_SPEC Rotor operator- () const {
			return inverse();
		}

		// Rotate a vector by this rotor
		KITTEN_FUNC_SPEC glm::vec3 rotate(glm::vec3 v) const {
			// Calculate v * ab
			vec3 a = w * v + cross(q, v);	// The vector
			float c = dot(v, q);			// The trivector

			// Calculate (w - q) * (a + c). Ignoring the scaler-trivector parts
			return w * a		// The scaler-vector product
				+ cross(q, a)	// The bivector-vector product
				+ c * q;		// The bivector-trivector product
		}

		KITTEN_FUNC_SPEC glm::mat3 matrix() {
			mat3 cm = crossMatrix(q);
			return abT(q, q) + mat3(w * w) + 2 * w * cm + cm * cm;
		}

		// Get the euler angle in radians
		KITTEN_FUNC_SPEC glm::vec3 euler() {
			return vec3(
				atan2(2 * (w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y)),
				asin(2 * (w * q.y - q.x * q.z)),
				atan2(2 * (w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))
			);
		}

		// Get the euler angle in degrees
		KITTEN_FUNC_SPEC glm::vec3 eulerDeg() {
			return euler() * 57.29577951308232f;
		}

		// Returns both the axis and rotation angle in radians
		KITTEN_FUNC_SPEC vec3 axis(float& angle) {
			float l = length(q);
			if (l == 0) {
				angle = 0;
				return vec3(1, 0, 0);
			}

			angle = 2 * atan2(l, w);
			return q / l;
		}

		// Returns the axis of rotation
		KITTEN_FUNC_SPEC vec3 axis() { float a; return axis(a); }

		// Returns both the axis and rotation angle in degrees
		KITTEN_FUNC_SPEC vec3 axisDeg(float& angle) {
			vec3 a = axis(angle);
			angle *= 57.29577951308232f;
			return a;
		}

		// Returns the angle of rotation in radians
		KITTEN_FUNC_SPEC float angle() { float a; axis(a); return a; }

		// Returns the angle of rotation in degrees
		KITTEN_FUNC_SPEC float angleDeg() { float a; axis(a); return a * 57.29577951308232f; }

		KITTEN_FUNC_SPEC friend glm::vec3 operator*(Rotor lhs, const glm::vec3& rhs) {
			return lhs.rotate(rhs);
		}

		KITTEN_FUNC_SPEC friend Rotor operator*(Rotor lhs, const Rotor& rhs) {
			return Rotor(lhs.w * rhs.q + rhs.w * lhs.q + cross(lhs.q, rhs.q),
				lhs.w * rhs.w - dot(lhs.q, rhs.q));
		}

		KITTEN_FUNC_SPEC friend Rotor operator*(float lhs, const Rotor& rhs) {
			if (rhs.w == 1) return Rotor();
			float na = lhs * acos(rhs.w);	// New angle
			float nw = cos(na);				// New cosine
			float s = sqrt((1 - nw * nw) / length2(rhs.q));
			if (fract(na * 0.1591549430918954) > 0.5) s = -s;
			return Rotor(rhs.q * s, nw);
		}

		// Gets the vec4 repersentation laid out in { y^z, z^x, x^y, scaler }
		KITTEN_FUNC_SPEC explicit operator glm::vec4() const {
			return vec4(q, w);
		}
	};

	// mix rotors a to b from t=[0, 1] (unclamped)
	KITTEN_FUNC_SPEC inline Rotor mix(Rotor a, Rotor b, float t) {
		return (t * (b * a.inverse())) * a;
	}
}