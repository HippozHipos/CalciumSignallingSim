#include "Cycpch.h"
#include "RendererAPI.h"
#include "Cyclopes/Core/Assert.h"

#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLRendererAPI.h"

namespace cyc {

	RendererAPI::Type RendererAPI::m_Type;

	CycUnique<RendererAPI> RendererAPI::Create(RendererAPI::Type type)
	{
		switch ((int)type)
		{
			case (int)RendererAPI::Type::OPENGL: 
			{
				m_Type = type; 
				return CycMakeUnique<OpenGLRendererAPI>();
			}
		}

		CYC_ASSERT(false, "RendererAPI creation failed. RendererAPI::Type is not valid.");
		return nullptr;
	}

	RendererAPI::Type RendererAPI::GetType()
	{
		return m_Type;
	}
}