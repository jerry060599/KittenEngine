
#include "../includes/modules/Mesh.h"
#include "../includes/modules/KittenAssets.h"
#include "../includes/modules/KittenRendering.h"
#include "../includes/modules/KittenPreprocessor.h"
#include <glad/glad.h> 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

namespace Kitten {
	unsigned int meshImportFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

	Mesh::~Mesh() {
		if (initialized) {
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
			glDeleteVertexArrays(1, &VAO);
		}
	}

	void Mesh::draw() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Mesh::initGL() {
		if (initialized) return;
		initialized = true;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
	}

	void Mesh::transform(mat4 mat) {
		mat3 mat_n = normalTransform(mat);

		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i].pos = mat * vec4(vertices[i].pos, 1.f);
			vertices[i].norm = mat_n * vertices[i].norm;
		}
	}

	void Mesh::upload() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, (GLsizei)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glBindVertexArray(0);
	}

	mat4 aiMat2GLM(aiMatrix4x4 mat) {
		mat4 glmMat = *((mat4*)&mat);
		return transpose(glmMat);
	}

	void Mesh::calculateBounds() {
		bounds = { vertices[0].pos, vertices[0].pos };
		for (size_t i = 0; i < vertices.size(); i++)
			bounds = bounds.absorb(vertices[i].pos);
	}

	void loadMeshFrom(std::filesystem::path path) {
		if (resources.count(path.string())) return;

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path.string(), meshImportFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}

		vector<aiNode*> processStack;
		vector<mat4> transformStack;
		processStack.push_back(scene->mRootNode);
		transformStack.push_back(mat4(1));

		Material** mats = new Material * [scene->mNumMaterials];
		for (size_t i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* aim = scene->mMaterials[i];
			Material* mat = new Material;
			string nName = path.string() + "\\materials\\" + aim->GetName().C_Str();
			printf("loading sub-material: %s\n", nName.c_str());
			resources[nName] = mat;
			mats[i] = mat;

			aiColor3D col;
			if (AI_SUCCESS == aim->Get(AI_MATKEY_COLOR_DIFFUSE, col))
				mat->props.col = vec4(col.r, col.g, col.b, 1);
			else
				mat->props.col = vec4(1);
			if (AI_SUCCESS == aim->Get(AI_MATKEY_COLOR_SPECULAR, col))
				mat->props.col1 = vec4(col.r, col.g, col.b, 1);
			else
				mat->props.col1 = vec4(1);

			if (AI_SUCCESS != aim->Get(AI_MATKEY_OPACITY, mat->props.col.a))
				mat->props.col.a = 1;
			if (AI_SUCCESS != aim->Get(AI_MATKEY_SHININESS, mat->props.params0.x)) // Bling expo
				mat->props.params0.x = 40;

			aiString str;
			if (AI_SUCCESS == aim->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), str)) {
				string p = path.parent_path().string() + "\\" + str.C_Str();
				loadTexture(p);
				mat->texs[0] = (Texture*)resources[p];
			}
			if (AI_SUCCESS == aim->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), str)) {
				string p = path.parent_path().string() + "\\" + str.C_Str();
				loadTexture(p);
				mat->texs[1] = (Texture*)resources[p];
			}
		}

		bool firstMesh = true;
		while (processStack.size()) {
			auto node = processStack.back();
			processStack.pop_back();
			mat4 transform = transformStack.back();
			transformStack.pop_back();

			if (node->mNumMeshes) {
				Mesh* mesh = new Mesh;
				string nName = path.string() + "\\" + node->mName.C_Str();
				resources[nName] = mesh;
				if (firstMesh) {
					resources[path.string()] = mesh;
					firstMesh = false;
					printf("loading sub-mesh: %s (\\%s)\n", path.string().c_str(), node->mName.C_Str());
				}
				else
					printf("loading sub-mesh: %s\n", nName.c_str());

				mesh->defTransform = transform;
				mat3 normMat = (mat3)normalTransform(transform);
				int matIndex = -1;
				mesh->groups.push_back(0);

				for (size_t i = 0; i < node->mNumMeshes; i++) {
					aiMesh* aim = scene->mMeshes[node->mMeshes[i]];
					const unsigned int startIndex = (unsigned int)mesh->vertices.size();
					if (aim->mMaterialIndex >= 0)
						if (matIndex < 0)
							matIndex = aim->mMaterialIndex;
						else
							printf("WARNING: Only one material supported per sub-mesh!\n");

					for (size_t j = 0; j < aim->mNumVertices; j++) {
						Vertex v;

						v.pos.x = aim->mVertices[j].x;
						v.pos.y = aim->mVertices[j].y;
						v.pos.z = aim->mVertices[j].z;
						v.pos = transform * vec4(v.pos, 1.f);

						if (aim->mNormals) {
							v.norm.x = aim->mNormals[j].x;
							v.norm.y = aim->mNormals[j].y;
							v.norm.z = aim->mNormals[j].z;
							v.norm = normMat * v.norm;
						}
						else v.norm = vec3(0);

						if (aim->mTextureCoords[0]) {
							v.uv.x = aim->mTextureCoords[0][j].x;
							v.uv.y = aim->mTextureCoords[0][j].y;
						}
						else v.uv = vec2(0);

						mesh->vertices.push_back(v);
					}

					for (size_t j = 0; j < aim->mNumFaces; j++) {
						aiFace face = aim->mFaces[j];
						for (unsigned int k = 0; k < face.mNumIndices; k++)
							mesh->indices.push_back(face.mIndices[k] + startIndex);
					}
					mesh->groups.push_back(mesh->indices.size());
				}

				if (matIndex >= 0)
					mesh->defMaterial = mats[matIndex];
				else
					mesh->defMaterial = &defMaterial;

				mesh->initGL();
				mesh->upload();
				mesh->calculateBounds();
			}

			for (size_t i = 0; i < node->mNumChildren; i++) {
				processStack.push_back(node->mChildren[i]);
				transformStack.push_back(transform * aiMat2GLM(node->mTransformation));
			}
		}

		delete[] mats;
	}

	Mesh* loadMeshExact(path path) {
		Mesh* mesh = new Mesh;
		std::ifstream input(path.string());

		string text = loadText(path.string());
		string line;

		while (std::getline(input, line, '\n')) {
			if (line[0] == 'v' && line[1] == ' ') {
				std::istringstream iss(line.substr(1));
				Vertex node{};
				iss >> node.pos.x >> node.pos.y >> node.pos.z;
				if (!iss) {
					delete mesh;
					return nullptr;
				}
				mesh->vertices.push_back(node);
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				if (!mesh->groups.size()) mesh->groups.push_back(0);

				std::istringstream iss(line.substr(1));
				int i;
				while (iss >> i) {
					if (i < 1)
						mesh->indices.push_back(mesh->vertices.size() + i);
					else
						mesh->indices.push_back(i - 1);
					iss.ignore(256, ' ');
				}
			}
			else if (line[0] == 'o' && line[1] == ' ')
				mesh->groups.push_back(mesh->indices.size());
		}
		mesh->groups.push_back(mesh->indices.size());

		mesh->initGL();
		mesh->upload();
		mesh->calculateBounds();
		return mesh;
	}
}