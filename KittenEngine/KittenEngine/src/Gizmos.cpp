#include "../includes/modules/Gizmos.h"
#include "../includes/modules/ComputeBuffer.h"
#include "../includes/modules/KittenAssets.h"
#include "../includes/modules/KittenRendering.h"
#include "../includes/modules/Shader.h"
#include <vector>

namespace Kitten {
	std::vector<GizmoData> gizmoRenderQueue;
	ComputeBuffer* gizmoBuffer;
	constexpr int MAX_GIZMO_PER_DRAW = 1024;

	void initGizmos() {
		gizmoBuffer = new ComputeBuffer(sizeof(GizmoData), MAX_GIZMO_PER_DRAW, GL_DYNAMIC_DRAW);
	}

	inline vec3 cameraPos() {
		return -transpose((mat3)viewMat) * (vec3)viewMat[3];
	}

	void queueGizmoLine(vec3 a, vec3 b, float thickness, vec4 color) {
		vec3 cam = cameraPos();
		vec3 dir = normalize(mix(a, b, lineClosestPoints(a, b, cam)) - cameraPos());

		GizmoData g;
		g.model[1] = vec4(b - a, 0);
		g.model[0] = vec4(normalize(cross((vec3)g.model[1], dir)), 0);
		g.model[2] = vec4(dir, 0);
		g.model[0] *= thickness;
		g.model[3] = vec4(a - 0.5f * (vec3)g.model[0], 1);
		g.color = color;
		g.model = projMat * viewMat * g.model;
		gizmoRenderQueue.push_back(g);
	}

	void queueGizmoSquare(vec3 a, float size, vec4 color) {
		vec3 dir = normalize(a - cameraPos());
		mat3 basis = orthoBasisZ(dir);
		GizmoData g;
		g.model = (mat4)(basis * mat3(size));
		g.model[3] = vec4(a - 0.5f * (vec3)g.model[0] - 0.5f * (vec3)g.model[1], 1);
		g.color = color;
		g.model = projMat * viewMat * g.model;
		gizmoRenderQueue.push_back(g);
	}

	void queueGizmoLineScreenspace(vec2 a, vec2 b, float thickness, vec4 color) {
		GizmoData g;
		g.model[0] = vec4(b - a, 0, 0);
		g.model[1] = vec4(-g.model[0].y * thickness, g.model[0].x * thickness, 0, 0);
		g.model[2] = vec4(0);
		float aspect = getAspect();
		a.x *= aspect;
		g.model[3] = vec4(vec3(a, 0) - 0.5f * (vec3)g.model[1], 1);
		g.model = glm::ortho(-aspect, aspect, -1.f, 1.f) * g.model;
		g.color = color;
		gizmoRenderQueue.push_back(g);
	}

	void queueGizmoSquareScreenspace(vec2 a, float size, vec4 color) {
		GizmoData g;
		g.model = (mat4)mat3(size);
		float aspect = getAspect();
		a.x *= aspect;
		g.model[3] = vec4(vec3(a, 0) - 0.5f * (vec3)g.model[0] - 0.5f * (vec3)g.model[1], 1);
		g.model = glm::ortho(-aspect, aspect, -1.f, 1.f) * g.model;
		g.color = color;
		gizmoRenderQueue.push_back(g);
	}

	void queueGizmo(GizmoData g) {
		gizmoRenderQueue.push_back(g);
	}

	void renderGizmos() {
		if (gizmoRenderQueue.size()) {
			glTempVar<GL_BLEND_ALPHA> blend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTempVar<GL_CULL_FACE> cull(false);

			static auto shader = get<Shader>("KittenEngine\\shaders\\gizmo.glsl");
			shader->use();
			gizmoBuffer->bind(3);
			glBindVertexArray(defMesh->VAO);

			int i = 0;
			for (; i < (int)gizmoRenderQueue.size() - MAX_GIZMO_PER_DRAW; i += MAX_GIZMO_PER_DRAW) {
				gizmoBuffer->upload(&gizmoRenderQueue[i]);
				glDrawElementsInstanced(shader->drawMode(), (GLsizei)defMesh->indices.size(), GL_UNSIGNED_INT, 0, MAX_GIZMO_PER_DRAW);
			}
			int leftOver = (int)gizmoRenderQueue.size() - i;
			gizmoBuffer->upload(&gizmoRenderQueue[i], leftOver);
			glDrawElementsInstanced(shader->drawMode(), (GLsizei)defMesh->indices.size(), GL_UNSIGNED_INT, 0, leftOver);

			shader->unuse();
			gizmoRenderQueue.clear();
		}
	}
}