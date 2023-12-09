#pragma once

#include "Cyclopes/Core/Window.h"

namespace cyc {

	class ImGuiContext
	{
	public:
		void OnInit(Window* window);
		void OnDestroy();

		void OnBeginRender();
		void OnEndRender(Window* window);

	private:
		void SetStyle();
		Window* m_RenderWindow = nullptr;
	};

}
