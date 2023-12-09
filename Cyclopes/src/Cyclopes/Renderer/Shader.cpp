#include "Cycpch.h"
#include "Shader.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLShader.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	CycUnique<Shader> Shader::Create(Type type)
	{
		switch (type)
		{
			case Type::OPENGL: return CycMakeUnique<OpenGLShader>();
		}
		CYC_ASSERT(false, "Shader of given type could not be created");
	}
}