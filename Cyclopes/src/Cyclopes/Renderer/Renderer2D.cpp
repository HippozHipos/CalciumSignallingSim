#include "Cycpch.h"
#include "Renderer2D.h"

#include "Cyclopes/Renderer/Graphics.h"
#include "Cyclopes/Renderer/Texture.h"
#include "Cyclopes/Renderer/Cycsh.h"

#include "Cyclopes/Core/ResourceManager.h"
#include "Cyclopes/Core/Assert.h"
#include "Cyclopes/Core/Window.h"
#include "Cyclopes/Core/Log.h"

namespace cyc {

	namespace
	{
		const char* vertexShader2D =
			"#version 330 core\n"
			"layout (location = 0) in vec2 vertexPosition;\n"
			"layout (location = 1) in vec4 vertexColor;\n"
			"layout (location = 2) in vec2 vertexTexture;\n"
			"layout (location = 3) in float vertexTextureId;\n"
			"uniform mat4 vertexOrthoProj;\n"
			"out vec4 fragmentColor;\n"
			"out vec2 fragmentTexture;\n"
			"out float fragmentTextureId;\n"
			"void main()\n"
			"{\n"
			"   fragmentColor = vertexColor;\n"
			"   fragmentTexture = vertexTexture;\n"
			"   fragmentTextureId = vertexTextureId;\n"
			"   gl_Position = vertexOrthoProj * vec4(vertexPosition, 1, 1);\n"
			"}";

		const char* fragmentShader2D =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec4 fragmentColor;\n"
			"in vec2 fragmentTexture;\n"
			"in float fragmentTextureId;\n"
			"uniform sampler2D Samplers[16];\n"
			"void main()\n"
			"{\n"
			"   int index = int(fragmentTextureId);\n"
			"   FragColor = texture2D(Samplers[index], fragmentTexture) * fragmentColor;\n"
			"}";
	}

	CycUnique<cyc::VertexBuffer> Renderer2D::m_VertexBuffer;
	CycUnique<cyc::IndexBuffer> Renderer2D::m_IndexBuffer;
	CycUnique<cyc::Shader> Renderer2D::m_Shader;
	cyc::VertexArray Renderer2D::m_VertexArray;

	CycVector<Renderer2D::Vertex> Renderer2D::m_DynamicVertices;
	CycVector<std::uint32_t> Renderer2D::m_DynamicIndices;

	int Renderer2D::m_CurrentBatch = 0;
	int Renderer2D::m_IndicesDrawn = 0;
	int Renderer2D::m_TotalBatches = 0;

	Camera2D Renderer2D::m_Camera;

	int Renderer2D::Stats::MaxIndicesPerBatch()
	{
		return m_IndicesPerBatch;
	}

	int Renderer2D::Stats::NTotalBatches()
	{
		return m_TotalBatches;
	}

	int Renderer2D::Stats::NIndicesDrawn()
	{
		return m_IndicesDrawn;
	}

	int Renderer2D::Stats::NVerticesDrawn()
	{
		return m_DynamicVertices.size();
	}

	int Renderer2D::Stats::NQuadsDrawn()
	{
		return m_IndicesDrawn / 6;
	}

	int Renderer2D::Stats::NTrianglesDrawn()
	{
		return NQuadsDrawn() * 2;
	}

	void Renderer2D::FillRect(const Vec2& translation, const Vec2& size, const char* texture, const Vec4& color, float rotation)
	{
		AddQuad(translation, size, color, TextureManager::GetID2D(texture), rotation);
	}

	void Renderer2D::FillRect(const Vec2& translation, const Vec2& size, const char* texture, float rotation)
	{
		FillRect(translation, size, texture, { 1.0f, 1.0f, 1.0f, 1.0f }, rotation);
	}

	void Renderer2D::FillRect(const Vec2& translation, const Vec2& size, const Vec4& color)
	{
		FillRect(translation, size, color, 0.0f);
	}

	void Renderer2D::FillRect(const Vec2& translation, const Vec2& size, const Vec4& color, float rotation)
	{
		FillRect(translation, size, "WhiteTexture1x1", color, rotation);
	}

	void Renderer2D::FillRect(const Vec2& translation, const Vec2& size, const char* texture)
	{
		FillRect(translation, size, texture, 0.0f);
	}

	void Renderer2D::FillRect(Rect2DDesc desc)
	{
		AddQuad({ desc.x, desc.y },
			{ desc.width, desc.height },
			{ desc.r, desc.g, desc.b, desc.a }, 
			desc.texture, 
			desc.rotation);
	}

	void Renderer2D::AddQuad(const Vec2& translation, const Vec2& size, const Vec4& color, int texture, float rotation)
	{
		AddIndices();
		Quad quad = MakeQuad(translation, size, color, texture, rotation);
		m_DynamicVertices.push_back(quad.vertex[0]);
		m_DynamicVertices.push_back(quad.vertex[1]);
		m_DynamicVertices.push_back(quad.vertex[2]);
		m_DynamicVertices.push_back(quad.vertex[3]);
	}

	Renderer2D::Quad Renderer2D::MakeQuad(const Vec2& translation, const Vec2& size, const Vec4& color, int texture, float rotation)
	{
		Quad quad;

		glm::vec2 halfSize = size * 0.5f;

		glm::vec3 positions[4] = {
			{ -halfSize.x, -halfSize.y, 0.0f },
			{ halfSize.x, -halfSize.y, 0.0f },
			{ -halfSize.x, halfSize.y, 0.0f },
			{ halfSize.x, halfSize.y, 0.0f }
		};

		VertexTexMap tex = GetVertexTexMap();

		for (int i = 0; i < 4; i++)
		{
			glm::mat4 trans = glm::translate(glm::mat4(1), { translation.x + halfSize.x, translation.y + halfSize.x, 0.0f });
			quad.vertex[i].position = glm::rotate(trans, rotation, { 0.0f, 0.0f, 1.0f }) *
				Vec4{ positions[i].x, positions[i].y, 1.0f, 1.0f };
			quad.vertex[i].color = color;
			quad.vertex[i].texture = tex.position[i];
			quad.vertex[i].textureID = texture;
		}

		return quad;
	}

	constexpr Renderer2D::VertexTexMap Renderer2D::GetVertexTexMap()
	{
		return
		{
			 Vec2{ 0.0f,  0.0f },
			 Vec2{ 1.0f,  0.0f },
			 Vec2{ 0.0f,  1.0f },
			 Vec2{ 1.0f,  1.0f }
		};
	}

	void Renderer2D::AddIndices()
	{
		unsigned int indices[6] =
		{
			0, 1, 2, //triangle 1
			2, 1, 3  //triangle 2        
		};
		for (int i = 0; i < 6; i++)
			m_DynamicIndices.push_back(indices[i] + (int)m_DynamicVertices.size());
	}

	void Renderer2D::_Init()
	{
		m_Shader = Shader::Create(cyc::Shader::Type::OPENGL);
		Cycsh source = Cycsh{ vertexShader2D, fragmentShader2D };
		m_Shader->Init(source);
		m_Shader->Bind();

		m_VertexArray.Init();
		m_VertexArray.Bind();

		m_VertexBuffer = VertexBuffer::Create(cyc::VertexBuffer::Type::OPENGL);
		m_VertexBuffer->Init();
		m_VertexBuffer->Bind();

		m_IndexBuffer = IndexBuffer::Create(cyc::IndexBuffer::Type::OPENGL);
		m_IndexBuffer->Init();
		m_IndexBuffer->Bind();

		m_VertexArray.SetupLayout<DataType>(2); //position
		m_VertexArray.SetupLayout<DataType>(4); //color
		m_VertexArray.SetupLayout<DataType>(2); //texture
		m_VertexArray.SetupLayout<DataType>(1); //texture id
		m_VertexArray.AddLayout();

		m_VertexArray.UnBind();
		m_IndexBuffer->UnBind();
		m_VertexBuffer->UnBind();
	}

	void Renderer2D::DrawNextBatch()
	{
		int nIndices = m_IndicesPerBatch;
		if (m_CurrentBatch == m_TotalBatches - 1)
			nIndices = m_DynamicIndices.size() - m_IndicesDrawn; 
		
		Graphics::Render::IndexedVertices(nIndices, m_IndicesDrawn);
		m_IndicesDrawn += nIndices;
	}

	void Renderer2D::_Flush()
	{
		BufferData();
		m_VertexArray.Bind();
		m_TotalBatches = (m_DynamicVertices.size() * sizeof(DataType) * m_VertexStride) / m_MaxBatchSize + 1;
		for (m_CurrentBatch = 0; m_CurrentBatch < m_TotalBatches; m_CurrentBatch++)
			DrawNextBatch();
	}

	void Renderer2D::BufferData()
	{
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind(); 
		if (m_VertexBuffer->GetSize() >= m_DynamicVertices.size() * sizeof(DataType) * m_VertexStride)
		{
			m_VertexBuffer->BufferSubData(m_DynamicVertices.data(), m_DynamicVertices.size() * sizeof(DataType) * m_VertexStride, 0);
		}
		else
		{
			m_VertexBuffer->BufferData(m_DynamicVertices.data(), m_DynamicVertices.size() * sizeof(DataType) * m_VertexStride, false);
		}

		if (m_IndexBuffer->GetSize() >= m_DynamicIndices.size() * sizeof(std::uint32_t))
		{
			m_IndexBuffer->BufferSubData(m_DynamicIndices.data(), m_DynamicIndices.size() * sizeof(std::uint32_t), 0);
		}
		else
		{
			m_IndexBuffer->BufferData(m_DynamicIndices.data(), m_DynamicIndices.size() * sizeof(std::uint32_t), false);
		}
	}

	void Renderer2D::SetCamera(const Camera2D& camera)
	{
		m_Camera = camera;
	}

	void Renderer2D::_StartBatches()
	{
		m_Shader->Bind();
		m_Shader->SetMat4f("vertexOrthoProj", m_Camera.GetOrthoProjectionMat());
		std::vector<int> samplers;
		for (int i = 0; i < TextureManager::Count2D(); i++)
			samplers.push_back(i);
		m_Shader->SetIntBuffer("Samplers", samplers.data(), samplers.size());

		m_DynamicVertices.clear();
		m_DynamicIndices.clear();
		m_CurrentBatch = 0;
		m_IndicesDrawn = 0;
		m_TotalBatches = 0;
	}
}