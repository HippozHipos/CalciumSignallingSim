#pragma once

#include "Cyclopes/Core/Base.h"
#include "glad/glad.h"

namespace cyc {

	class IndexBuffer
	{
	public:
		enum class Type
		{
			OPENGL,
			DX12
		};

	public:
		static CycUnique<IndexBuffer> Create(Type type);
		virtual void Init() = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0; 
		virtual void BufferData(void* data, long long size, bool staticDraw = true) const = 0;
		virtual void BufferData(long long size, bool staticDraw = true) const = 0;
		virtual std::size_t GetSize() const = 0;
		virtual void BufferSubData(void* data, std::size_t size, std::size_t offset) const = 0;
		virtual void Destroy() = 0;	
	
	protected:
		GLuint m_GlBufferId;
;
	};

}
