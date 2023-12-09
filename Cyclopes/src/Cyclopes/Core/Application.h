#pragma once

#include "Base.h"
#include "Log.h"

#include "Cyclopes/imgui/ImGuiContext.h"
#include "Cyclopes/Core/Window.h"

#include "Time.h"


namespace cyc {

	bool RunMessagePump();

	class Application
	{
	public:
		Application(int width, int height);
		virtual ~Application() = default;

	public:
		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnDestroy();
		virtual void OnImGuiRender();


	public:
		void _OnCoreInit();
		void _OnCoreUpdate();
		void _OnCoreDestroy();

		void _OnClientInit();
		void _OnClientUpdate();
		void _OnClientDestroy();

	public:
		void _Run();

	protected:
		CycUnique<Window> window; 

	private:
		ImGuiContext m_ImGuiContext;
		bool m_Running = false;
	};

	Application* CreateApplication();
}