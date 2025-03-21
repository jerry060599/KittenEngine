
#include <cassert>
#include <iostream>

#include "KittenEngine/includes/KittenEngine.h"
#include "KittenEngine/includes/modules/BasicCameraControl.h"

using namespace glm;
using namespace std;

Kit::BasicCameraControl camera;
Kit::Mesh* mesh;

void renderScene() {
	Kit::lights[0].dir = -normalize(Kit::lights[0].pos);
	Kit::projMat = glm::perspective(45.0f, Kit::getAspect(), 0.05f, 512.f);
	Kit::viewMat = camera.getViewMatrix();

	// Render everything
	Kit::startRender();
	glClearColor(0.3f, 0.3f, 0.35f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Kit::renderShadows(mesh);
	Kit::renderForward(Kit::defMesh, Kit::defBaseShader, Kit::defForwardShader);
	Kit::renderForward(mesh, Kit::defBaseShader, Kit::defForwardShader);

	Kit::get<Kit::Font>("resources\\fonts\\Quicksand_Regular.ttf")->
		render("Hello world!!", 8, vec4(1, 1, 1, 1), Kit::Bound<2>(vec2(0, 0), vec2(0.35, 1)));
}

void renderGui() {
	ImGui::Begin("Control Panel");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::DragFloat3("Light Position", (float*)&Kit::lights[0].pos, 0.01f);

	ImGui::End();
}

void initScene() {
	Kit::loadDirectory("resources");
	mesh = Kit::get<Kit::Mesh>("resources\\teapot\\teapot.obj");

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

	Kit::ambientLight.col = vec4(0.f, 0.f, 0.1f, 1);

	camera.angle = vec2(30, 30);
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mode) {
	camera.processMouseButton(button, action, mode);
}

void cursorPosCallback(GLFWwindow* w, double xp, double yp) {
	camera.processMousePos(xp, yp);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll(xoffset, yoffset);
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);
}

int main(int argc, char** argv) {
	// Init window and OpenGL
	Kit::initWindow(ivec2(800, 600), "OpenGL Window");

	// Register callbacks
	Kit::getIO().mouseButtonCallback = mouseButtonCallback;
	Kit::getIO().cursorPosCallback = cursorPosCallback;
	Kit::getIO().scrollCallback = scrollCallback;
	Kit::getIO().keyCallback = keyCallback;

	// Init scene
	initScene();

	while (!Kit::shouldClose()) {
		Kit::startFrame();
		renderScene();		// Render
		renderGui();		// GUI Render
		Kit::endFrame();
	}

	Kit::terminate();
	return 0;
}