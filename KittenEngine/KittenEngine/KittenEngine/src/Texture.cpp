#include "../includes/modules/Texture.h"

namespace Kitten {
	Texture::Texture() {}
	Texture::Texture(int width, int height, GLenum channels, GLenum dataType)
		:width(width), height(height), channels(channels), dataType(dataType), ratio(float(width) / height) {
		glGenTextures(1, &glHandle);
		glBindTexture(GL_TEXTURE_2D, glHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, dataType, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(Texture* xpos, Texture* xneg, Texture* ypos, Texture* yneg, Texture* zpos, Texture* zneg) {
		isCubeMap = true;
		glGenTextures(1, &glHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glHandle);

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			0, GL_RGB, xpos->width, xpos->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, xpos->rawData
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			0, GL_RGB, xneg->width, xneg->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, xneg->rawData
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			0, GL_RGB, ypos->width, ypos->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ypos->rawData
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			0, GL_RGB, yneg->width, yneg->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, yneg->rawData
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			0, GL_RGB, zpos->width, zpos->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, zpos->rawData
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			0, GL_RGB, zneg->width, zneg->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, zneg->rawData
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	Texture::~Texture() {
		glDeleteTextures(1, &glHandle);
		if (rawData) delete[] rawData;
	}

	void Texture::genMipmap() {
		glBindTexture(GL_TEXTURE_2D, glHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::setAniso(float v) {
		glBindTexture(GL_TEXTURE_2D, glHandle);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, v);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::resize(int nw, int nh) {
		if (nw == width && nh == height) return;
		width = nw;
		height = nh;
		glBindTexture(GL_TEXTURE_2D, glHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, dataType, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}