
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


// http://psgraphics.blogspot.com/2014/11/making-orthonormal-basis-from-unit.html
mat3 orthoBasisX(vec3 n) {
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

mat3 orthoBasisY(vec3 n) {
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

mat3 orthoBasisZ(vec3 n) {
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
