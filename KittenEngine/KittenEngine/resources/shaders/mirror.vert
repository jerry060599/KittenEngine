#version 430 core

#include "resources/shaders/kittenCommonVert.glsl"

out vec3 wpos;

void main() {
    wpos = (viewMat * modelMat * vec4(vPos, 1)).xyz;
    gl_Position = vpMat * modelMat * vec4(vPos, 1);
}