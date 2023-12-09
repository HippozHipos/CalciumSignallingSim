#include "Cycpch.h"
#include "Cycdefs.h"

#include "Application.h"

#include <imgui.h>
#include <Cyclopes/imgui/backend/imgui_impl_opengl3.h>

#include "Cyclopes/Core/Assert.h"

#include "Cyclopes/Platform/RenderAPI/OpenGl/OpenGLError.h"
#include "Cyclopes/Platform/OS/Windows/WindowsWindow.h"
#include "Cyclopes/Renderer/Graphics.h"
#include "Cyclopes/Renderer/Renderer2D.h"
#include "Cyclopes/Core/ResourceManager.h"

namespace cyc {

    bool RunMessagePump()
    {
#ifdef CYC_PLATFORM_WINDOWS
        MSG msg;
        while (PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
#endif
    }


    Application::Application(int width, int height)
    {
        m_Running = true;
        cyc::Logger::Init();
        window = Window::Create({ 100, 100, width, height });
        Display::_x = 100; Display::_y = 100;
        Display::_width = width; Display::_height = height;
    }

    void Application::_OnCoreInit()
    {
        Graphics::Init(window.get());
        m_ImGuiContext.OnInit(window.get());
        TextureManager::_Init();
        Renderer2D::_Init();
    }

    void Application::_OnCoreUpdate()
    {
        window->_UpdateProperty();
        WindowProperties& p = window->GetProperty();
        while (window->HasEvent())
        {
            cyc::WindowEvent we = window->ReadEvent();
            switch ((int)we.GetType())
            {
                case((int)cyc::EventType::W_CLOSE): window->_Destroy(); break;
                case((int)cyc::EventType::W_RESIZED): Graphics::SetViewport((float)p.x, (float)p.y, (float)p.width, (float)p.height); break;
            }
        }
        Display::_x = p.rawx; Display::_y = p.rawy;
        Display::_width = p.width; Display::_height = p.height;
        Renderer2D::_StartBatches();
    }

    void Application::_OnCoreDestroy()
    {
        Graphics::Destroy();
        m_ImGuiContext.OnDestroy();
    }

    void Application::OnInit()
    {
        
    }

    void Application::OnUpdate()
    {

    }

    void Application::OnDestroy()
    {

    }

    void Application::OnImGuiRender()
    {

    }

    void Application::_OnClientInit()
    {
        OnInit();
        LayerStack& ls = window->GetLayerStack();
        ls._OnInit();
    }

    void Application::_OnClientUpdate()
    {
        LayerStack& ls = window->GetLayerStack();

        OnUpdate();
        ls._OnUpdate();
        Renderer2D::_Flush();

        m_ImGuiContext.OnBeginRender();
        OnImGuiRender();
        ls._OnImGuiRender();
        if (window->GetWindowCount())
        {
            m_ImGuiContext.OnEndRender(window.get());
            Graphics::SwapBuffers();
        }
    }

    void Application::_OnClientDestroy()
    {
        OnDestroy();
        LayerStack& ls = window->GetLayerStack();
        ls._OnDestroy();
    }

    void Application::_Run()
    {
        _OnCoreInit();
        _OnClientInit();

        while (m_Running)
        {
            RunMessagePump();
            Time::_Reset();
            _OnCoreUpdate();
            _OnClientUpdate();
            Time::_UpdateElapsedTime();
            m_Running = window->GetWindowCount(); 
        }

        _OnClientDestroy();
        _OnCoreDestroy();
    }
}

