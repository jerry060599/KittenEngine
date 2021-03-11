
#include "../includes/modules/KittenRendering.h"

using namespace glm;

namespace Kitten {
	mat4 projMat;
	mat4 viewMat;
	mat4 modelMat(1);

	Material defMaterial{ vec4(1), vec4(1), vec4(0) };

	vector<UBOLight> lights;
	UBOLight ambientLight = { vec4(0.04f, 0.04f, 0.08f, 1.f), vec4(0), vec3(0), (int)KittenLight::AMBIENT };

	unsigned int d_uboCommon = 0;
	unsigned int d_modelCommon = 0;
	unsigned int d_matCommon = 0;
	unsigned int d_lightCommon = 0;

	Texture* defTexture;
	Mesh* defMesh;

	bool initialized = false;

	void initRender() {
		if (initialized) return;
		initialized = true;

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

		glDepthFunc(GL_LEQUAL);

		{// Gen white texture for binding null textures
			unsigned int handle;
			glGenTextures(1, &handle);
			glBindTexture(GL_TEXTURE_2D, handle);

			unsigned int data = ~0;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			Texture* img = new Texture;
			img->glHandle = handle;
			img->width = 1;
			img->height = 1;
			img->channels = 4;
			img->ratio = 1;
			img->borders = ivec4(0);
			resources["white.tex"] = img;
			defTexture = img;
		}

		{// Gen standard quad for FBO bliting
			defMesh = new Mesh;
			defMesh->vertices.push_back({ vec3(0, 0, 0), vec3(0, 0, 1), vec2(0, 0) });
			defMesh->vertices.push_back({ vec3(1, 0, 0), vec3(0, 0, 1), vec2(1, 0) });
			defMesh->vertices.push_back({ vec3(1, 1, 0), vec3(0, 0, 1), vec2(1, 1) });
			defMesh->vertices.push_back({ vec3(0, 1, 0), vec3(0, 0, 1), vec2(0, 1) });
			defMesh->indices.push_back(3);
			defMesh->indices.push_back(0);
			defMesh->indices.push_back(1);
			defMesh->indices.push_back(3);
			defMesh->indices.push_back(1);
			defMesh->indices.push_back(2);

			defMesh->defMaterial = &defMaterial;

			defMesh->initGL();
			defMesh->upload();
			defMesh->calculateBounds();
		}
	}

	inline void uploadUniformBuff(unsigned int handle, void* data, size_t size) {
		glBindBuffer(GL_UNIFORM_BUFFER, handle);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void startRender() {
		initRender();

		UBOCommon uboCommon;

		uboCommon.projMat = projMat;
		uboCommon.projMatInv = inverse(projMat);
		uboCommon.viewMat = viewMat;
		uboCommon.viewMatInv = inverse(viewMat);

		uboCommon.viewMat_n = normalTransform(uboCommon.viewMat);
		uboCommon.vpMat = uboCommon.projMat * uboCommon.viewMat;
		uboCommon.vpMatInv = inverse(uboCommon.vpMat);

		uploadUniformBuff(d_uboCommon, &uboCommon, sizeof(UBOCommon));

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, d_uboCommon, 0, sizeof(UBOCommon));
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, d_modelCommon, 0, sizeof(UBOModel));
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, d_matCommon, 0, sizeof(UBOMat));
		glBindBufferRange(GL_UNIFORM_BUFFER, 3, d_lightCommon, 0, sizeof(UBOLight));
	}

	void uploadMat(Material* mat) {
		if (!mat) mat = &defMaterial;

		uploadUniformBuff(d_matCommon, mat, sizeof(UBOMat));
		for (size_t i = 0; i < MAT_CUBEMAP - 1; i++) {
			glActiveTexture((GLenum)(GL_TEXTURE0 + i));
			glBindTexture(GL_TEXTURE_2D, mat->texs[i] ? mat->texs[i]->glHandle : defTexture->glHandle);
		}
		if (mat->texs[MAT_CUBEMAP]) {
			glActiveTexture((GLenum)(GL_TEXTURE0 + MAT_CUBEMAP));
			glBindTexture(GL_TEXTURE_CUBE_MAP, mat->texs[MAT_CUBEMAP]->glHandle);
		}

	}

	void renderForward(Mesh* mesh, Shader* base, Shader* light) {
		UBOModel uboModel;
		uboModel.modelMat = modelMat * mesh->defTransform;
		uboModel.modelMatInv = inverse(uboModel.modelMat);
		uboModel.modelMat_n = normalTransform(uboModel.modelMat);

		uploadUniformBuff(d_modelCommon, &uboModel, sizeof(UBOModel));
		uploadMat(mesh->defMaterial);
		uploadUniformBuff(d_lightCommon, &ambientLight, sizeof(UBOLight));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(base->glHandle);
		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);

		if (light) {
			glBlendFunc(GL_ONE, GL_ONE);

			glUseProgram(light->glHandle);
			for (size_t i = 0; i < lights.size(); i++) {
				uploadUniformBuff(d_lightCommon, &lights[i], sizeof(UBOLight));
				glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}
}