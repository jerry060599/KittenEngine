#pragma once
const float episilon = 0.000001f;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
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