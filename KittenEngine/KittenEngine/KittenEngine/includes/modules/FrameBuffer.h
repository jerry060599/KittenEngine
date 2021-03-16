#pragma once
#include "Texture.h"

namespace Kitten {
	class FrameBuffer {
	public:
		bool managed = false;
		int width, height;
		unsigned int glHandle = 0;
		unsigned int depthStencil = 0;
		Texture* buffs[8] = {};
		vec4 lastViewport;

		FrameBuffer();
		FrameBuffer(int width, int height, int numGBuffers = 1);
		~FrameBuffer();

		void resize(int width, int height);
		void bind();
		void unbind();
	};
}