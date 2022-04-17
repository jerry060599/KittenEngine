
#include <cassert>
#include <iostream>

#include "KittenEngine/includes/KittenEngine.h"

using namespace glm;
using namespace std;

ivec2 res(800, 600);

vec2 camEuler = vec2(30.0, 30.0);
float camDist = 2;

vec2 lastMousePos = vec2(0.0, 0.0);
bool camRot = false;
bool camSlide = false;

Kit::Mesh* mesh;

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
	Kit::loadDirectory("resources");
	mesh = (Kit::Mesh*)Kit::resources["resources\\teapot\\teapot.obj"];

	Kit::defMesh->defMaterial = &Kit::defMaterial;
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
	if (camRot) {
		vec2 curMousePos = vec2(xp, yp);
		vec2 mouseDelta = curMousePos - lastMousePos;
		mouseDelta *= 0.8f;

		camEuler += mouseDelta;
		camEuler.y = glm::clamp(camEuler.y, -70.f, 70.f);
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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camDist = glm::max(0.5f, camDist * powf(1.2f, (float)yoffset));
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

int main(int argc, char** argv) {
	// Init window and OpenGL
	Kit::initWindow(res, "OpenGL Window");
	Kit::initRender();

	// Register callbacks
	Kit::getIO().mouseButtonCallback = mouseButtonCallback;
	Kit::getIO().cursorPosCallback = cursorPosCallback;
	Kit::getIO().scrollCallback = scrollCallback;
	Kit::getIO().framebufferSizeCallback = framebufferSizeCallback;
	Kit::getIO().keyCallback = keyCallback;

	// Init scene
	initScene();

	while (!Kit::shouldClose()) {
		Kit::startFrame();
		renderScene();		// Render
		renderGui();		// GUI Render
		Kit::endFrame();
	}
}