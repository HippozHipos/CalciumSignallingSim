#include "Cycpch.h"
#include "OpenGLVertexArray.h"
#include "OpenGLError.h"

#include "glad/glad.h"

namespace cyc {

	void VertexArray::Init()
	{
		glGenVertexArrays(1, &m_GlBufferId);
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_GlBufferId);
	}

	void VertexArray::UnBind()
	{
		glBindVertexArray(0);
	}
	
}