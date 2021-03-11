#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

using namespace glm;
using namespace std;

namespace Kitten {
	class Texture {
	public:
		bool isCubeMap = false;
		unsigned int glHandle;
		int width;
		int height;
		GLenum channels;
		GLenum dataType;
		ivec4 borders = ivec4(0);
		float ratio;
		unsigned char* rawData = nullptr;

		Texture();
		Texture(int width, int height, GLenum channels = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE);
		Texture(Texture* xpos, Texture* xneg, Texture* ypos, Texture* yneg, Texture* zpos, Texture* zneg);
		~Texture();

		void genMipmap();
		void resize(int w, int h);
		void setAniso(float v);
	};
}