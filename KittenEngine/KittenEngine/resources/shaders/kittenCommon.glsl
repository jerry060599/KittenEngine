
layout (binding = 0, std140) uniform globalUBO {
	mat4 projMat;
	mat4 projMatInv;
	mat4 viewMat;
	mat4 viewMatInv;
	mat4 vpMat;
	mat4 vpMatInv;
	mat4 viewMat_n;
};

layout (binding = 1, std140) uniform modelUBO {
	mat4 modelMat;
	mat4 modelMatInv;
	mat4 modelMat_n;
};

layout (binding = 2, std140) uniform materialUBO {
	vec4 matColor;
	vec4 matColor1;
	vec4 matParams0;
	vec4 matParams1;
};

const float pi = 3.14159265359;
const float e = 2.71828182846;

float cross(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

float length2(vec3 v) {
	return dot(v, v);
}

float length2(vec2 v) {
	return dot(v, v);
}

float pow2(float v) {
	return v * v;
}

float pow3(float v) {
	return v * v * v;
}

float pow4(float v) {
	v *= v;
	return v * v;
}

vec3 reflect(vec3 v, vec3 norm) {
	return v - 2 * dot(norm, v) * norm;
}

vec3 camWPos() {
	return vec3(viewMatInv[3]);
}

vec3 getViewDir(vec3 worldPos){
	return normalize(camWPos() - worldPos);
}