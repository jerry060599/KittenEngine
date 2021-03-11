#version 430 core

#include "resources/shaders/kittenCommonFrag.glsl"

in vec3 wpos;
out vec4 fragColor;

void main() {
	vec4 uv = projMat * vec4(wpos, 1);
	uv.xyz /= -uv.w;
	uv.xy = (uv.xy - 1) * 0.5;
	fragColor = texture(tex_d, uv.xy);
}