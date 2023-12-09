#pragma once

#include "Cyclopes/Renderer/VertexBuffer.h"

#include "Cyclopes/Core/Base.h"
#include "Cyclopes/Core/Assert.h"
#include "OpenGLError.h"

#include <vector>
#include <glad/glad.h>


namespace cyc {

	enum class CYC_GL_BUFFER_TYPE
	{
		ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER,
		COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER,
		SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER
	};

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		void Init() override;
		void Bind() const override;
		void UnBind() const override;
		void BufferData(void* data, long long int size, bool staticDraw = true) const override;
		void BufferData(long long int size, bool staticDraw = true) const override;
		void Memcpy(void* data, std::size_t size) const override;
		std::size_t GetSize() const override;
		void BufferSubData(void* data, std::size_t size, std::size_t offset) const override;
		void Destroy() override;
	};

}

