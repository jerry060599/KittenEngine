#version 430 core

#include "KittenEngine/shaders/kittenCommonFrag.glsl"

out vec4 fragColor;

void main() {
	fragColor = matColor;
}