
#include <stdio.h>
#include "../includes/modules/Shader.h"

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
}