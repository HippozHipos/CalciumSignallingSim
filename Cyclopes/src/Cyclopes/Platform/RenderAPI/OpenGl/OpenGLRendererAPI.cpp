#include "Cycpch.h"
#include "OpenGLRendererAPI.h"
#include "Cyclopes/Core/Log.h"

namespace cyc {

	void OpenGLRendererAPI::Clear(float r, float g, float b, float a) const
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexedVertices(int indexCount) const
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedVertices(int indexCount, int offset) const
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)offset);
	}
}
