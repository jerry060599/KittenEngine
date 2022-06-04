#pragma once
// Jerry Hsu, 2021

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KittenAssets.h"
#include "Rotor.h"
#include "glTempVar.h"


namespace Kitten {
	using namespace glm;

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

		float shadowBias = 0.0005f;
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
	extern vector<string> includePaths;
	extern int shadowRes;
	extern float shadowDist;

	extern Texture* defTexture;
	extern Texture* defCubemap;
	extern Mesh* defMesh, * defMeshPoly;
	extern Shader* defBaseShader;
	extern Shader* defForwardShader;
	extern Shader* defUnlitShader;
	extern Shader* defEnvShader;
	extern Shader* defBlitShader;

	extern ivec2 windowRes;

	double getTime();
	float getAspect();

	bool shouldClose();
	void checkErr(const char* tag = nullptr);
	void initRender();
	void startRender();
	void startFrame();
	void endFrame();
	void startRenderMesh(mat4 transform);
	void startRenderMaterial(Material* mat);
	void render(Mesh* mesh, Shader* base = nullptr);
	void renderLine(Mesh* mesh, Shader* base = nullptr);
	void renderInstanced(Mesh* mesh, int count, Shader* base = nullptr);
	void renderForward(Mesh* mesh, Shader* base, Shader* light = nullptr);
	void renderInstancedForward(Mesh* mesh, int count, Shader* base, Shader* light = nullptr);
	void renderShadows(Mesh* mesh, Shader* base = nullptr);
	void renderInstancedShadows(Mesh* mesh, int count, Shader* base = nullptr);
	void renderEnv(Texture* cubemap);
};