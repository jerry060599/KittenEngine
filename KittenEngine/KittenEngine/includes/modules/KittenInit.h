#pragma once
// Jerry Hsu, 2021

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Kitten {
	extern GLFWwindow* window;

	void initWindow(glm::ivec2 res, const char* title = "OpenGL Window");
}