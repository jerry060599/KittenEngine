#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KittenAssets.h"

using namespace glm;

namespace Kitten {
	const int MAT_TEX0 = 0;
	const int MAT_TEX1 = 1;
	const int MAT_TEX2 = 2;
	const int MAT_TEX3 = 3;
	const int MAT_TEX4 = 4;
	const int MAT_TEX5 = 5;
	const int MAT_TEX6 = 6;
	const int MAT_CUBEMAP = 7;

	typedef struct UBOCommon {
		mat4 projMat;
		mat4 projMatInv;
		mat4 viewMat;
		mat4 viewMatInv;
		mat4 vpMat;
		mat4 vpMatInv;
		mat4 viewMat_n;
	} UBOCommon;

	typedef struct UBOModel {
		mat4 modelMat;
		mat4 modelMatInv;
		mat4 modelMat_n;
	} UBOModel;

	enum class KittenLight { AMBIENT, POINT, SPOT, DIR };

	typedef struct UBOLight {
		vec4 col;
		vec4 params;
		vec3 pos;
		int type;
	} UBOLight;

	extern Material defMaterial;
	extern UBOLight ambientLight;
	extern mat4 projMat;
	extern mat4 viewMat;
	extern mat4 modelMat;
	extern vector<UBOLight> lights;

	extern Texture* defTexture;
	extern Mesh* defMesh;

	void initRender();
	void startRender();
	void renderForward(Mesh* mesh, Shader* base, Shader* light = nullptr);
};