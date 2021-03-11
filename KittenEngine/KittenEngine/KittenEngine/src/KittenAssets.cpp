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

bool checkCompile(unsigned int obj) {
	int  success;
	char infoLog[512];
	glGetShaderiv(obj, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(obj, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

bool checkLink(unsigned int obj) {
	int  success;
	char infoLog[512];
	glGetProgramiv(obj, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(obj, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

bool linkVertFrag(unsigned int vert, unsigned int frag, unsigned int* handle) {
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vert);
	glAttachShader(shaderProgram, frag);
	glLinkProgram(shaderProgram);

	if (checkLink(shaderProgram)) {

		glDeleteShader(vert);
		glDeleteShader(frag);

		*handle = shaderProgram;
		return true;
	}
	return false;
}

bool compileShader(string path, GLenum type, unsigned int* handle) {
	set<string> includeSet;
	string src = Kitten::loadTextWithIncludes(path, includeSet);
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

namespace Kitten {
	map<string, void*> resources;

	void Kitten::loadDirectory(path root) {
		if (root.is_relative())
			for (auto& p : recursive_directory_iterator(root)) {
				path f(p);
				if (is_regular_file(f))
					loadAsset(f);
			}
		else
			cout << "Resource path must be relative" << endl;
	}

	void loadAsset(path path) {
		string ext = path.extension().string();
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
			loadTexture(path);
		else if (ext == ".vert" || ext == ".frag" || ext == ".comp")
			loadShader(path);
		else if (ext == ".obj" || ext == ".fbx")
			loadMesh(path);
		else if (ext == ".glsl" || ext == ".include" || ext == ".mtl") {
		}
		else if (ext == ".csv" || ext == ".txt" || ext == ".cfg" || ext == ".json") {
			cout << "loading text: " << path.string().c_str() << endl;
			resources[path.string()] = new string(loadText(path.string()));
		}
		else
			cout << "unknown asset type " << path << endl;
		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
			printf("KittenAsset GL error: %d\n", error);
	}

	void loadMesh(path path) {
		cout << "loading model: " << path.string().c_str() << endl;
		loadMeshFrom(path);
	}

	void loadTexture(path path) {
		if (resources.count(path.string()))
			return;
		string filename = path.filename().string();
		string parsedName;
		Tags tags;
		parseAssetTag(filename, parsedName, tags);
		printf("loading image: %s (%s)\n", path.string().c_str(), parsedName.c_str());

		parsedName = path.parent_path().string().append("\\").append(parsedName);

		int width, height, nrChannels;
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
			img->channels = GL_RGBA;
			img->dataType = GL_UNSIGNED_BYTE;
			img->height = height;
			img->ratio = float(width) / height;
			img->borders = tags["border"];
			img->rawData = data;

			glTexImage2D(GL_TEXTURE_2D, 0, img->channels, width, height, 0, img->channels, img->dataType, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			resources[parsedName] = img;
			resources[path.string()] = img;
		}
		else
			cout << "failed to load " << path << endl;
	}

	void loadShader(path path) {
		cout << "loading shader: " << path.string().c_str() << endl;
		string ext = path.extension().string();
		Shader* shader;
		unsigned int handle;
		string name = path.filename().string();

		if (ext == ".vert") {
			string sName = path.string().substr(0, path.string().length() - 5).append(".glsl");
			if (compileShader(path.string(), GL_VERTEX_SHADER, &handle))
				if (resources.count(sName)) {
					cout << "Fragment shader found. Linking under " << sName << endl;
					shader = static_cast<Shader*>(resources[sName]);
					if (shader->type == ShaderType::SHADERTYPE_FRAG && linkVertFrag(handle, shader->glHandle, &(shader->glHandle)))
						shader->type = ShaderType::SHADERTYPE_VERTFRAG;
					else cout << path << " shader link error" << endl;
				}
				else {
					shader = new Shader;
					shader->type = ShaderType::SHADERTYPE_VERT;
					shader->glHandle = handle;
					resources[sName] = shader;
				}
		}
		else if (ext == ".frag") {
			string sName = path.string().substr(0, path.string().length() - 5).append(".glsl");
			if (compileShader(path.string(), GL_FRAGMENT_SHADER, &handle))
				if (resources.count(sName)) {
					cout << "Vertex shader found. Linking under " << sName << endl;
					shader = static_cast<Shader*>(resources[sName]);
					if (shader->type == ShaderType::SHADERTYPE_VERT && linkVertFrag(shader->glHandle, handle, &(shader->glHandle)))
						shader->type = ShaderType::SHADERTYPE_VERTFRAG;
					else cout << path << " shader link error" << endl;
				}
				else {
					shader = new Shader;
					shader->type = ShaderType::SHADERTYPE_FRAG;
					shader->glHandle = handle;
					resources[sName] = shader;
				}
		}
		else if (ext == ".comp") {
			string sName = path.string().substr(0, path.string().length() - 5).append(".glsl");
			if (resources.count(sName))
				cout << "error duplicate shader" << endl;
			else if (compileShader(path.string(), GL_COMPUTE_SHADER, &handle)) {
				unsigned int shaderProgram;
				shaderProgram = glCreateProgram();
				glAttachShader(shaderProgram, handle);
				glLinkProgram(shaderProgram);

				if (checkCompile(shaderProgram)) {
					glDeleteShader(handle);
					shader = new Shader;
					shader->type = ShaderType::SHADERTYPE_COMPUTE;
					shader->glHandle = shaderProgram;
					resources[sName] = shader;
				}
			}
		}
		else cout << "error loading " << path << " shader must end with Vert.glsl, Frag.glsl, or Compute.glsl" << endl;
	}
}