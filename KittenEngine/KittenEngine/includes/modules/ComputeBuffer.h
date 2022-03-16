#pragma once
// Jerry Hsu, 2021

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

using namespace glm;
using namespace std;

namespace Kitten {
	class ComputeBuffer {
	public:
		unsigned int glHandle;
		size_t elementSize, size;
		GLenum usage;

		ComputeBuffer();
		ComputeBuffer(size_t elementSize, size_t size, GLenum usage = GL_DYNAMIC_READ);
		~ComputeBuffer();

		void bind(int loc);
		void resize(size_t newSize);
		void upload(void* src);
		void download(void* dst);
	};
}