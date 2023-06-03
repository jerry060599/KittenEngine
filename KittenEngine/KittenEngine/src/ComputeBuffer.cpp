
#include "../includes/modules/ComputeBuffer.h"

using namespace Kitten;
using namespace glm;

ComputeBuffer::ComputeBuffer(size_t elementSize, size_t size, GLenum usage)
	: elementSize(elementSize), size(size), usage(usage) {
	glGenBuffers(1, &glHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, glHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, elementSize * size, nullptr, usage);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

ComputeBuffer::~ComputeBuffer() {
	glDeleteBuffers(1, &glHandle);
}

void Kitten::ComputeBuffer::bind(int loc) {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, loc, glHandle);
}

void ComputeBuffer::resize(size_t newSize) {
	if (size == newSize) return;

	glDeleteBuffers(1, &glHandle);
	size = newSize;

	glGenBuffers(1, &glHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, glHandle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, elementSize * size, nullptr, usage);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ComputeBuffer::upload(void* src) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, glHandle);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, elementSize * size, src);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ComputeBuffer::download(void* dst) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, glHandle);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, elementSize * size, dst);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
