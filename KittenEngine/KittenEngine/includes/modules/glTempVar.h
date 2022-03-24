#pragma once
// Jerry Hsu, 2022

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Kitten {
	using namespace glm;

	inline constexpr GLenum GL_BLEND_ALPHA = GL_BLEND_SRC_ALPHA;

	template<typename T>
	class glTempVar {
	public:
		glTempVar(GLenum key, T val);
	};

	inline void glSetBool(const GLenum key, bool val) {
		switch (key) {
		case GL_BLEND: if (val) glEnable(GL_BLEND); else glDisable(GL_BLEND); break;
		case GL_CULL_FACE: if (val) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE); break;
		case GL_DEPTH_TEST: if (val) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST); break;
		case GL_DEPTH_WRITEMASK: if (val) glEnable(GL_DEPTH_WRITEMASK); else glDisable(GL_DEPTH_WRITEMASK); break;
		case GL_DITHER: if (val) glEnable(GL_DITHER); else glDisable(GL_DITHER); break;
		case GL_LINE_SMOOTH: if (val) glEnable(GL_LINE_SMOOTH); else glDisable(GL_LINE_SMOOTH); break;
		case GL_PROGRAM_POINT_SIZE: if (val) glEnable(GL_PROGRAM_POINT_SIZE); else glDisable(GL_PROGRAM_POINT_SIZE); break;
		case GL_POLYGON_SMOOTH: if (val) glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH); break;
		case GL_SCISSOR_TEST: if (val) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<bool>");
		}
	}

	inline void glSetInt(const GLenum key, int val) {
		switch (key) {
		case GL_ACTIVE_TEXTURE: glActiveTexture(val); break;
		case GL_ARRAY_BUFFER_BINDING: glBindBuffer(GL_ARRAY_BUFFER, val); break;
		case GL_COLOR_LOGIC_OP: glLogicOp(val); break;
		case GL_CULL_FACE_MODE: glCullFace(val); break;
		case GL_CURRENT_PROGRAM: glUseProgram(val); break;
		case GL_DEPTH_FUNC: glDepthFunc(val); break;
		case GL_DRAW_BUFFER: glDrawBuffer(val); break;
		case GL_DRAW_FRAMEBUFFER_BINDING: glBindFramebuffer(GL_DRAW_FRAMEBUFFER, val); break;
		case GL_READ_FRAMEBUFFER_BINDING: glBindFramebuffer(GL_READ_FRAMEBUFFER, val); break;
		case GL_ELEMENT_ARRAY_BUFFER_BINDING: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, val); break;
		case GL_FRAGMENT_SHADER_DERIVATIVE_HINT: glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, val); break;
		case GL_LINE_SMOOTH_HINT: glHint(GL_LINE_SMOOTH_HINT, val); break;
		case GL_LOGIC_OP_MODE: glLogicOp(val); break;
		case GL_PACK_ALIGNMENT: glPixelStorei(GL_PACK_ALIGNMENT, val); break;
		case GL_PACK_IMAGE_HEIGHT: glPixelStorei(GL_PACK_IMAGE_HEIGHT, val); break;
		case GL_PACK_LSB_FIRST: glPixelStorei(GL_PACK_LSB_FIRST, val); break;
		case GL_PACK_ROW_LENGTH: glPixelStorei(GL_PACK_ROW_LENGTH, val); break;
		case GL_PACK_SKIP_IMAGES: glPixelStorei(GL_PACK_SKIP_IMAGES, val); break;
		case GL_PACK_SKIP_PIXELS: glPixelStorei(GL_PACK_SKIP_PIXELS, val); break;
		case GL_PACK_SKIP_ROWS: glPixelStorei(GL_PACK_SKIP_ROWS, val); break;
		case GL_PACK_SWAP_BYTES: glPixelStorei(GL_PACK_SWAP_BYTES, val); break;
		case GL_PIXEL_PACK_BUFFER_BINDING: glBindBuffer(GL_PIXEL_PACK_BUFFER, val); break;
		case GL_PIXEL_UNPACK_BUFFER_BINDING: glBindBuffer(GL_PIXEL_UNPACK_BUFFER, val); break;
		case GL_PRIMITIVE_RESTART_INDEX: glPrimitiveRestartIndex(val); break;
		case GL_PROGRAM_PIPELINE_BINDING: glBindProgramPipeline(val); break;
		case GL_PROVOKING_VERTEX: glProvokingVertex(val); break;
		case GL_POLYGON_SMOOTH_HINT: glHint(GL_POLYGON_SMOOTH_HINT, val); break;
		case GL_READ_BUFFER: glReadBuffer(val); break;
		case GL_RENDERBUFFER_BINDING: glBindRenderbuffer(GL_RENDERBUFFER, val); break;
		case GL_SHADER_STORAGE_BUFFER_BINDING: glBindBuffer(GL_SHADER_STORAGE_BUFFER, val); break;
		case GL_STENCIL_BACK_WRITEMASK: glStencilMaskSeparate(GL_BACK, val); break;
		case GL_STENCIL_CLEAR_VALUE: glClearStencil(val); break;
		case GL_TEXTURE_BINDING_1D: glBindTexture(GL_TEXTURE_1D, val); break;
		case GL_TEXTURE_BINDING_1D_ARRAY: glBindTexture(GL_TEXTURE_1D_ARRAY, val); break;
		case GL_TEXTURE_BINDING_2D: glBindTexture(GL_TEXTURE_2D, val); break;
		case GL_TEXTURE_BINDING_2D_ARRAY: glBindTexture(GL_TEXTURE_2D_ARRAY, val); break;
		case GL_TEXTURE_BINDING_2D_MULTISAMPLE: glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, val); break;
		case GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY: glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, val); break;
		case GL_TEXTURE_BINDING_3D: glBindTexture(GL_TEXTURE_3D, val); break;
		case GL_TEXTURE_BINDING_BUFFER: glBindTexture(GL_TEXTURE_BUFFER, val); break;
		case GL_TEXTURE_BINDING_CUBE_MAP: glBindTexture(GL_TEXTURE_CUBE_MAP, val); break;
		case GL_TEXTURE_BINDING_RECTANGLE: glBindTexture(GL_TEXTURE_RECTANGLE, val); break;
		case GL_TEXTURE_COMPRESSION_HINT: glHint(GL_TEXTURE_COMPRESSION_HINT, val); break;
		case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING: glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, val); break;
		case GL_UNIFORM_BUFFER_BINDING: glBindBuffer(GL_UNIFORM_BUFFER, val); break;
		case GL_UNPACK_ALIGNMENT: glPixelStorei(GL_UNPACK_ALIGNMENT, val); break;
		case GL_UNPACK_IMAGE_HEIGHT: glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, val); break;
		case GL_UNPACK_LSB_FIRST: glPixelStorei(GL_UNPACK_LSB_FIRST, val); break;
		case GL_UNPACK_ROW_LENGTH: glPixelStorei(GL_UNPACK_ROW_LENGTH, val); break;
		case GL_UNPACK_SKIP_IMAGES: glPixelStorei(GL_UNPACK_SKIP_IMAGES, val); break;
		case GL_UNPACK_SKIP_PIXELS: glPixelStorei(GL_UNPACK_SKIP_PIXELS, val); break;
		case GL_UNPACK_SKIP_ROWS: glPixelStorei(GL_UNPACK_SKIP_ROWS, val); break;
		case GL_UNPACK_SWAP_BYTES: glPixelStorei(GL_UNPACK_SWAP_BYTES, val); break;
		case GL_VERTEX_ARRAY_BINDING: glBindVertexArray(val); break;
		case GL_POLYGON_MODE: glPolygonMode(GL_FRONT_AND_BACK, val); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<int>");
		}
	}

	inline void glSetFloat(const GLenum key, float val) {
		switch (key) {
		case GL_DEPTH_CLEAR_VALUE: glClearDepthf(val); break;
		case GL_LINE_WIDTH: glLineWidth(val); break;
		case GL_POINT_FADE_THRESHOLD_SIZE: glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, val); break;
		case GL_POINT_SIZE: glPointSize(val); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<float>");
		}
	}

	inline void glSetVec2(const GLenum key, vec2 val) {
		switch (key) {
		case GL_DEPTH_RANGE:  glDepthRangef(val.x, val.y); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<Range>");
		}
	}

	inline void glSetIvec2(const GLenum key, ivec2 val) {
		switch (key) {
		case GL_BLEND_DST_ALPHA: case GL_BLEND_SRC_ALPHA: glBlendFunc(val.x, val.y); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<Range>");
		}
	}

	inline void glSetVec4(const GLenum key, vec4 val) {
		switch (key) {
		case GL_BLEND_COLOR: glBlendColor(val.x, val.y, val.z, val.w); break;
		case GL_COLOR_CLEAR_VALUE: glClearColor(val.x, val.y, val.z, val.w); break;


		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<vec4>");
		}
	}

	inline void glSetIvec4(const GLenum key, ivec4 val) {
		switch (key) {
		case GL_SCISSOR_BOX: glScissor(val[0], val[1], val[2], val[3]); break;
		case GL_VIEWPORT: glViewport(val[0], val[1], val[2], val[3]); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<ivec4>");
		}
	}

	inline void glSetBvec4(const GLenum key, bvec4 val) {
		switch (key) {
		case GL_COLOR_WRITEMASK: glColorMask(val[0], val[1], val[2], val[3]); break;

		default:
			throw new exception("Invalid GLenum for Kitten: : glTempVar<ivec4>");
		}
	}

	template <>
	class glTempVar<bool> {
	public:
		const GLenum key;
		const GLboolean oldVal;

		glTempVar(GLenum key, bool val) : key(key), oldVal(0) {
			glGetBooleanv(key, (GLboolean*)&oldVal);
			glSetBool(key, val);
		}

		~glTempVar() {
			glSetBool(key, oldVal);
		}
	};

	template <>
	class glTempVar<int> {
	public:
		const GLenum key;
		const int oldVal;

		glTempVar(GLenum key, int val) : key(key), oldVal(0) {
			glGetIntegerv(key, (int*)&oldVal);
			glSetInt(key, val);
		}

		~glTempVar() {
			glSetInt(key, oldVal);
		}
	};

	template <>
	class glTempVar<unsigned int> {
	public:
		const GLenum key;
		const unsigned int oldVal;

		glTempVar(GLenum key, unsigned int val) : key(key), oldVal(0) {
			glGetIntegerv(key, (int*)&oldVal);
			glSetInt(key, val);
		}

		~glTempVar() {
			glSetInt(key, oldVal);
		}
	};

	template <>
	class glTempVar<float> {
	public:
		const GLenum key;
		const float oldVal;

		glTempVar(GLenum key, float val) : key(key), oldVal(0) {
			glGetFloatv(key, (float*)&oldVal);
			glSetFloat(key, val);
		}

		~glTempVar() {
			glSetFloat(key, oldVal);
		}
	};

	template <>
	class glTempVar<vec2> {
	public:
		const GLenum key;
		const vec2 oldVal;

		glTempVar(GLenum key, vec2 val) : key(key), oldVal(vec2(0)) {
			glGetFloatv(key, (float*)&oldVal);
			glSetVec2(key, val);
		}

		glTempVar(GLenum key, float a, float b) : glTempVar(key, vec2(a, b)) {};

		~glTempVar() {
			glSetVec2(key, oldVal);
		}
	};

	template <>
	class glTempVar<ivec2> {
	public:
		const GLenum key;
		const ivec2 oldVal;

		glTempVar(GLenum key, ivec2 val) : key(key), oldVal(ivec2(0)) {
			if (key == GL_BLEND_SRC_ALPHA || key == GL_BLEND_DST_ALPHA) {
				glGetIntegerv(GL_BLEND_SRC_ALPHA, (int*)&oldVal.x);
				glGetIntegerv(GL_BLEND_DST_ALPHA, (int*)&oldVal.y);
			}
			else
				glGetIntegerv(key, (int*)&oldVal);
			glSetIvec2(key, val);
		}

		glTempVar(GLenum key, int a, int b) : glTempVar(key, ivec2(a, b)) {};

		~glTempVar() {
			glSetIvec2(key, oldVal);
		}
	};

	template <>
	class glTempVar<vec4> {
	public:
		const GLenum key;
		const vec4 oldVal;

		glTempVar(GLenum key, vec4 val) : key(key), oldVal(vec4(0)) {
			glGetFloatv(key, (float*)&oldVal);
			glSetVec4(key, val);
		}

		glTempVar(GLenum key, float a, float b, float c, float d) : glTempVar(key, vec4(a, b, c, d)) {};

		~glTempVar() {
			glSetVec4(key, oldVal);
		}
	};

	template <>
	class glTempVar<ivec4> {
	public:
		const GLenum key;
		const ivec4 oldVal;

		glTempVar(GLenum key, ivec4 val) : key(key), oldVal(ivec4(0)) {
			glGetIntegerv(key, (int*)&oldVal);
			glSetIvec4(key, val);
		}

		glTempVar(GLenum key, int a, int b, int c, int d) : glTempVar(key, ivec4(a, b, c, d)) {};

		~glTempVar() {
			glSetIvec4(key, oldVal);
		}
	};

	template <>
	class glTempVar<bvec4> {
	public:
		const GLenum key;
		const GLboolean oldVal[4]{};

		glTempVar(GLenum key, bvec4 val) : key(key) {
			glGetBooleanv(key, (GLboolean*)oldVal);
			glSetBvec4(key, val);
		}

		glTempVar(GLenum key, bool a, bool b, bool c, bool d) : glTempVar(key, bvec4(a, b, c, d)) {};

		~glTempVar() {
			glSetBvec4(key, bvec4(oldVal[0], oldVal[1], oldVal[2], oldVal[3]));
		}
	};
}