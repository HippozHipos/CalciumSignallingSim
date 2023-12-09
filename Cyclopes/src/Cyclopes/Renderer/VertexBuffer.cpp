#include "Cycpch.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLVertexBuffer.h"

namespace cyc {

	CycUnique<VertexBuffer> cyc::VertexBuffer::Create(Type type)
	{
		switch (type)
		{
			case Type::OPENGL: return CycMakeUnique<OpenGLVertexBuffer>();
		}
		CYC_ASSERT(false, "Vertex buffer of given type could not be created");
	}

}
