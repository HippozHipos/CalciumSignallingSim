#include "Cycpch.h"
#include "GraphicsContext.h"

#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLContext.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	CycUnique<GraphicsContext> GraphicsContext::Create(Type type)
	{
		switch ((int)type)
		{
			case (int)Type::OPENGL: 
			{
				return std::make_unique<OpenGLContext>();
			}
		}

		CYC_ASSERT(false, "Graphics context of given type could not be created");
	}
}