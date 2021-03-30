#pragma once
#include <string>
#include <vector>
#include <glad/glad.h> 

using namespace std;
namespace Kitten {
	enum class ShaderType {
		VERT = 1,
		FRAG = 2,
		GEOM = 4,
		TESS = 8,
		COMP = 16
	};

	class Shader {
	public:
		~Shader();

		unsigned int glHandle = 0;
		int type;
		bool tryLinked = false;
		vector<unsigned int> unlinkedHandles;

		bool link();
		void use();
		void unuse();
		GLenum drawMode();
	};
}