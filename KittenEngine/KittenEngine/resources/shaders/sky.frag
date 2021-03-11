#version 430 core

#include "resources/shaders/kittenCommonFrag.glsl"

in vec3 dir;
out vec4 fragColor;

void main() {
	fragColor = texture(tex_env, dir);
}