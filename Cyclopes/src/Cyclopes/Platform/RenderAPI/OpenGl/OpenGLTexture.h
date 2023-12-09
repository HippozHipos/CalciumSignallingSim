#pragma once

#include "Cyclopes/Renderer/Texture.h"
#include <glad/glad.h>

namespace cyc {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint8_t slot = 0);
		OpenGLTexture2D(const char* path, uint8_t slot = 0);

	public:
		void Init(uint8_t slot = 0) override;
		void Bind(uint8_t slot = 0) override;
		void UnBind() override;
		void BufferData() override;
		void BufferData(int width, int height) override;

	private:
		GLuint m_GLTextureId = 0;
	};

}