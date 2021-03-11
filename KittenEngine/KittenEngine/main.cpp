#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "KittenEngine/includes/KittenEngine.h"

#include <cassert>
#include <iostream>

using namespace glm;
using namespace std;

GLFWwindow* window = nullptr;
ivec2 res(800, 600);

vec2 camEuler = vec2(30.0, 30.0);
vec2 lightEuler = vec2(30.0, 30.0);
float camDist = 30;
vec2 lastMousePos = vec2(0.0, 0.0);
bool camRot = false;
bool camSlide = false;

Kitten::Mesh* mesh;
Kitten::Shader* baseShader;
Kitten::Shader* forwardShader;

void renderScene() {
	double t = glfwGetTime();

	glClearColor(0.f, 0.f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspect = (float)res.x / (float)res.y;
	Kitten::projMat = glm::perspective(45.0f, aspect, 0.05f, 512.f);
	Kitten::viewMat = glm::rotate(glm::mat4(1.0f), glm::radians(camEuler.y), { 1.0f, 0.0f, 0.0f });
	Kitten::viewMat = glm::rotate(Kitten::viewMat, glm::radians(camEuler.x), { 0.0f, 1.0f, 0.0f });
	Kitten::viewMat = glm::translate(mat4(1.f), { 0.0f, 0.0f, -camDist }) * Kitten::viewMat;

	Kitten::lights[0].pos = vec3(cos(radians(lightEuler.x)), 0, sin(radians(lightEuler.x))) * sin(radians(lightEuler.y)) - vec3(0, cos(radians(lightEuler.y)), 0);

	Kitten::startRender();

	Kitten::renderForward(mesh, baseShader, forwardShader);
}

void renderGui() {
	Kitten::Material* mat = mesh->defMaterial;

	ImGui::Begin("Control Panel");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::DragFloat("Specular Alpha", &mat->props.params0.x, 1.f, 0.f, numeric_limits<float>::infinity());
	ImGui::ColorEdit3("Diffuse Color", (float*)&mat->props.col, ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("Specular Color", (float*)&mat->props.col1, ImGuiColorEditFlags_Float);
	ImGui::Separator();
	ImGui::DragFloat("Light Intensity", (float*)&Kitten::lights[0].col.w, 0.05f, 0.f, numeric_limits<float>::infinity());
	ImGui::ColorEdit3("Light Color", (float*)&Kitten::lights[0].col, ImGuiColorEditFlags_Float);
	ImGui::Separator();
	ImGui::DragFloat("Ambient Light Intensity", (float*)&Kitten::ambientLight.col.w, 0.05f, 0.f, numeric_limits<float>::infinity());
	ImGui::ColorEdit3("Ambient Light Color", (float*)&Kitten::ambientLight.col, ImGuiColorEditFlags_Float);

	ImGui::End();
}

void initScene() {
	Kitten::loadDirectory("resources");
	mesh = (Kitten::Mesh*)Kitten::resources["resources\\teapot\\teapot.obj"];
	baseShader = (Kitten::Shader*)Kitten::resources["resources\\shaders\\blingBase.glsl"];
	forwardShader = (Kitten::Shader*)Kitten::resources["resources\\shaders\\blingForward.glsl"];

	mesh->transform(glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), { 1.0f, 0.0f, 0.0f }));
	mesh->upload();

	Kitten::UBOLight light;
	light.col = vec4(1, 1, 1, 2);
	light.pos = vec3(0, -1, 0);
	light.params = vec4(0);
	light.type = (int)Kitten::KittenLight::DIR;
	Kitten::lights.push_back(light);
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mode) {
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Activate rotation mode
		camRot = true;
		double xpos, ypos;
		glfwGetCursorPos(w, &xpos, &ypos);
		lastMousePos = glm::vec2(xpos, ypos);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		// Activate rotation mode
		camSlide = true;
		double xpos, ypos;
		glfwGetCursorPos(w, &xpos, &ypos);
		lastMousePos = glm::vec2(xpos, ypos);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		camRot = false;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		camSlide = false;
}

void cursorPosCallback(GLFWwindow* w, double xp, double yp) {
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (camRot) {
		vec2* target;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			target = &lightEuler;
		else
			target = &camEuler;

		vec2 curMousePos = vec2(xp, yp);
		vec2 mouseDelta = curMousePos - lastMousePos;
		mouseDelta *= 0.8f;

		*target += mouseDelta;
		(*target).y = glm::clamp((*target).y, -70.f, 70.f);
		lastMousePos = curMousePos;
	}
	if (camSlide) {
		vec2 curMousePos = vec2(xp, yp);
		vec2 mouseDelta = curMousePos - lastMousePos;
		mouseDelta *= 0.2f;
		camDist = glm::max(0.5f, camDist * powf(1.2f, mouseDelta.x));
		lastMousePos = curMousePos;
	}
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
	res.x = width;
	res.y = height;
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);

	if (ImGui::GetIO().WantCaptureMouse) return;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (ImGui::GetIO().WantCaptureMouse) return;

	camDist = glm::max(0.5f, camDist * powf(1.2f, yoffset));
}

void initGL() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(res.x, res.y, "CS6610 P1", nullptr, nullptr);
	if (!window) {
		std::cerr << "Cannot create window";
		std::exit(1);
	}
	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scrollCallback);

	assert(window);
	if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
		std::cerr << "Failed to intialize OpenGL loader" << std::endl;
		std::exit(1);
	}
	assert(glGetError() == GL_NO_ERROR);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::SetNextWindowSize(ImVec2(res.x, res.y));
}

int main(int argc, char** argv) {
	initGL();
	initScene();

	if (argc > 1) {
		Kitten::loadAsset(argv[1]);
		mesh = (Kitten::Mesh*)Kitten::resources[argv[1]];
	}

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		renderScene();

		glfwPollEvents();
		renderGui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
			printf("GL error: %d\n", error);

		glfwSwapBuffers(window);
	}
}