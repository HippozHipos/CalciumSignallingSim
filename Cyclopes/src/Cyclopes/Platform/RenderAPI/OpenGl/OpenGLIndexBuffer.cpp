#include "Cycpch.h"
#include "OpenGLIndexBuffer.h"

#include "glad/glad.h"

namespace cyc {
	void OpenGLIndexBuffer::Init()
	{
		glGenBuffers(1, &m_GlBufferId);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GlBufferId);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::BufferData(void* data, long long size, bool staticDraw) const
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void OpenGLIndexBuffer::BufferData(long long size, bool staticDraw) const
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	std::size_t OpenGLIndexBuffer::GetSize() const
	{
		GLint size = 0;
		glBindBuffer(GL_ARRAY_BUFFER, m_GlBufferId);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		return (std::size_t)size;
	}

	void OpenGLIndexBuffer::BufferSubData(void* data, std::size_t size, std::size_t offset) const
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
	}

	void OpenGLIndexBuffer::Destroy()
	{
		glDeleteBuffers(1, &m_GlBufferId);
	}
}