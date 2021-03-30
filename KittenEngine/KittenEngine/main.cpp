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
float camDist = 2;

vec2 lastMousePos = vec2(0.0, 0.0);
bool camRot = false;
bool camSlide = false;

string meshPath = "resources\\teapot\\teapot.obj";

namespace Kit = Kitten;

Kit::Material quadMat = Kit::defMaterial;
Kit::Mesh* mesh;

Kit::FrameBuffer* frameBuff = nullptr;

void renderScene() {
	Kit::lights[0].dir = -normalize(Kit::lights[0].pos);

	float aspect = (float)res.x / (float)res.y;
	Kit::projMat = glm::perspective(45.0f, aspect, 0.05f, 512.f);
	Kit::viewMat = glm::rotate(glm::mat4(1.0f), glm::radians(camEuler.y), { 1.0f, 0.0f, 0.0f });
	Kit::viewMat = glm::rotate(Kit::viewMat, glm::radians(camEuler.x), { 0.0f, 1.0f, 0.0f });
	Kit::viewMat = glm::translate(mat4(1.f), { 0.0f, 0.0f, -camDist }) * Kit::viewMat;

	// Render everything
	Kit::startRender();
	glClearColor(0.3f, 0.3f, 0.35f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Kit::renderShadows(mesh);
	Kit::renderForward(Kit::defMesh, Kit::defBaseShader, Kit::defForwardShader);
	Kit::renderForward(mesh, Kit::defBaseShader, Kit::defForwardShader);
}

void renderGui() {
	ImGui::Begin("Control Panel");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::DragFloat3("Light Position", (float*)&Kit::lights[0].pos, 0.01f);

	ImGui::End();
}

void initScene() {
	Kit::loadAsset(meshPath);
	Kit::loadDirectory("resources");
	mesh = (Kit::Mesh*)Kit::resources[meshPath];

	Kit::defMesh->defMaterial = &quadMat;
	Kit::defMesh->defMaterial->props.col = vec4(0.4f, 0.4f, 0.5f, 1);
	Kit::defMesh->defMaterial->props.col1 = vec4(0.5f, 0.5f, 0.5f, 1);
	Kit::defMesh->defMaterial->props.params0.x = 6;
	Kit::defMesh->defTransform = glm::translate(glm::scale(glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), { 1.0f, 0.0f, 0.0f }), { 2.f, 2.f, 1.f }), { -.5f, -.5f, 0.f });

	float invScale = pow(mesh->bounds.volume(), -1.f / 3.f);
	mesh->transform(glm::rotate(glm::scale(mat4(1), { invScale, invScale, invScale }), glm::radians(-90.f), { 1.0f, 0.0f, 0.0f }));
	mesh->calculateBounds();
	mesh->defTransform = glm::translate(mat4(1), { 0, 0.03f - mesh->bounds.min.y, 0 });
	mesh->upload();

	mesh->defMaterial->props.col = vec4(0.1f, 0.1f, 0.1f, 1);
	mesh->defMaterial->props.col1 = vec4(0.5f, 0.5f, 0.5f, 1);
	mesh->defMaterial->props.params0.x = 6;

	Kit::UBOLight light;
	light.col = vec4(1, 1, 1, 4);
	light.dir = vec3(sin(radians(30.f)), -cos(radians(30.f)), 0);
	light.pos = -2.f * light.dir;
	light.spread = 0.9f;
	light.hasShadow = true;
	light.type = (int)Kit::KittenLight::SPOT;
	Kit::lights.push_back(light);
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
		target = &camEuler;

		vec2 curMousePos = vec2(xp, yp);
		vec2 mouseDelta = curMousePos - lastMousePos;
		mouseDelta *= 0.8f;

		*target += mouseDelta;
		(*target).y = glm::clamp((*target).y, -70.f, 70.f);
		lastMousePos = curMousePos;
	}
	if (camSlide) {
		float* target;
		target = &camDist;

		vec2 curMousePos = vec2(xp, yp);
		vec2 mouseDelta = curMousePos - lastMousePos;
		mouseDelta *= 0.2f;
		*target = glm::max(0.5f, *target * powf(1.2f, mouseDelta.x));
		lastMousePos = curMousePos;
	}
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
	res.x = width;
	res.y = height;
	glViewport(0, 0, width, height);
	if (frameBuff) frameBuff->resize(res.x, res.y);
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);

	if (ImGui::GetIO().WantCaptureMouse) return;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (ImGui::GetIO().WantCaptureMouse) return;
	float* target;
	target = &camDist;
	*target = glm::max(0.5f, *target * powf(1.2f, yoffset));
}

void initGL() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	Kit::initRender();

	if (argc > 1) meshPath = string(argv[1]);
	initScene();

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