#pragma once

#include "Cyclopes/Renderer/IndexBuffer.h"

#include "Cyclopes/Core/Base.h"

namespace cyc {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		void Init() override;
		void Bind() const override;
		void UnBind() const override;
		void BufferData(void* data, long long size, bool staticDraw = true) const override;
		void BufferData(long long size, bool staticDraw = true) const override;
		std::size_t GetSize() const override;
		void BufferSubData(void* data, std::size_t size, std::size_t offset) const override;
		void Destroy() override;
	};

}