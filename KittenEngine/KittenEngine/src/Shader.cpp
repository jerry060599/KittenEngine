
#include <stdio.h>
#include "../includes/modules/KittenRendering.h"
#include "../includes/modules/Shader.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

namespace Kitten {
	Shader::~Shader() {
		for (auto h : unlinkedHandles)
			glDeleteShader(h);
		if (glHandle) glDeleteProgram(glHandle);
	}

	bool checkLink(unsigned int obj) {
		int  success;
		char infoLog[512];
		glGetProgramiv(obj, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(obj, 512, NULL, infoLog);
			printf(" shader link failed %s\n", infoLog);
			return false;
		}

		return true;
	}

	bool Shader::link() {
		tryLinked = true;
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();
		for (auto h : unlinkedHandles)
			glAttachShader(shaderProgram, h);
		glLinkProgram(shaderProgram);

		if (checkLink(shaderProgram)) {
			glHandle = shaderProgram;
			return true;
		}

		checkErr("shader_link_err");
		return false;
	}

	void Shader::use() {
		if (!tryLinked) link();
		glUseProgram(glHandle);
	}

	void Shader::unuse() {
		glUseProgram(0);
	}

	GLenum Shader::drawMode() {
		if (type & (int)ShaderType::TESS)
			return GL_PATCHES;
		return GL_TRIANGLES;
	}

	void Shader::dispatchCompute(int numThreads) {
		dispatchCompute(ivec3(numThreads, 1, 1));
	}

	void Shader::dispatchCompute(glm::ivec2 numThreads) {
		dispatchCompute(ivec3(numThreads, 1));
	}

	void Shader::dispatchCompute(ivec3 numThreads) {
		ivec3 size;
		glGetProgramiv(glHandle, GL_COMPUTE_WORK_GROUP_SIZE, (int*)&size);
		ivec3 numGroups = (numThreads - 1) / size + 1;

		use();
		glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
		unuse();
	}

	void Shader::setBuffer(const char* name, ComputeBuffer* buffer) {
		throw new exception("TODO here.");
		/*
		glUseProgram(glHandle);
		int binding = glGetProgramResourceIndex(glHandle, GL_SHADER_STORAGE_BLOCK, name);
		//glGetActiveUniformBlockiv(glHandle, glGetUniformLocation(glHandle, name), GL_UNIFORM_BLOCK_BINDING, &binding);
		//glGetUniformiv(glHandle, glGetUniformLocation(glHandle, name), &binding);
		printf("%d\n", binding);
		buffer->bind(binding);
		glUseProgram(0);*/
	}

	void Shader::setBool(const char* name, bool v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform1i(glGetUniformLocation(glHandle, name), v);
	}

	void Shader::setInt(const char* name, int v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform1i(glGetUniformLocation(glHandle, name), v);
	}

	void Shader::setFloat(const char* name, float v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform1f(glGetUniformLocation(glHandle, name), v);
	}

	void Shader::setFloat2(const char* name, glm::vec2 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform2fv(glGetUniformLocation(glHandle, name), 1, glm::value_ptr(v));
	}

	void Shader::setFloat3(const char* name, glm::vec3 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform3fv(glGetUniformLocation(glHandle, name), 1, glm::value_ptr(v));
	}

	void Shader::setFloat4(const char* name, glm::vec4 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform4fv(glGetUniformLocation(glHandle, name), 1, glm::value_ptr(v));
	}

	void Shader::setRotor(const char* name, Rotor v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniform4fv(glGetUniformLocation(glHandle, name), 1, (float*)&v);
	}

	void Shader::setMat2(const char* name, glm::mat2 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniformMatrix2fv(glGetUniformLocation(glHandle, name), 1, GL_FALSE, glm::value_ptr(v));
	}

	void Shader::setMat3(const char* name, glm::mat3 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniformMatrix3fv(glGetUniformLocation(glHandle, name), 1, GL_FALSE, glm::value_ptr(v));
	}

	void Shader::setMat4(const char* name, glm::mat4 v) {
		glTempVar<GL_CURRENT_PROGRAM> prog(glHandle);
		glUniformMatrix4fv(glGetUniformLocation(glHandle, name), 1, GL_FALSE, glm::value_ptr(v));
	}
}