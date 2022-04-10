#pragma once
// Jerry Hsu, 2021

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <functional>
#include "Timer.h"
#include "StopWatch.h"
#include <tuple>

using namespace glm;
using namespace std;

inline long getStringHash(string& str) {
	long h = 0;
	for (char c : str) h = h * 31 + c;
	return h;
}

inline mat4 TRSMat(vec3 t, float r, vec2 s) {
	mat4 mat(1);
	mat = rotate(mat, -r, vec3(0, 0, 1));
	mat = scale(mat, vec3(s, 1));
	mat = translate(mat, -t);
	return mat;
}

inline mat4 TRSMat(vec2 t, float r, vec2 s) {
	mat4 mat(1);
	mat = rotate(mat, -r, vec3(0, 0, 1));
	mat = scale(mat, vec3(s, 1));
	mat = translate(mat, vec3(-t, 0));
	return mat;
}

inline float cross(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

inline float length2(vec3 v) {
	return dot(v, v);
}

inline float length2(vec2 v) {
	return dot(v, v);
}

inline double pow2(double v) {
	return v * v;
}

inline int pow2(int v) {
	return v * v;
}

inline float pow2(float v) {
	return v * v;
}

inline float pow3(float v) {
	return v * v * v;
}

inline float pow4(float v) {
	v *= v;
	return v * v;
}

inline mat4 diag(vec4 d) {
	return mat4(
		d.x, 0, 0, 0,
		0, d.y, 0, 0,
		0, 0, d.z, 0,
		0, 0, 0, d.w
	);
}

inline mat3 diag(vec3 d) {
	return mat3(
		d.x, 0, 0,
		0, d.y, 0,
		0, 0, d.z
	);
}

inline mat2 diag(vec2 d) {
	return mat2(
		d.x, 0,
		0, d.y
	);
}

inline float min(vec2 v) {
	return glm::min(v.x, v.y);
}

inline float min(vec3 v) {
	return glm::min(v.x, glm::min(v.y, v.z));
}

inline float min(vec4 v) {
	return glm::min(glm::min(v.x, v.y), glm::min(v.z, v.w));
}

inline float max(vec2 v) {
	return glm::max(v.x, v.y);
}

inline float max(vec3 v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

inline float max(vec4 v) {
	return glm::max(glm::max(v.x, v.y), glm::max(v.z, v.w));
}

inline vec4 diag(mat4 m) {
	return vec4(m[0][0], m[1][1], m[2][2], m[3][3]);
}

inline vec3 diag(mat3 m) {
	return vec3(m[0][0], m[1][1], m[2][2]);
}

inline vec2 diag(mat2 m) {
	return vec2(m[0][0], m[1][1]);
}

inline float trace(mat4 m) {
	return m[0][0] + m[1][1] + m[2][2] + m[3][3];
}

inline float trace(mat3 m) {
	return m[0][0] + m[1][1] + m[2][2];
}

inline float trace(mat2 m) {
	return m[0][0] + m[1][1];
}

inline vec3 reflect(vec3 v, vec3 norm) {
	return v - 2 * dot(norm, v) * norm;
}

inline vec2 clampLen(vec2 v, float l) {
	float len = v.x * v.x + v.y * v.y;
	if (len > l * l)
		return v / sqrtf(len) * l;
	return v;
}

inline mat4 normalTransform(mat4 mat) {
	mat4 outMat = transpose(inverse(mat3(mat)));
	outMat[3] = vec4(0, 0, 0, 1);
	return outMat;
}

inline vec3 perturb(vec3 x, float h, int ind) {
	x[ind] += h;
	return x;
}

inline mat3 abT(vec3 a, vec3 b) {
	return mat3(
		b.x * a.x, b.y * a.x, b.z * a.x,
		b.x * a.y, b.y * a.y, b.z * a.y,
		b.x * a.z, b.y * a.z, b.z * a.z
	);
}

inline vec3 safeOrthonorm(vec3 v) {
	if (abs(v.y) == 1) return normalize(cross(vec3(1, 0, 0), v));
	return normalize(cross(vec3(0, 1, 0), v));
}

// http://psgraphics.blogspot.com/2014/11/making-orthonormal-basis-from-unit.html
inline mat3 orthoBasisX(vec3 n) {
	mat3 basis;
	basis[0] = n;
	if (n.z >= n.y) {
		const float a = 1.0f / (1.0f + n.z);
		const float b = -n.x * n.y * a;
		basis[1] = vec3(b, 1.0f - n.y * n.y * a, -n.y);
		basis[2] = -vec3(1.0f - n.x * n.x * a, b, -n.x);
	}
	else {
		const float a = 1.0f / (1.0f + n.y);
		const float b = -n.x * n.z * a;
		basis[1] = vec3(1.0f - n.x * n.x * a, -n.x, b);
		basis[2] = -vec3(b, -n.z, 1.0f - n.z * n.z * a);
	}
	return basis;
}

inline mat3 orthoBasisY(vec3 n) {
	mat3 basis;
	basis[1] = n;
	if (n.z >= n.y) {
		const float a = 1.0f / (1.0f + n.z);
		const float b = -n.x * n.y * a;
		basis[0] = vec3(b, 1.0f - n.y * n.y * a, -n.y);
		basis[2] = vec3(1.0f - n.x * n.x * a, b, -n.x);
	}
	else {
		const float a = 1.0f / (1.0f + n.y);
		const float b = -n.x * n.z * a;
		basis[0] = vec3(1.0f - n.x * n.x * a, -n.x, b);
		basis[2] = vec3(b, -n.z, 1.0f - n.z * n.z * a);
	}
	return basis;
}

inline mat3 orthoBasisZ(vec3 n) {
	mat3 basis;
	basis[2] = n;
	if (n.z >= n.y) {
		const float a = 1.0f / (1.0f + n.z);
		const float b = -n.x * n.y * a;
		basis[0] = vec3(1.0f - n.x * n.x * a, b, -n.x);
		basis[1] = vec3(b, 1.0f - n.y * n.y * a, -n.y);
	}
	else {
		const float a = 1.0f / (1.0f + n.y);
		const float b = -n.x * n.z * a;
		basis[0] = vec3(b, -n.z, 1.0f - n.z * n.z * a);
		basis[1] = vec3(1.0f - n.x * n.x * a, -n.x, b);
	}
	return basis;
}

inline mat4 rotateView(vec3 dir) {
	return mat4(transpose(orthoBasisZ(normalize(-dir))));
}

inline int numBatches(int n, int batchSize) {
	return (n - 1) / batchSize + 1;
}

inline int numBatches(size_t n, int batchSize) {
	return numBatches((int)n, batchSize);
}

// Returns the closest points between two lines in the form mix(a0, a1, uv.x) and mix(b0, b1, uv.y)
inline vec2 lineClosestPoints(vec3 a0, vec3 a1, vec3 b0, vec3 b1) {
	mat2x3 A(a1 - a0, b0 - b1);
	//return inverse(transpose(A) * A) * (transpose(A) * (b0 - a0));
	vec3 diff = b0 - a0;
	mat2 M(length2(A[0]), dot(A[0], A[1]), 0, length2(A[1]));
	M[1][0] = M[0][1];
	return inverse(M) * vec2(dot(A[0], diff), dot(A[1], diff));
}

inline vec2 lineInt(vec2 a0, vec2 a1, vec2 b0, vec2 b1) {
	vec2 d0 = a0 - a1;
	vec2 d1 = b1 - b0;

	float c = cross(d0, d1);
	return (vec2(d1.y, -d0.y) * (a0.x - b0.x) + vec2(-d1.x, d0.x) * (a0.y - b0.y)) / (c + 0.00001f * float(c == 0));
}

inline bool lineHasInt(vec2 s) {
	return s.x >= 0 && s.x <= 1 && s.y >= 0 && s.y <= 1;
}

inline int wrap(int i, int period) {
	return (i + period) % period;
}

// Returns the distance between a and b modulo len
inline int cyclicDist(int a, int b, int len) {
	return ((b - a) + len / 2 + len) % len - len / 2;
}

template <typename T>
T& slice(void* x, size_t index) {
	return ((T*)x)[index];
}

namespace std {
	namespace {

		// Code from boost
		// Reciprocal of the golden ratio helps spread entropy
		//     and handles duplicates.
		// See Mike Seymour in magic-numbers-in-boosthash-combine:
		//     http://stackoverflow.com/questions/4948780

		template <class T>
		inline void hash_combine(std::size_t& seed, T const& v) {
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl {
			static void apply(size_t& seed, Tuple const& tuple) {
				HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
				hash_combine(seed, std::get<Index>(tuple));
			}
		};

		template <class Tuple>
		struct HashValueImpl<Tuple, 0> {
			static void apply(size_t& seed, Tuple const& tuple) {
				hash_combine(seed, std::get<0>(tuple));
			}
		};
	}

	template <typename ... TT>
	struct hash<std::tuple<TT...>> {
		size_t
			operator()(std::tuple<TT...> const& tt) const {
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
			return seed;
		}
	};

	template <typename T1, typename T2>
	struct hash<std::pair<T1, T2>> {
		std::size_t operator() (const std::pair<T1, T2>& pair) const {
			size_t s = std::hash<T1>()(pair.first);
			size_t h = std::hash<T2>()(pair.second);
			return s ^ (h + 0x9e3779b9 + (s << 6) + (s >> 2));
		}
	};

	template <int N>
	struct hash<glm::vec<N, int, defaultp>> {
		std::size_t operator() (const glm::vec<N, int, defaultp>& v) const {
			size_t h = 0x9e3779b9;
			for (int i = 0; i < N; i++)
				h = v[i] ^ (h + 0x9e3779b9 + (v[i] << 6) + (v[i] >> 2));
			return h;
		}
	};

	template <int N>
	struct hash<glm::vec<N, bool, defaultp>> {
		std::size_t operator() (const glm::vec<N, int, defaultp>& v) const {
			size_t h = 0;
			for (int i = 0; i < N; i++)
				h = (h << 1) + v[i];
			return h;
		}
	};

	template <int N>
	struct hash<glm::vec<N, glm::i64, defaultp>> {
		std::size_t operator() (const glm::vec<N, int, defaultp>& v) const {
			size_t h = 0x9e3779b9;
			for (int i = 0; i < N; i++)
				h = v[i] ^ (h + 0x9e3779b9llu + (v[i] << 6) + (v[i] >> 2));
			return h;
		}
	};
}

template <int c, int r>
void print(mat<c, r, f32, defaultp> m, const char* format = "%.4f") {
	for (int i = 0; i < r; i++) {
		printf(i == 0 ? "{{" : " {");

		for (int j = 0; j < c; j++) {
			printf(format, m[j][i]);
			if (j != c - 1) printf(", ");
		}

		printf(i == r - 1 ? "}}\n" : "}\n");
	}
}

template <int s>
void print(vec<s, float, defaultp> v, const char* format = "%.4f") {
	printf("{");
	for (int i = 0; i < s; i++) {
		printf(format, v[i]);
		if (i != s - 1) printf(", ");
	}
	printf("}\n");
}

/// <summary>
/// Performs the element-wise multiplcation of a .* b
/// </summary>
/// <param name="a">a matrix</param>
/// <param name="b">b matrix</param>
/// <returns>a .* b</returns>
template <int s, typename T>
mat<s, s, T, defaultp> elemMul(mat<s, s, T, defaultp> a, mat<s, s, T, defaultp> b) {
	for (int i = 0; i < s; i++)
		a[i] *= b[i];
	return a;
}

/// <summary>
/// Performs the element-wise multiplcation of a .* b
/// </summary>
/// <param name="a">a vector</param>
/// <param name="b">b vector</param>
/// <returns>a .* b</returns>
template <int s, typename T>
vec<s, T, defaultp> elemMul(vec<s, T, defaultp> a, vec<s, T, defaultp> b) {
	return a * b;
}

template <int s, typename T>
T compSum(vec<s, T, defaultp> v) {
	T sum = v[0];
	for (int i = 1; i < s; i++)
		sum += v[i];
	return sum;
}

template <int s, typename T>
float compSum(mat<s, s, T, defaultp> a) {
	vec<s, T, defaultp> sum = a[0];
	for (int i = 1; i < s; i++)
		sum += a[i];
	return compSum(sum);
}

// Returns the bary-centric coords of the closest point to x on triangle p
inline vec3 baryCoord(const mat3& p, const vec3& x) {
	mat2x3 a(p[1] - p[0], p[2] - p[0]);
	vec3 e2 = x - p[0];

	mat2 m2(length2(a[0]), dot(a[0], a[1]), 0, length2(a[1]));
	m2[1][0] = m2[0][1];

	vec2 uv = inverse(m2) * vec2(dot(a[0], e2), dot(a[1], e2));

	return vec3(1 - uv.x - uv.y, uv.x, uv.y);
}

// Returns the bary-centric coords of the closest point to p[3] on triangle (p[0],p[1],p[2])
inline vec3 baryCoord(const mat4x3& p) {
	return baryCoord(*(mat3*)&p, p[3]);
}