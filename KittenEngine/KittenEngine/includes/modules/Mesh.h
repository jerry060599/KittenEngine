#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include "KittenAssets.h"

using namespace std;
using namespace std::filesystem;
using namespace glm;

namespace Kitten {
	typedef struct Vertex {
		vec3 pos;
		vec3 norm;
		vec2 uv;
	} Vertex;

	class Mesh {
	public:
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<size_t> groups;
		mat4 defTransform = mat4(1);
		Material* defMaterial;

		Bound<> bounds;

		unsigned int VAO, VBO, EBO;

		bool initialized = false;

		~Mesh();

		void polygonize();
		void transform(mat4);
		void draw();
		void initGL();
		void upload();
		void calculateBounds();
		void writeOBJ(string p);
		void writePOLY(string p);
	};

	class TetMesh : public Mesh {
	public:
		vector<unsigned int> tetIndices;
		size_t numTet();
		void writeMSH(string p);
		void flipInverted();
		void regenSurface();
	};

	Mesh* genQuadMesh(int rows = 1, int cols = 1);
	void loadMeshFrom(path path);
	void loadTetgenFrom(path path);
	Mesh* loadMeshExact(path path);
	TetMesh* loadTetMeshExact(path path);
}