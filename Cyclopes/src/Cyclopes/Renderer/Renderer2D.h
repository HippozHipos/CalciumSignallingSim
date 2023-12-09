#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "Cyclopes/Renderer/VertexBuffer.h"
#include "Cyclopes/Renderer/IndexBuffer.h"
#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLVertexArray.h"
#include "Cyclopes/Renderer/Shader.h"
#include "Cyclopes/Renderer/Camera2D.h"

namespace cyc {

	struct Rect2DDesc
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 50.0f;
		float height = 50.0f;
		float r = 1.0f;
		float g = 0.0f;
		float b = 1.0f;
		float a = 1.0f;
		float rotation = 0.0f;
		int texture = 0;
	};

	class Renderer2D
	{
	private:
		using DataType = float;
		using Vec2 = glm::vec<2, DataType, glm::packed_lowp>;
		using Vec4 = glm::vec<4, DataType, glm::packed_lowp>;

	public:
		static void FillRect(const Vec2& position, const Vec2& size, const char* texture, const Vec4& tint, float rotation=0.0f);
		static void FillRect(const Vec2& position, const Vec2& size, const char* texture, float rotation);
		static void FillRect(const Vec2& position, const Vec2& size, const Vec4& color, float rotation);
		static void FillRect(const Vec2& position, const Vec2& size, const Vec4& color);
		static void FillRect(const Vec2& position, const Vec2& size, const char* texture);
		static void FillRect(Rect2DDesc desc);

	public:
		struct Stats
		{
			static int NTotalBatches();
			static int NIndicesDrawn();
			static int NVerticesDrawn();
			static int MaxIndicesPerBatch();
			static int NQuadsDrawn();
			static int NTrianglesDrawn();
		};

	private:
		struct Vertex
		{
			Vec2 position = { 0.0f, 0.0f };
			Vec4 color = { 1.0f, 0.0f, 1.0f, 1.0f };
			Vec2 texture = { 0.0f, 0.0f };
			DataType textureID = 0;
		};

		struct Quad
		{
			Vertex vertex[4];
		};

		struct VertexTexMap
		{
			Vec2 position[4];
		};

	public:
		static void SetCamera(const Camera2D& camera);

	public:
		static void _Init();
		static void _StartBatches();
		static void _Flush();

	private:
		static void DrawNextBatch();
		static void BufferData();

	private:
		static void AddQuad(const Vec2& translation, const Vec2& size, const Vec4& color, int texture, float rotation);
		static Quad MakeQuad(const Vec2& translation, const Vec2& size, const Vec4& color, int texture, float rotation);
		
		static void AddIndices();

		static constexpr VertexTexMap GetVertexTexMap();

	private:
		static CycUnique<cyc::VertexBuffer> m_VertexBuffer;
		static CycUnique<cyc::IndexBuffer> m_IndexBuffer;
		static CycUnique<cyc::Shader> m_Shader;
		static cyc::VertexArray m_VertexArray;

	private:
		static CycVector<Vertex> m_DynamicVertices;
		static CycVector<std::uint32_t> m_DynamicIndices;

	private:
		static int m_CurrentBatch;
		static int m_TotalBatches;
		static int m_IndicesDrawn;
		static constexpr int m_MaxBatchSize = 1024 * 1024 * 2; //2mb
		static constexpr int m_VertexStride = 9; 
		static constexpr int m_IndicesPerBatch = (m_MaxBatchSize / (sizeof(Vertex) * 4)) * 6; //how many indices in 2mb of vertex data

	private:
		static Camera2D m_Camera;
		
	};

}

