
#include "../includes/modules/Shader.h"

namespace Kitten {
	Shader::~Shader() {
		if (glHandle) glDeleteProgram(glHandle);
	}
}