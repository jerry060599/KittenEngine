
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

mat3 orthoBasisZ(vec3 zNorm) {
	mat3 basis;
	basis[2] = zNorm;
	if (zNorm.z >= zNorm.y) {
		const float a = 1.0f / (1.0f + zNorm.z);
		const float b = -zNorm.x * zNorm.y * a;
		basis[0] = vec3(1.0f - zNorm.x * zNorm.x * a, b, -zNorm.x);
		basis[1] = vec3(b, 1.0f - zNorm.y * zNorm.y * a, -zNorm.y);
	}
	else {
		const float a = 1.0f / (1.0f + zNorm.y);
		const float b = -zNorm.x * zNorm.z * a;
		basis[0] = vec3(b, -zNorm.z, 1.0f - zNorm.z * zNorm.z * a);
		basis[1] = vec3(1.0f - zNorm.x * zNorm.x * a, -zNorm.x, b);
	}
	return basis;
}

mat3 orthoBasisY(vec3 yNorm) {
	mat3 basis;
	basis[1] = yNorm;
	if (yNorm.z >= yNorm.y) {
		const float a = 1.0f / (1.0f + yNorm.z);
		const float b = -yNorm.x * yNorm.y * a;
		basis[0] = vec3(1.0f - yNorm.x * yNorm.x * a, b, -yNorm.x);
		basis[2] = -vec3(b, 1.0f - yNorm.y * yNorm.y * a, -yNorm.y);
	}
	else {
		const float a = 1.0f / (1.0f + yNorm.y);
		const float b = -yNorm.x * yNorm.z * a;
		basis[0] = vec3(b, -yNorm.z, 1.0f - yNorm.z * yNorm.z * a);
		basis[2] = -vec3(1.0f - yNorm.x * yNorm.x * a, -yNorm.x, b);
	}
	return basis;
}
