#include "Cycpch.h"
#include "Graphics.h"
#include "GraphicsContext.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	CycUnique<RendererAPI> Graphics::Render::m_RendererAPI = nullptr;

	void Graphics::Render::SetRendererAPI()
	{
#ifdef CYC_PLATFORM_WINDOWS
		m_RendererAPI = RendererAPI::Create(RendererAPI::Type::OPENGL); return;//change to dx when dx is implemented
#endif

		//fallback to opengl if no other rendering api fits the os
		m_RendererAPI = RendererAPI::Create(RendererAPI::Type::OPENGL);
	}

	void Graphics::Render::Clear(float r, float g, float b, float a)
	{
		CYC_ASSERT(m_RendererAPI != nullptr, "Renderer API not set");
		m_RendererAPI->Clear(r, g, b, a);
	}

	void Graphics::Render::IndexedVertices(int indexCount)
	{
		CYC_ASSERT(m_RendererAPI != nullptr, "Renderer API not set");
		m_RendererAPI->DrawIndexedVertices(indexCount);
	}

	void Graphics::Render::IndexedVertices(int indexCount, int offset)
	{
		CYC_ASSERT(m_RendererAPI != nullptr, "Renderer API not set");
		m_RendererAPI->DrawIndexedVertices(indexCount, offset);
	}

	CycUnique<GraphicsContext> Graphics::m_Context = nullptr;

	void Graphics::SetContext()
	{
#ifdef CYC_PLATFORM_WINDOWS
		m_Context = GraphicsContext::Create(GraphicsContext::Type::OPENGL);
		return;//will be changed to dx when dx is implemented
#endif

		//fallback to opengl if no other rendering api fits the os
		m_Context = GraphicsContext::Create(GraphicsContext::Type::OPENGL);
	}

	void Graphics::Init(Window* window)
	{
		SetContext();
		m_Context->Init(window);
		Render::SetRendererAPI();
	}

	void Graphics::Destroy()
	{
		m_Context->Destroy();
	}

	void Graphics::SwapBuffers()
	{
		m_Context->SwapBuffers();
	}

	void Graphics::MakeCurrent()
	{
		m_Context->MakeCurrent();
	}

	void* Graphics::GetContext()
	{
		return m_Context->GetContext();
	}

	void Graphics::SetSwapInterval(int interval)
	{
		m_Context->SetSwapInterval(interval);
	}

	void Graphics::SetViewport(float x, float y, float width, float height)
	{
		m_Context->SetViewport(x, y, width, height);
	}

}