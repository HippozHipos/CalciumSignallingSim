#pragma once

#include "Cyclopes/Core/Base.h"

namespace cyc {
	
	class Window;
	class GraphicsContext
	{
	public:
		enum class Type
		{
			OPENGL,
			DX12
		};

	public:
		GraphicsContext() = default;
		virtual ~GraphicsContext() = default;

	public:
		static CycUnique<GraphicsContext> Create(Type type);

	public:
		virtual void Init(Window* window) = 0;
		virtual void Destroy() = 0;
		virtual void SwapBuffers() = 0;
		virtual void MakeCurrent() = 0;
		virtual void* GetContext() = 0;
		virtual void SetSwapInterval(int interval) = 0;
		virtual void SetViewport(float x, float y, float width, float height) = 0; 
	};

}
