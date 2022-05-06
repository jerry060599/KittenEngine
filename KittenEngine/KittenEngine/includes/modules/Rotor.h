#include "Common.h"

namespace Kitten {
	struct Rotor {
		glm::vec3 q;	// The bivector part laid out in the { y^z, -x^z, x^y } basis
		float w;		// The scaler part

		inline void setFromOuter(glm::vec3 a, glm::vec3 b) {
			q = cross(a, b);
			q.y = -q.y;
			w = dot(a, b);
		}

		static Rotor outerProduct(glm::vec3 a, glm::vec3 b) {
			Rotor r;
			r.setFromOuter(a, b);
			return r;
		}

		static Rotor angleAxis(float rad, glm::vec3 axis) {
			mat3 basis = orthoBasisZ(axis);
			rad /= 2;
			return outerProduct(basis[1], cos(rad) * basis[1] + sin(rad) * basis[0]);
		}

		static Rotor eulerAngles(vec3 rad) {
			rad /= 2;
			vec3 c = cos(rad);
			vec3 s = sin(rad);
			return outerProduct(vec3(0, 1, 0), vec3(s.z, c.z, 0))
				* outerProduct(vec3(1, 0, 0), vec3(c.y, 0, s.y))
				* outerProduct(vec3(0, 0, 1), vec3(0, s.x, c.x));
		}

		Rotor() : q(vec3(0)), w(1) {}

		// The geometric product a*b
		Rotor(glm::vec3 a, glm::vec3 b) {
			setFromOuter(a, b);
		}

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

		// glm::vec3 euler() { }

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