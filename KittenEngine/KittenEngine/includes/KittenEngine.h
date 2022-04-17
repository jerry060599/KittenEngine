#pragma once

/*
// KittenEngine Quick Start Skeleton Code

#include "KittenEngine/includes/KittenEngine.h"

using namespace glm;
using namespace std;

ivec2 res(800, 600);

void renderScene() {
	Kit::startRender();
	// Do rendering here
}

void renderGui() {
	ImGui::Begin("ImGui Window");
	// Do ImGui here
	ImGui::End();
}

void initScene() {
	Kit::loadDirectory("resources");
	// Do scene preprocessing here
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mode) {
	// Mouse inputs here
}

void cursorPosCallback(GLFWwindow* w, double xp, double yp) {
	// Mouse inputs here
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	// Mouse inputs here
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
	// Frame buffer here
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode) {
	// Key inputs here
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

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "modules/KittenAssets.h"
#include "modules/KittenRendering.h"

#include "modules/KittenInit.h"

namespace Kit = Kitten;
