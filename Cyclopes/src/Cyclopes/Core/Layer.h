#pragma once

#include "Cyclopes/Event/Event.h"

namespace cyc {

	class Window;
	class Renderer2D;
	class Layer
	{
	public:
		Layer(const CycString& name);
		virtual ~Layer() = default;

	public:
		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnEvent(Event& e);
		virtual void OnDestroy();
		virtual void OnImGuiRender();

		const CycString& GetName() const;

		void SetWindow(Window* win);

	protected:
		Window* window = nullptr;

	private:
		CycString m_Name;
	};

}
