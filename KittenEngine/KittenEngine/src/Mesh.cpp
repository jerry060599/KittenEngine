
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
#include <unordered_set>

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

	void Mesh::setFromLine(vector<vec3>& points) {
		if (points.size() < 2) {
			vertices.clear();
			indices.clear();
			return;
		}

		vertices.resize(points.size());
		for (size_t i = 0; i < points.size(); i++)
			vertices[i] = { points[i] };

		indices.resize(2 * points.size() - 2);
		for (size_t i = 0; i < points.size() - 1; i++) {
			indices[2 * i] = i;
			indices[2 * i + 1] = i + 1;
		}
	}

	void Mesh::polygonize() {
		auto v = vertices;
		vertices.clear();
		for (size_t i = 0; i < indices.size(); i++) {
			vertices.push_back(v[indices[i]]);
			indices[i] = (int)vertices.size() - 1;
		}

		upload();
	}

	void Mesh::transform(mat4 mat) {
		mat3 mat_n = normalTransform(mat);

		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i].pos = mat * vec4(vertices[i].pos, 1.f);
			vertices[i].norm = mat_n * vertices[i].norm;
		}
	}

	void Mesh::upload() {
		initGL();
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

	void Mesh::writeOBJ(string p) {
		FILE* file;
		fopen_s(&file, p.c_str(), "w");
		if (file) {
			fprintf(file, "# WaveFront *.obj file\n\n");

			for (size_t i = 0; i < vertices.size(); i++) {
				vec3 v = vertices[i].pos;
				fprintf(file, "v %.16f %.16f %.16f\n", v.x, v.y, v.z);
			}

			fprintf(file, "# %zd vertices\n\no mesh\n", vertices.size());
			for (size_t i = 0; i < indices.size() / 3; i++) {
				ivec3 v = ivec3(
					indices[3 * i + 0],
					indices[3 * i + 1],
					indices[3 * i + 2]
				) + 1;
				fprintf(file, "f %d %d %d\n", v.x, v.y, v.z);
			}
			fprintf(file, "# %zd triangles\n\n", indices.size() / 3);

			fclose(file);
		}
	}


	void Mesh::writePOLY(string p) {
		FILE* file;
		fopen_s(&file, p.c_str(), "w");
		if (file) {
			fprintf(file, "%zd 3 0 0\n", vertices.size());

			for (size_t i = 0; i < vertices.size(); i++) {
				vec3 v = vertices[i].pos;
				fprintf(file, "%zd %.16f %.16f %.16f\n", i + 1, v.x, v.y, v.z);
			}
			fprintf(file, "%zd 0\n", indices.size() / 3);

			for (size_t i = 0; i < indices.size() / 3; i++) {
				ivec3 v = ivec3(
					indices[3 * i + 0],
					indices[3 * i + 1],
					indices[3 * i + 2]
				) + 1;
				fprintf(file, "1\n3 %d %d %d\n", v.x, v.y, v.z);
			}
			fprintf(file, "0\n0\n");

			fclose(file);
		}
	}

	void loadMeshFrom(std::filesystem::path path) {
		if (resources.count(path.string())) return;

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path.string(), meshImportFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "err: assimp error " << import.GetErrorString() << endl;
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
			printf("asset: loading sub-material %s\n", nName.c_str());
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
					printf("asset: loading sub-mesh %s (\\%s)\n", path.string().c_str(), node->mName.C_Str());
				}
				else
					printf("asset: loading sub-mesh %s\n", nName.c_str());

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
							printf("err: only one material supported per sub-mesh!\n");

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

	void loadTetgenFrom(path path) {
		string name = path.string().substr(0, path.string().size() - path.extension().string().size()) + ".tetgen";
		printf("asset: loading tetgen-mesh %s (.tetgen)\n", path.string().c_str());

		auto itr = resources.find(name);
		TetMesh* mesh;
		if (itr != resources.end())
			mesh = (TetMesh*)itr->second;
		else {
			mesh = new TetMesh;
			mesh->defMaterial = nullptr;
			mesh->defTransform = mat4(1);
			resources[name] = mesh;
		}
		resources[path.string()] = mesh;

		string ext = path.extension().string();
		if (ext == ".node") {
			std::ifstream input(path.string());
			int nV = 0, dim, marker, attributes;

			{
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);
				iss >> nV >> dim >> marker >> attributes;
			}

			// printf("%d %d %d %d\n", nV, dim, marker, attributes);
			for (size_t i = 0; i < nV; i++) {
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);

				Vertex node{};
				int v;
				iss >> v >> node.pos.x >> node.pos.y >> node.pos.z;
				mesh->vertices.push_back(node);
				// printf("v %f %f %f\n", node.pos.x, node.pos.y, node.pos.z);
			}

			input.close();
			mesh->initGL();
			mesh->upload();
			mesh->calculateBounds();
		}
		else if (ext == ".face") {
			std::ifstream input(path.string());

			string line;
			int nV = 0, marker;
			{
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);
				iss >> nV >> marker;
			}

			for (size_t i = 0; i < nV; i++) {
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);

				ivec3 tri;
				int v, m;
				iss >> v >> tri.x >> tri.y >> tri.z >> m;
				mesh->indices.push_back(tri.x - 1);
				mesh->indices.push_back(tri.y - 1);
				mesh->indices.push_back(tri.z - 1);
				// printf("f %d %d %d\n", tri.x - 1, tri.y - 1, tri.z - 1);
			}

			input.close();

			mesh->groups.push_back(mesh->indices.size());

			mesh->initGL();
			mesh->upload();
		}
		else if (ext == ".ele") {
			std::ifstream input(path.string());

			string line;
			int nV = 0, nN, attribute;
			{
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);
				iss >> nV >> nN >> attribute;
			}

			for (size_t i = 0; i < nV; i++) {
				string line;
				std::getline(input, line, '\n');
				std::istringstream iss(line);

				ivec4 tri;
				int v;
				iss >> v >> tri.x >> tri.y >> tri.z >> tri.w;
				mesh->tetIndices.push_back(tri.x - 1);
				mesh->tetIndices.push_back(tri.y - 1);
				mesh->tetIndices.push_back(tri.z - 1);
				mesh->tetIndices.push_back(tri.w - 1);
				// printf("e %d %d %d %d\n", tri.x - 1, tri.y - 1, tri.z - 1, tri.w - 1);
			}

			input.close();
			mesh->initGL();
			mesh->upload();
		}
	}

	Mesh* genQuadMesh(int rows, int cols) {
		Mesh* mesh = new Mesh;

		mesh->vertices.reserve((rows + 1) * (cols + 1));
		for (int r = 0; r <= rows; r++)
			for (int c = 0; c <= cols; c++) {
				vec2 p = vec2(c / (float)cols, r / (float)rows);
				mesh->vertices.push_back({ vec3(p.x, p.y, 0), vec3(0, 0, 1), p });
			}

		mesh->indices.reserve(6 * rows * cols);
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < cols; c++) {
				const int idx = r * (cols + 1) + c;
				mesh->indices.push_back(idx);
				mesh->indices.push_back(idx + 1);
				mesh->indices.push_back(idx + cols + 1);
				mesh->indices.push_back(idx + 1);
				mesh->indices.push_back(idx + cols + 2);
				mesh->indices.push_back(idx + cols + 1);
			}

		mesh->defMaterial = nullptr;
		mesh->defTransform = mat4(1);
		mesh->initGL();
		mesh->upload();
		mesh->calculateBounds();
		return mesh;
	}

	Mesh* loadMeshExact(path path) {
		Mesh* mesh = new Mesh;
		std::ifstream input(path.string());

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
						mesh->indices.push_back((unsigned int)mesh->vertices.size() + i);
					else
						mesh->indices.push_back(i - 1);
					iss.ignore(256, ' ');
				}
			}
			else if (line[0] == 'o' && line[1] == ' ')
				mesh->groups.push_back(mesh->indices.size());
		}
		mesh->groups.push_back(mesh->indices.size());

		mesh->defMaterial = nullptr;
		mesh->defTransform = mat4(1);
		mesh->initGL();
		mesh->upload();
		mesh->calculateBounds();
		return mesh;
	}

	TetMesh* loadTetMeshExact(path path) {
		TetMesh* mesh = new TetMesh;
		std::ifstream input(path.string());

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
						mesh->tetIndices.push_back((unsigned int)mesh->vertices.size() + i);
					else
						mesh->tetIndices.push_back(i - 1);
					iss.ignore(256, ' ');
				}
			}
		}

		mesh->regenSurface();

		mesh->defMaterial = nullptr;
		mesh->defTransform = mat4(1);
		mesh->initGL();
		mesh->upload();
		mesh->calculateBounds();
		return mesh;
	}

	size_t TetMesh::numTet() {
		return tetIndices.size() / 4;
	}

	void TetMesh::writeMSH(string p) {
		FILE* file;
		fopen_s(&file, p.c_str(), "w");
		if (file) {
			fprintf(file, "$MeshFormat\n4.1 0 8\n$EndMeshFormat\n");
			fprintf(file, "$Nodes\n1 %zd 1 %zd\n3 0 0 %zd\n", vertices.size(), vertices.size(), vertices.size());
			for (size_t i = 1; i <= vertices.size(); i++)
				fprintf(file, "%zd\n", i);

			for (size_t i = 0; i < vertices.size(); i++) {
				vec3 v = vertices[i].pos;
				fprintf(file, "%.16f %.16f %.16f\n", v.x, v.y, v.z);
			}

			fprintf(file, "$EndNodes\n$Elements\n1 %zd 1 %zd\n3 0 4 %zd\n", numTet(), numTet(), numTet());
			for (size_t i = 0; i < numTet(); i++) {
				fprintf(file, "%zd %d %d %d %d\n", i + 1,
					tetIndices[4 * i + 0] + 1,
					tetIndices[4 * i + 1] + 1,
					tetIndices[4 * i + 2] + 1,
					tetIndices[4 * i + 3] + 1
				);
			}

			fprintf(file, "$EndElements\n$Surface\n%zd\n", indices.size() / 3);

			for (size_t i = 0; i < indices.size() / 3; i++) {
				ivec3 v = ivec3(
					indices[3 * i + 0],
					indices[3 * i + 1],
					indices[3 * i + 2]
				) + 1;
				fprintf(file, "%d %d %d\n", v.x, v.y, v.z);
			}
			fprintf(file, "$EndSurface\n");

			fclose(file);
		}
	}

	void TetMesh::flipInverted() {
		for (size_t i = 0; i < tetIndices.size(); i += 4) {
			auto& i0 = tetIndices[i + 0];
			auto& i1 = tetIndices[i + 1];
			auto& i2 = tetIndices[i + 2];
			auto& i3 = tetIndices[i + 3];

			float v = determinant(mat3(
				vertices[i1].pos - vertices[i0].pos,
				vertices[i2].pos - vertices[i0].pos,
				vertices[i3].pos - vertices[i0].pos
			));

			if (v < 0) std::swap(i2, i3);
		}
	}

	void TetMesh::regenSurface() {
		using tri = tuple<int, int, int>;
		unordered_set<tri> faceSet;

		auto sortTri = [](tri t) {
			if (get<0>(t) > get<1>(t)) swap(get<0>(t), get<1>(t));
			if (get<1>(t) > get<2>(t)) {
				swap(get<1>(t), get<2>(t));
				if (get<0>(t) > get<1>(t)) swap(get<0>(t), get<1>(t));
			}
			return t;
		};

		for (size_t i = 0; i < tetIndices.size(); i += 4) {
			tri tris[4]{
				sortTri(make_tuple(tetIndices[i + 0], tetIndices[i + 1], tetIndices[i + 2])),
				sortTri(make_tuple(tetIndices[i + 0], tetIndices[i + 1], tetIndices[i + 3])),
				sortTri(make_tuple(tetIndices[i + 0], tetIndices[i + 2], tetIndices[i + 3])),
				sortTri(make_tuple(tetIndices[i + 1], tetIndices[i + 2], tetIndices[i + 3]))
			};

			// Relies on the assumption that interior faces have exactly two neighboring tets.
			for (size_t k = 0; k < 4; k++) {
				auto itr = faceSet.find(tris[k]);
				if (itr != faceSet.end())  // Is a face
					faceSet.erase(itr);
				else
					faceSet.insert(tris[k]);
			}
		}

		indices.clear();
		for (auto t : faceSet) {
			indices.push_back(get<0>(t));
			indices.push_back(get<1>(t));
			indices.push_back(get<2>(t));
		}

		groups.clear();
		groups.push_back(indices.size());
	}
}