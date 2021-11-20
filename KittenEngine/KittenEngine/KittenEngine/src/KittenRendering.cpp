
#include "../includes/modules/KittenRendering.h"
#include "../includes/modules/KittenPreprocessor.h"

using namespace glm;

namespace Kitten {
	mat4 projMat;
	mat4 viewMat;
	mat4 modelMat(1);

	Material defMaterial{ vec4(1), vec4(1), vec4(1), vec4(1) };

	vector<UBOLight> lights;
	UBOLight ambientLight = { vec4(0.04f, 0.04f, 0.08f, 1.f), vec3(0), 0, 0, 0, 0, 0, vec3(0), (int)KittenLight::AMBIENT };

	unsigned int d_uboCommon = 0;
	unsigned int d_modelCommon = 0;
	unsigned int d_matCommon = 0;
	unsigned int d_lightCommon = 0;
	int shadowRes = 2048;
	float shadowDist = 15.f;

	Texture* defTexture;
	Texture* defCubemap;
	Mesh* defMesh, * defMeshPoly;

	Shader* defBaseShader;
	Shader* defForwardShader;
	Shader* defUnlitShader;
	Shader* defEnvShader;
	Shader* defBlitShader;

	bool initialized = false;
	vector<FrameBuffer*> shadowMaps;

	void allocShadowMaps() {
		while (lights.size() > shadowMaps.size())
			shadowMaps.push_back(new FrameBuffer(shadowRes, shadowRes, 0));
		if (lights.size() < shadowMaps.size()) {
			for (size_t i = lights.size(); i < shadowMaps.size(); i++)
				delete shadowMaps[i];
			shadowMaps.resize(lights.size());
		}
		for (size_t i = 0; i < shadowMaps.size(); i++) {
			shadowMaps[i]->resize(shadowRes, shadowRes);
			mat4 proj(1);
			if (lights[i].type == (int)KittenLight::DIR)
				proj = glm::ortho(-shadowDist, shadowDist, -shadowDist, shadowDist, -shadowDist, shadowDist)
				* rotateView(lights[i].dir) * glm::translate(mat4(1), -vec3(viewMat[3]));
			else if (lights[i].type == (int)KittenLight::SPOT)
				proj = glm::perspective(2 * (180 / glm::pi<float>()) * glm::acos(lights[i].spread), 1.f, lights[i].radius, shadowDist)
				* rotateView(lights[i].dir) * glm::translate(mat4(1), -lights[i].pos);
			lights[i].shadowProj = proj;
		}
	}

	void clearShadowMaps() {
		for (auto fb : shadowMaps) {
			fb->bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			fb->unbind();
		}
	}

	void checkErr(const char* tag) {
		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
			if (tag)
				printf("GL error %d at %s\n", error, tag);
			else
				printf("GL error %d\n", error);
	}

	void initRender() {
		if (initialized) return;
		initialized = true;

		includePaths.push_back("KittenEngine\\shaders");

		loadDirectory("KittenEngine\\shaders");
		defBaseShader = get<Kitten::Shader>("KittenEngine\\shaders\\blingBase.glsl");
		defForwardShader = get<Kitten::Shader>("KittenEngine\\shaders\\blingForward.glsl");
		defUnlitShader = get<Kitten::Shader>("KittenEngine\\shaders\\unlit.glsl");
		defEnvShader = get<Kitten::Shader>("KittenEngine\\shaders\\env.glsl");
		defBlitShader = get<Kitten::Shader>("KittenEngine\\shaders\\blit.glsl");

		glGenBuffers(1, &d_uboCommon);
		glBindBuffer(GL_UNIFORM_BUFFER, d_uboCommon);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOCommon), NULL, GL_STATIC_DRAW);

		glGenBuffers(1, &d_modelCommon);
		glBindBuffer(GL_UNIFORM_BUFFER, d_modelCommon);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOModel), NULL, GL_STATIC_DRAW);

		glGenBuffers(1, &d_matCommon);
		glBindBuffer(GL_UNIFORM_BUFFER, d_matCommon);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOMat), NULL, GL_STATIC_DRAW);

		glGenBuffers(1, &d_lightCommon);
		glBindBuffer(GL_UNIFORM_BUFFER, d_lightCommon);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOLight), NULL, GL_STATIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);

		const float patchLvl[]{ 16, 16, 16, 16 };
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, patchLvl);
		glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, patchLvl);

		{// Gen white texture for binding null textures
			unsigned int handle;
			glGenTextures(1, &handle);
			glBindTexture(GL_TEXTURE_2D, handle);

			unsigned int data = ~0;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			Texture* img = new Texture;
			img->glHandle = handle;
			img->width = 1;
			img->height = 1;
			img->deviceFormat = GL_RGBA8;
			img->hostFormat = GL_RGBA;
			img->hostDataType = GL_UNSIGNED_BYTE;
			img->ratio = 1;
			img->borders = ivec4(0);
			img->rawData = new unsigned char[4];
			img->rawData[0] = ~0;
			img->rawData[1] = ~0;
			img->rawData[2] = ~0;
			img->rawData[3] = ~0;
			resources["white.tex"] = img;
			defTexture = img;
		}

		// Gen standard quad for FBO bliting
		defMesh = genQuadMesh(1, 1);
		defMeshPoly = genQuadMesh(1, 1);
		defMeshPoly->polygonize();

		defCubemap = new Texture(defTexture, defTexture, defTexture, defTexture, defTexture, defTexture);
	}

	inline void uploadUniformBuff(unsigned int handle, void* data, size_t size) {
		glBindBuffer(GL_UNIFORM_BUFFER, handle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void uploadUBOCommonBuff() {
		UBOCommon uboCommon;

		uboCommon.projMat = projMat;
		uboCommon.projMatInv = inverse(projMat);
		uboCommon.viewMat = viewMat;
		uboCommon.viewMatInv = inverse(viewMat);

		uboCommon.viewMat_n = normalTransform(uboCommon.viewMat);
		uboCommon.vpMat = uboCommon.projMat * uboCommon.viewMat;
		uboCommon.vpMatInv = inverse(uboCommon.vpMat);

		uploadUniformBuff(d_uboCommon, &uboCommon, sizeof(UBOCommon));
	}

	void startRender() {
		initRender();

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, d_uboCommon, 0, sizeof(UBOCommon));
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, d_modelCommon, 0, sizeof(UBOModel));
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, d_matCommon, 0, sizeof(UBOMat));
		glBindBufferRange(GL_UNIFORM_BUFFER, 3, d_lightCommon, 0, sizeof(UBOLight));

		ambientLight.type = (int)KittenLight::AMBIENT;
		uploadUBOCommonBuff();

		allocShadowMaps();
		clearShadowMaps();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);

		modelMat = mat4(1);
	}

	void startRenderMesh(mat4 transform) {
		UBOModel uboModel;
		uboModel.modelMat = modelMat * transform;
		uboModel.modelMatInv = inverse(uboModel.modelMat);
		uboModel.modelMat_n = normalTransform(uboModel.modelMat);
		uploadUniformBuff(d_modelCommon, &uboModel, sizeof(UBOModel));
	}

	void startRenderMaterial(Material* mat) {
		if (!mat) mat = &defMaterial;

		uploadUniformBuff(d_matCommon, mat, sizeof(UBOMat));
		for (size_t i = 0; i <= MAT_TEX5; i++) {
			glActiveTexture((GLenum)(GL_TEXTURE0 + i));
			glBindTexture(GL_TEXTURE_2D, mat->texs[i] ? mat->texs[i]->glHandle : defTexture->glHandle);
		}
		glActiveTexture((GLenum)(GL_TEXTURE0 + MAT_CUBEMAP));
		glBindTexture(GL_TEXTURE_CUBE_MAP, mat->texs[MAT_CUBEMAP] ? mat->texs[MAT_CUBEMAP]->glHandle : defCubemap->glHandle);
	}

	void render(Mesh* mesh, Shader* base) {
		renderForward(mesh, base);
	}

	void renderInstanced(Mesh* mesh, int count, Shader* base) {
		startRenderMesh(mesh->defTransform);
		startRenderMaterial(mesh->defMaterial);
		uploadUniformBuff(d_lightCommon, &ambientLight, sizeof(UBOLight));

		if (!base) base = defUnlitShader;
		base->use();
		glBindVertexArray(mesh->VAO);
		glDrawElementsInstanced(base->drawMode(), (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0, count);
	}

	void renderForward(Mesh* mesh, Shader* base, Shader* light) {
		startRenderMesh(mesh->defTransform);
		startRenderMaterial(mesh->defMaterial);
		uploadUniformBuff(d_lightCommon, &ambientLight, sizeof(UBOLight));

		if (!base) base = defUnlitShader;
		base->use();
		glBindVertexArray(mesh->VAO);
		glDrawElements(base->drawMode(), (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);

		if (light) {
			glBlendFunc(GL_ONE, GL_ONE);

			light->use();
			for (size_t i = 0; i < lights.size(); i++) {
				if (lights[i].type == (int)KittenLight::SPOT
					|| lights[i].type == (int)KittenLight::DIR) {
					glActiveTexture((GLenum)(GL_TEXTURE0 + MAT_SHADOW));
					glBindTexture(GL_TEXTURE_2D, shadowMaps[i]->depthStencil);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				}
				uploadUniformBuff(d_lightCommon, &lights[i], sizeof(UBOLight));

				glDrawElements(light->drawMode(), (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void renderShadows(Mesh* mesh, Shader* base) {
		startRenderMesh(mesh->defTransform);
		startRenderMaterial(mesh->defMaterial);

		mat4 oldView = viewMat;
		mat4 oldProj = projMat;
		viewMat = mat4(1);
		glDisable(GL_CULL_FACE);

		if (base == nullptr) base = defUnlitShader;
		base->use();
		glBindVertexArray(mesh->VAO);
		for (size_t i = 0; i < lights.size(); i++)
			if (lights[i].hasShadow
				&& (lights[i].type == (int)KittenLight::SPOT
					|| lights[i].type == (int)KittenLight::DIR)) {
				projMat = lights[i].shadowProj;
				uploadUBOCommonBuff();

				shadowMaps[i]->bind();
				glDrawElements(base->drawMode(), (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
				shadowMaps[i]->unbind();
			}

		viewMat = oldView;
		projMat = oldProj;
		uploadUBOCommonBuff();
		glEnable(GL_CULL_FACE);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void renderEnv(Texture* cubemap) {
		glActiveTexture((GLenum)(GL_TEXTURE0 + MAT_CUBEMAP));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->glHandle);
		defEnvShader->use();
		glBindVertexArray(defMesh->VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)defMesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}