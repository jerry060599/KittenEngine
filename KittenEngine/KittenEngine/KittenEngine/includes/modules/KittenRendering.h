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
	const int MAT_SHADOW = 6;
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
		vec4 col = vec4(1);
		vec3 dir = vec3(0, -1, 0);
		float radius = 0.05f;

		float param;
		int hasShadow = false;
		float spread = 0.5f;
		float focus = 0.3f;

		vec3 pos = vec3(0);
		int type = 0;

		mat4 shadowProj;
	} UBOLight;

	extern Material defMaterial;
	extern UBOLight ambientLight;
	extern mat4 projMat;
	extern mat4 viewMat;
	extern mat4 modelMat;
	extern vector<UBOLight> lights;
	extern int shadowRes;
	extern float shadowDist;

	extern Texture* defTexture;
	extern Texture* defCubemap;
	extern Mesh* defMesh;
	extern Shader* defBaseShader;
	extern Shader* defForwardShader;
	extern Shader* defUnlitShader;
	extern Shader* defEnvShader;

	void initRender();
	void startRender();
	void renderForward(Mesh* mesh, Shader* base, Shader* light = nullptr);
	void renderShadows(Mesh* mesh, Shader* base = nullptr);
	void renderEnv(Texture* cubemap);
};