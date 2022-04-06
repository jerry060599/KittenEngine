
#include "MeshCCD.h"
#include "../includes/modules/Bound.h"
#include "../opt/polynomial.h"
#include <unordered_set>

namespace Kitten {
	// Determines when the tetrahedra formed by the four points in p becomes degenerate
	int planarMovingPoints(const mat4x3& p, const mat4x3& d, vec3& t) {
		vec3 e0 = p[1] - p[0];
		vec3 e1 = p[2] - p[0];
		vec3 e2 = p[3] - p[0];

		vec3 d0 = d[1] - d[0];
		vec3 d1 = d[2] - d[0];
		vec3 d2 = d[3] - d[0];

		vec3 e0xe1 = cross(e0, e1);
		vec3 d0xd1 = cross(d0, d1);
		vec3 cs = cross(e0, d1) + cross(d0, e1);

		float poly[4];
		poly[0] = dot(e0xe1, e2);
		poly[1] = dot(d2, e0xe1) + dot(e2, cs);
		poly[2] = dot(e2, d0xd1) + dot(d2, cs);
		poly[3] = dot(d0xd1, d2);

		return cy::PolynomialRoots<3>((float*)&t, poly);
	}

	// ccd between triangle formed by points 0,1,2 and point 3.
	bool intMovingTriPoint(const mat4x3& points, const mat4x3& deltas, float& t, vec3& bary) {
		vec3 ts;
		int nt = planarMovingPoints(points, deltas, ts);
		for (size_t i = 0; i < nt; i++) {
			if (ts[i] < 0) continue;
			if (ts[i] > 1) return false;

			mat4x3 x = points + deltas * ts[i];
			bary = baryCoord(x);
			if (all(greaterThanEqual(bary, vec3(0)))) {
				t = ts[i];
				return true;
			}
		}
		return false;
	}

	// ccd between edges formed by points 0,1 and 2,3.
	bool intMovingEdgeEdge(const mat4x3& points, const mat4x3& deltas, float& t, vec2& uv) {
		return false;
		vec3 ts;
		int nt = planarMovingPoints(points, deltas, ts);
		for (size_t i = 0; i < nt; i++) {
			if (ts[i] < 0) continue;
			if (ts[i] > 1) return false;
			mat4x3 x = points + deltas * ts[i];
			uv = lineClosestPoints(x[0], x[1], x[2], x[3]);
			if (lineHasInt(uv)) {
				t = ts[i];
				return true;
			}
		}
		return false;
	}

	void rtcErrorFunc(void* userPtr, enum RTCError error, const char* str) {
		printf("error %d: %s\n", error, str);
	}

	MeshCCD::MeshCCD() {
		rtcDevice = rtcNewDevice(nullptr);
		if (!rtcDevice)
			printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));
		rtcSetDeviceErrorFunction(rtcDevice, rtcErrorFunc, NULL);

		rtcScene = rtcNewScene(rtcDevice);
	}

	MeshCCD::~MeshCCD() {
		for (auto p : meshes) {
			rtcDetachGeometry(rtcScene, p.second->geomID);
			delete[] p.second->ownsEdge;
			delete[] p.second->ownsVert;
			delete p.second;
		}
		rtcReleaseScene(rtcScene);
		rtcReleaseDevice(rtcDevice);
	}

	bvec3* calcEdgeOwnership(Kitten::Mesh* mesh) {
		bvec3* ownership = new bvec3[mesh->indices.size() / 3];

		std::unordered_set<pair<int, int>> edgeSet;
		for (size_t i = 0; i < mesh->indices.size() / 3; i++) {
			bvec3 o(true);

			for (int k = 0; k < 3; k++) {
				auto e = std::make_pair(mesh->indices[k + 3 * i], mesh->indices[((k + 1) % 3) + 3 * i]);
				if (e.first > e.second) std::swap(e.first, e.second);
				if (edgeSet.count(e))
					o[k] = false;
				else {
					o[k] = true;
					edgeSet.insert(e);
				}
			}
			ownership[i] = o;
		}

		return ownership;
	}

	bvec3* calcVertOwnership(Kitten::Mesh* mesh) {
		bvec3* ownership = new bvec3[mesh->indices.size() / 3];
		bool* visited = new bool[mesh->vertices.size()];
		for (size_t i = 0; i < mesh->vertices.size(); i++)
			visited[i] = false;

		for (size_t i = 0; i < mesh->indices.size() / 3; i++) {
			bvec3 o(true);
			for (int k = 0; k < 3; k++) {
				bool* v = &visited[mesh->indices[k + 3 * i]];
				if (*v)
					o[k] = false;
				else {
					o[k] = true;
					*v = true;
				}
			}
			ownership[i] = o;
		}

		delete[] visited;
		return ownership;
	}

	glm::vec3*& MeshCCD::operator[](Kitten::Mesh* mesh) {
		auto itr = meshes.find(mesh);
		if (itr != meshes.end())
			return itr->second->delta;

		RTCMesh* ptr = new RTCMesh;
		*ptr = {};
		ptr->mesh = mesh;
		ptr->ownsEdge = calcEdgeOwnership(mesh);
		ptr->ownsVert = calcVertOwnership(mesh);
		meshes[mesh] = ptr;
		return ptr->delta;
	}

	void MeshCCD::attachStatic(Kitten::Mesh* mesh) {
		attach(mesh, nullptr);
	}

	void MeshCCD::attach(Kitten::Mesh* mesh, glm::vec3* delta) {
		(*this)[mesh] = delta;
	}

	void MeshCCD::detach(Kitten::Mesh* mesh) {
		auto itr = meshes.find(mesh);
		rtcDetachGeometry(rtcScene, itr->second->geomID);
		delete[] itr->second->ownsEdge;
		delete[] itr->second->ownsVert;
		delete itr->second;
		meshes.erase(itr);
	}

	void MeshCCD::dirty(Kitten::Mesh* mesh) {
		meshes[mesh]->dirty = true;
	}

	void boundFunc(const struct RTCBoundsFunctionArguments* args) {
		MeshCCD::RTCMesh& data = *(MeshCCD::RTCMesh*)args->geometryUserPtr;
		Kitten::Mesh& mesh = *data.mesh;

		ivec3 i = ivec3(0, 1, 2) + (int)(3 * args->primID);
		i = ivec3(mesh.indices[i[0]], mesh.indices[i[1]], mesh.indices[i[2]]);

		Kitten::Bound<> bound(mesh.vertices[i.x].pos);
		bound.absorb(mesh.vertices[i.y].pos);
		bound.absorb(mesh.vertices[i.z].pos);

		if (data.delta) {
			bound.absorb(mesh.vertices[i.x].pos + data.delta[i.x]);
			bound.absorb(mesh.vertices[i.y].pos + data.delta[i.y]);
			bound.absorb(mesh.vertices[i.z].pos + data.delta[i.z]);
		}

		*(vec3*)&args->bounds_o->lower_x = bound.min;
		*(vec3*)&args->bounds_o->upper_x = bound.max;
	}

	void MeshCCD::rebuildBVH() {
		// Loop through every mesh
		for (auto pair : meshes) {
			// Allocate rtcGeom and attach to scene if needed.
			if (pair.second->rtcGeom == nullptr) {
				// Allocate
				pair.second->rtcGeom = rtcNewGeometry(rtcDevice, RTC_GEOMETRY_TYPE_USER);
				rtcSetGeometryUserPrimitiveCount(pair.second->rtcGeom, pair.first->indices.size() / 3);
				rtcSetGeometryUserData(pair.second->rtcGeom, pair.second);
				rtcSetGeometryBoundsFunction(pair.second->rtcGeom, boundFunc, pair.first);
				rtcSetGeometryIntersectFunction(pair.second->rtcGeom, nullptr);
				rtcSetGeometryOccludedFunction(pair.second->rtcGeom, nullptr);

				// Attach
				rtcCommitGeometry(pair.second->rtcGeom);
				pair.second->geomID = rtcAttachGeometry(rtcScene, pair.second->rtcGeom);
				rtcReleaseGeometry(pair.second->rtcGeom);

				pair.second->dirty = false;
			}

			if (pair.second->dirty) {
				pair.second->dirty = false;
				// Recommit
				rtcCommitGeometry(pair.second->rtcGeom);
			}
		}

		rtcCommitScene(rtcScene);
	}

	void MeshCCD::triangleCCD(struct RTCCollision* collisions, unsigned int num_collisions, std::function<void(TriVertCollision)> triVertColCallback, std::function<void(EdgeEdgeCollision)> edgeEdgeColCallback) {
		for (int i = 0; i < num_collisions; i++) {
			const RTCCollision& col = collisions[i];
			if (col.geomID0 == col.geomID1 && col.primID0 == col.primID1) continue;
			const RTCMesh& artc = *(RTCMesh*)rtcGetGeometryUserData(rtcGetGeometry(rtcScene, col.geomID0));
			const RTCMesh& brtc = *(RTCMesh*)rtcGetGeometryUserData(rtcGetGeometry(rtcScene, col.geomID1));
			Mesh& a = *artc.mesh;
			Mesh& b = *brtc.mesh;

			// Get indices
			ivec3 aIndices = ivec3(0, 1, 2) + 3 * (int)col.primID0;
			aIndices = ivec3(a.indices[aIndices[0]], a.indices[aIndices[1]], a.indices[aIndices[2]]);
			ivec3 bIndices = ivec3(0, 1, 2) + 3 * (int)col.primID1;
			bIndices = ivec3(b.indices[bIndices[0]], b.indices[bIndices[1]], b.indices[bIndices[2]]);

			// Get positions
			mat3 apos(a.vertices[aIndices[0]].pos, a.vertices[aIndices[1]].pos, a.vertices[aIndices[2]].pos);
			mat3 bpos(b.vertices[bIndices[0]].pos, b.vertices[bIndices[1]].pos, b.vertices[bIndices[2]].pos);

			// Get positional deltas
			mat3 adelta(0);
			if (artc.delta) adelta = mat3(artc.delta[aIndices[0]], artc.delta[aIndices[1]], artc.delta[aIndices[2]]);
			mat3 bdelta(0);
			if (brtc.delta) bdelta = mat3(brtc.delta[bIndices[0]], brtc.delta[bIndices[1]], brtc.delta[bIndices[2]]);

			// Perform triangle-vertex tests
			for (int tri = 0; tri < 2; tri++) {
				mat4x3 pos(tri ? bpos : apos);
				mat4x3 delta(tri ? bdelta : adelta);
				bvec3 ownsVert = tri ? artc.ownsVert[col.primID0] : brtc.ownsVert[col.primID1];

				for (int k = 0; k < 3; k++) {
					if (!ownsVert[k]) continue;
					// Ignore the vertex if its part of the triangle.
					if (&a == &b && any(equal(tri ? bIndices : aIndices, ivec3(tri ? aIndices[k] : bIndices[k]))))
						continue;

					float t;
					vec3 bary;
					pos[3] = tri ? apos[k] : bpos[k];
					delta[3] = tri ? adelta[k] : bdelta[k];

					if (intMovingTriPoint(pos, delta, t, bary)) {
						// Found intersection
						mat4x3 cur = pos + t * delta;
						vec3 norm = cross(cur[1] - cur[0], cur[2] - cur[0]);
						norm *= sign(dot(norm, mat3(delta) * bary - delta[3]));

						// call callback
						TriVertCollision dat;
						dat.triMesh = tri ? &b : &a;
						dat.triIndex = tri ? col.primID1 : col.primID0;

						dat.vertMesh = tri ? &a : &b;
						dat.vertIndex = tri ? aIndices[k] : bIndices[k];

						dat.t = t;
						dat.bary = bary;
						dat.norm = norm;

						triVertColCallback(dat);
					}
				}
			}

			// Perform edge-edge tests
			bvec3 aOwnsEdge = artc.ownsEdge[col.primID0];
			bvec3 bOwnsEdge = brtc.ownsEdge[col.primID1];
			for (int ka = 0; ka < 3; ka++) {
				if (!aOwnsEdge[ka]) continue;
				ivec2 aEdgeOrder(ka, (ka + 1) % 3);
				mat4x3 pos;
				mat4x3 delta;

				pos[0] = apos[aEdgeOrder[0]];
				pos[1] = apos[aEdgeOrder[1]];
				delta[0] = adelta[aEdgeOrder[0]];
				delta[1] = adelta[aEdgeOrder[1]];

				for (int kb = 0; kb < 3; kb++) {
					if (!bOwnsEdge[kb]) continue;
					ivec2 bEdgeOrder(kb, (kb + 1) % 3);

					// Ignore collision if the edges share a vertex
					if (&a == &b) {
						ivec2 edge(aIndices[aEdgeOrder.x], aIndices[aEdgeOrder.y]);
						if (any(equal(edge, ivec2(bIndices[bEdgeOrder.x]))) ||
							any(equal(edge, ivec2(bIndices[bEdgeOrder.y]))))
							continue;
					}

					pos[2] = bpos[bEdgeOrder[0]];
					pos[3] = bpos[bEdgeOrder[1]];
					delta[2] = bdelta[bEdgeOrder[0]];
					delta[3] = bdelta[bEdgeOrder[1]];

					float t;
					vec2 uv;
					if (intMovingEdgeEdge(pos, delta, t, uv)) {
						// Found intersection
						mat4x3 cur = pos + t * delta;
						vec3 norm = cross(pos[1] - pos[0], pos[2] - pos[0]);
						norm *= sign(dot(norm, mix(delta[0], delta[1], uv.x) - mix(delta[2], delta[3], uv.y)));

						// call callback
						EdgeEdgeCollision dat;
						dat.aMesh = &a;
						dat.bMesh = &b;

						dat.ai = ivec2(aIndices[aEdgeOrder.x], aIndices[aEdgeOrder.y]);
						dat.bi = ivec2(bIndices[bEdgeOrder.x], bIndices[bEdgeOrder.y]);

						dat.t = t;
						dat.uv = uv;
						dat.norm = norm;

						edgeEdgeColCallback(dat);
					}
				}
			}
		}
	}

	void MeshCCD::collide(std::function<void(TriVertCollision)> triVertColCallback, std::function<void(EdgeEdgeCollision)> edgeEdgeColCallback) {
		struct params {
			MeshCCD* ccd;
			std::function<void(TriVertCollision)> tvf;
			std::function<void(EdgeEdgeCollision)> eef;
		};
		params p{ this, triVertColCallback, edgeEdgeColCallback };

		rtcCollide(rtcScene, rtcScene, [](void* userPtr, struct RTCCollision* collisions, unsigned int num_collisions) {
			params& p = *(params*)userPtr;
			p.ccd->triangleCCD(collisions, num_collisions, p.tvf, p.eef);
			}, &p);
	}
}