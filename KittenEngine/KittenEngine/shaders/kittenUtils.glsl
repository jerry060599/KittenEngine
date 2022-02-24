
const float pi = 3.14159265359;
const float e = 2.71828182846;

float cross2d(vec2 a, vec2 b) {
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

vec3 hue2rgb(float h) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(h + K.xyz) * 6.0 - K.www);
    return clamp(p - K.xxx, 0.0, 1.0);
}