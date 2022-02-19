#include "../includes/modules/KittenInit.h"

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Kitten {
	GLFWwindow* window = nullptr;

	void initWindow(glm::ivec2 res, const char* title) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 16);
		window = glfwCreateWindow(res.x, res.y, title, nullptr, nullptr);
		if (!window) {
			std::cerr << "Cannot create window";
			std::exit(1);
		}
		glfwMakeContextCurrent(window);

		assert(window);
		if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
			std::cerr << "Failed to intialize OpenGL loader" << std::endl;
			std::exit(1);
		}
		assert(glGetError() == GL_NO_ERROR);

		glEnable(GL_MULTISAMPLE);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		const char* glsl_version = "#version 130";
		ImGui_ImplOpenGL3_Init(glsl_version);

		ImGui::SetNextWindowSize(ImVec2((float)res.x, (float)res.y));
	}
}