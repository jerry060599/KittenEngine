#pragma once
#include <string>
#include <glad/glad.h> 

using namespace std;
namespace Kitten {
	enum class ShaderType {
		SHADERTYPE_VERT,
		SHADERTYPE_FRAG,
		SHADERTYPE_VERTFRAG,
		SHADERTYPE_COMPUTE
	};

	class Shader {
	public:
		~Shader();

		unsigned int glHandle = 0;
		ShaderType type;
	};
}