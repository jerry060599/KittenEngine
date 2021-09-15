#include "../includes/modules/KittenAssets.h"
#include "../includes/modules/KittenPreprocessor.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ratio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <regex>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

namespace Kitten {
	map<string, void*> resources;

	bool checkCompile(unsigned int obj) {
		int  success;
		char infoLog[512];
		glGetShaderiv(obj, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(obj, 512, NULL, infoLog);
			std::cout << "err: shader compilation failed.\n" << infoLog << std::endl;
			return false;
		}

		return true;
	}

	bool compileShader(string path, GLenum type, unsigned int* handle) {
		string src = Kitten::loadTextWithIncludes(path);
		unsigned int s = glCreateShader(type);
		const char* cstr = src.c_str();
		glShaderSource(s, 1, &cstr, NULL);
		glCompileShader(s);
		*handle = s;

		if (!checkCompile(s)) {
			Kitten::printWithLineNumber(src);
			return false;
		}
		return true;
	}

	void loadDirectory(path root) {
		if (root.is_relative())
			for (auto& p : recursive_directory_iterator(root)) {
				path f(p);
				if (is_regular_file(f))
					loadAsset(f);
			}
		else
			cout << "err: resource path must be relative" << endl;
	}

	void loadAsset(path path) {
		glGetError();
		string ext = path.extension().string();
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
			loadTexture(path);
		else if (ext == ".vert" || ext == ".frag" || ext == ".comp" || ext == ".geom" || ext == ".tesc" || ext == ".tese")
			loadShader(path);
		else if (ext == ".obj" || ext == ".fbx")
			loadMesh(path);
		else if (ext == ".glsl" || ext == ".include" || ext == ".mtl") {
		}
		else if (ext == ".csv" || ext == ".txt" || ext == ".cfg" || ext == ".json") {
			cout << "asset: loading text " << path.string().c_str() << endl;
			resources[path.string()] = new string(loadText(path.string()));
		}
		else
			cout << "err: unknown asset type " << path << endl;
		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
			printf("err: internal GL error: %d\n", error);
	}

	void loadMesh(path path) {
		cout << "asset: loading model " << path.string().c_str() << endl;
		loadMeshFrom(path);
	}

	void loadTexture(path path) {
		if (resources.count(path.string()))
			return;
		string filename = path.filename().string();
		string parsedName;
		Tags tags;
		parseAssetTag(filename, parsedName, tags);
		printf("asset: loading image %s (%s)\n", path.string().c_str(), parsedName.c_str());

		parsedName = path.parent_path().string().append("\\").append(parsedName);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 4);
		if (data) {
			unsigned int handle;
			glGenTextures(1, &handle);
			glBindTexture(GL_TEXTURE_2D, handle);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			if ((tags["point"]).x) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			Texture* img = new Texture;
			img->glHandle = handle;
			img->width = width;
			img->deviceFormat = GL_RGBA;
			img->hostFormat = GL_RGBA;
			img->hostDataType = GL_UNSIGNED_BYTE;
			img->height = height;
			img->ratio = float(width) / height;
			img->borders = tags["border"];
			img->rawData = data;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			resources[parsedName] = img;
			resources[path.string()] = img;
		}
		else
			cout << "err: failed to load " << path << endl;
	}

	void loadShader(path path) {
		string ext = path.extension().string();
		string name = path.filename().string();

		unsigned int handle;
		int type = 0;
		Shader* shader;
		string sName = path.string().substr(0, path.string().length() - 5).append(".glsl");

		printf("asset: loading shader %s (%s)\n", sName.c_str(), ext.c_str());

		if (resources.count(sName))
			shader = (Shader*)resources[sName];
		else {
			shader = new Shader;
			resources[sName] = shader;
		}

		if (ext == ".vert") {
			type = (int)ShaderType::VERT;
			compileShader(path.string(), GL_VERTEX_SHADER, &handle);
		}
		else if (ext == ".frag") {
			type = (int)ShaderType::FRAG;
			compileShader(path.string(), GL_FRAGMENT_SHADER, &handle);
		}
		else if (ext == ".geom") {
			type = (int)ShaderType::GEOM;
			compileShader(path.string(), GL_GEOMETRY_SHADER, &handle);
		}
		else if (ext == ".tesc") {
			type = (int)ShaderType::TESS;
			compileShader(path.string(), GL_TESS_CONTROL_SHADER, &handle);
		}
		else if (ext == ".tese") {
			type = (int)ShaderType::TESS;
			compileShader(path.string(), GL_TESS_EVALUATION_SHADER, &handle);
		}
		else if (ext == ".comp") {
			type = (int)ShaderType::COMP;
			compileShader(path.string(), GL_COMPUTE_SHADER, &handle);
		}
		if (type == 0) {
			printf("err: unknown shader type %s.", name.c_str());
			return;
		}

		shader->unlinkedHandles.push_back(handle);
		shader->type |= type;
	}
}