#include "OpenGLTexture.h"
#include "Cyclopes/Core/Log.h"

namespace cyc {

	OpenGLTexture2D::OpenGLTexture2D(uint8_t slot)
	{
		Init(slot);
	}

	OpenGLTexture2D::OpenGLTexture2D(const char* path, uint8_t slot) :
		Texture2D{path}
	{
		Init(slot);
		BufferData();
	}

	void OpenGLTexture2D::Init(uint8_t slot)
	{
		glGenTextures(1, &m_GLTextureId);
		Bind(slot);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void OpenGLTexture2D::Bind(uint8_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_GLTextureId);
	}

	void OpenGLTexture2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::BufferData()
	{
		if (m_Data == nullptr)
		{
			CYC_WARN("Opengl texture attempting to Buffer data when data is null");
		}
		glTexImage2D(GL_TEXTURE_2D, m_MitMapLevel, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);
	}

	void OpenGLTexture2D::BufferData(int width, int height)
	{
		m_Width = width;
		m_Height = height;
		BufferData();
	}
}