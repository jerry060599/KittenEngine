#pragma once
#include <filesystem>
#include <map>

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Common.h"
#include "Bound.h"
#include "Texture.h"
#include "Shader.h"
#include "FrameBuffer.h"

#include <assimp/Importer.hpp>

namespace Kitten {
	typedef struct UBOMat {
		vec4 col;
		vec4 col1;
		vec4 params0;
		vec4 params1;
	} UBOMat;

	typedef struct Material {
		UBOMat props;
		Texture* texs[8] = {};
	} Material;
}

#include "Mesh.h"

namespace Kitten {
	using namespace std;
	using namespace std::filesystem;

	extern unsigned int meshImportFlags;
	extern map<string, void*> resources;

	void loadDirectory(path dir);
	void loadAsset(path path);
	void loadShader(path path);
	void loadTexture(path path);
	void loadMesh(path path);
};

