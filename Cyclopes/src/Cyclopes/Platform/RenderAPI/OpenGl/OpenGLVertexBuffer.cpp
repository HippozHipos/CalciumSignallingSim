#include "Cycpch.h"
#include "OpenGLVertexBuffer.h"

namespace cyc {
	void OpenGLVertexBuffer::Init()
	{
		glGenBuffers(1, &m_GlBufferId);
	}

	void OpenGLVertexBuffer::BufferData(void* data, long long int size, bool staticDraw) const
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::BufferData(long long int size, bool staticDraw) const
	{
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_GlBufferId);
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::Memcpy(void* data, std::size_t size) const
	{
		void* buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(buf, data, size);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	std::size_t OpenGLVertexBuffer::GetSize() const
	{
		GLint size = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		return (std::size_t)size;
	}

	void OpenGLVertexBuffer::BufferSubData(void* data, std::size_t size, std::size_t offset) const
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}

	void OpenGLVertexBuffer::Destroy()
	{
		glDeleteBuffers(1, &m_GlBufferId);
	}

}