#include "Cycpch.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLIndexBuffer.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	CycUnique<IndexBuffer> IndexBuffer::Create(Type type) 
	{
		switch (type)
		{
			case Type::OPENGL: return CycMakeUnique<OpenGLIndexBuffer>();
		}
		CYC_ASSERT(false, "Index buffer of given type could not be created");
	}

}
