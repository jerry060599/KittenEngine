#pragma once
// Jerry Hsu, 2024

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Kitten {
	using namespace glm;

	// Gizmos can be queued with queueGizmo___ functions.
	// Gizmos provide a quick way to visualize/manipulate data in the scene.
	// Think of it as ImGui for the 3D renderer.
	typedef struct GizmoData {
		mat4 model;
		vec4 color;
	};

	void queueGizmoLine(vec3 a, vec3 b, float thickness, vec4 color);
	void queueGizmoSquare(vec3 a, float size, vec4 color);

	void queueGizmoLineScreenspace(vec2 a, vec2 b, float thickness, vec4 color);
	void queueGizmoSquareScreenspace(vec2 a, float size, vec4 color);

	void queueGizmo(GizmoData mat);

	void initGizmos();
	void renderGizmos();
}