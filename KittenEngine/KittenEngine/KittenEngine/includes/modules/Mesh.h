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
		vector<unsigned int> tetraInds;
		mat4 defTransform = mat4(1);
		Material* defMaterial;

		Bound bounds;

		unsigned int VAO, VBO, EBO;

		bool initialized = false;

		~Mesh();

		void transform(mat4);
		void draw();
		void initGL();
		void upload();
		void calculateBounds();
	};

	void loadMeshFrom(path path);
}