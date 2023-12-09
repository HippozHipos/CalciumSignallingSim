#pragma once

#include "Cyclopes/Core/Base.h"
#include "Cyclopes/Renderer/RendererAPI.h"
#include <glm/vec2.hpp>

namespace cyc {

	class GraphicsContext;
	class Window;
	class Graphics
	{
	public:
		struct Render
		{
		public:
			static void SetRendererAPI();

		public:
			static void Clear(float r, float g, float b, float a);
			static void IndexedVertices(int indexCount);
			static void IndexedVertices(int indexCount, int offset);

		private:
			static CycUnique<RendererAPI> m_RendererAPI;
		};

	public:
		static void SetContext();

	public:
		static void Init(Window* window);
		static void Destroy();
		static void SwapBuffers();
		static void MakeCurrent();
		static void* GetContext();
		static void SetSwapInterval(int interval);
		static void SetViewport(float x, float y, float width, float height);

	private:
		static CycUnique<GraphicsContext> m_Context;
	};

}
