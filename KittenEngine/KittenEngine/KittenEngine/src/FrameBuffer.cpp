#include <glad/glad.h>
#include "../includes/modules/FrameBuffer.h"

namespace Kitten {
	FrameBuffer::FrameBuffer() {}
	FrameBuffer::FrameBuffer(int width, int height, int numGBuffers)
		:managed(true), width(width), height(height) {
		glGenFramebuffers(1, &glHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, glHandle);

		glGenTextures(1, &depthStencil);
		glBindTexture(GL_TEXTURE_2D, depthStencil);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);

		for (size_t i = 0; i < numGBuffers; i++) {
			buffs[i] = new Texture(width, height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GLenum(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D, buffs[i]->glHandle, 0);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("error: fbo creation failure!!\n");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer() {
		if (managed) {
			int i = 0;
			while (buffs[i] && i < 8) delete buffs[i];

			glDeleteTextures(1, &depthStencil);
			glDeleteFramebuffers(1, &glHandle);
		}
	}

	void FrameBuffer::resize(int nw, int nh) {
		if (nw == width && nh == height) return;
		width = nw;
		height = nh;
		glBindFramebuffer(GL_FRAMEBUFFER, glHandle);

		int i = 0;
		while (buffs[i] && i < 8) buffs[i++]->resize(nw, nh);

		glBindTexture(GL_TEXTURE_2D, depthStencil);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::bind() {
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, glHandle);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}