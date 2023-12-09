#pragma once

#include "Cyclopes/Core/Base.h"
#include "Cyclopes/Renderer/Cycsh.h"
#include "glm/glm.hpp"

namespace cyc {

	class Shader
	{
	public:
		enum class Type
		{
			OPENGL,
			DX12
		};

	public:
		static CycUnique<Shader> Create(Type type);
		virtual void Init(const Cycsh& source) = 0;
		virtual void Bind() = 0;
		virtual void Destroy() = 0;

	public:
		virtual void SetMat4f(const CycString& name, const glm::mat4& value) = 0;
		virtual void SetInt(const CycString& name, int value) = 0;
		virtual void SetIntBuffer(const CycString& name, int* value, int size) = 0;
	};
}
