#version 430 core

#include "kittenCommonFrag.glsl"
#include "kittenLighting.glsl"

in vec2 uv;
in vec3 norm;
in vec3 wPos;
out vec4 fragColor;

void main() {
	vec3 r = reflect(-getViewDir(wPos), normalize(norm));
	fragColor = vec4(getLightCol(), 1) * matColor * texture(tex_d, uv);// + 0.5 * texture(tex_env, r) * texture(tex_s, uv);
}