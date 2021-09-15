#pragma once
const float episilon = 0.000001f;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <functional>
#include "Timer.h"

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

inline mat4 rotateView(vec3 dir) {
	mat4 o(1);
	o[2] = vec4(-normalize(dir), 0.f);
	o[0] = vec4(safeOrthonorm(vec3(o[2])), 0.f);
	o[1] = vec4(cross(vec3(o[2]), vec3(o[0])), 0.f);
	return transpose(o);
}

inline int numBatches(int n, int batchSize) {
	return (n - 1) / batchSize + 1;
}

inline int numBatches(size_t n, int batchSize) {
	return numBatches((int)n, batchSize);
}

inline vec2 lineInt(vec2 a0, vec2 a1, vec2 b0, vec2 b1) {
	vec2 d0 = a0 - a1;
	vec2 d1 = b1 - b0;

	vec2 diff = a0 - b0;
	float c = cross(d0, d1);
	return (vec2(d1.y, -d0.y) * (a0.x - b0.x) + vec2(-d1.x, d0.x) * (a0.y - b0.y)) / (c + 0.00001f * float(c == 0));
}

inline bool lineHasInt(vec2 s) {
	return s.x >= 0 && s.x <= 1 && s.y >= 0 && s.y <= 1;
}
