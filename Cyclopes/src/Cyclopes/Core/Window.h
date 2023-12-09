#pragma once

#include "Base.h"

#include "Cyclopes/Event/Mouse.h"
#include "Cyclopes/Event/Keyboard.h"
#include "Cyclopes/Event/WindowEvent.h"

#include "Cyclopes/Core/LayerStack.h"

#include <memory>
#include <bitset>

namespace cyc {

	struct WindowProperties
	{
		WindowProperties(int rawx, int rawy, int width, int height, CycString title) :
			rawx(rawx), rawy(rawy), width(width), height(height), title(title)
		{
		}

		WindowProperties(int rawx, int rawy, int width, int height) :
			rawx(rawx), rawy(rawy), width(width), height(height)
		{
		}

		int rawx = 0;
		int rawy = 0;
		static constexpr int x = 0;
		static constexpr int y = 0;
		int width = 0;
		int height = 0;
		CycString title = "";
	};

	class Window
	{
	public:
		Window() = default;
		virtual ~Window() = default;

	public:
		virtual bool IsFocused() const = 0;
		virtual void Focus() = 0;
		virtual const Mouse& GetMouse() const = 0;
		virtual const Keyboard& GetKeyboard() const = 0;
		virtual void _Destroy() = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual bool HasEvent() const = 0;
		virtual WindowEvent ReadEvent() const = 0;
		virtual LayerStack& GetLayerStack() = 0;
		virtual void _UpdateProperty() = 0;
		virtual WindowProperties GetProperty() const = 0;
		virtual int GetWindowCount() = 0;
		virtual void HideCursor(bool hide) = 0;
		virtual void LockCursor(bool lock) = 0;
		virtual bool CursorIsLocked() const = 0;
		virtual bool CursorIsHidden() const = 0;

	public:
		virtual void* GetNativeWindow() = 0; 

	public:
		static CycUnique<Window> Create(const WindowProperties& props = {100, 100, 600, 500, "Cyclopes Window"});

	protected:
		bool m_VSyncEnabled = false;
	};

	struct Display
	{
		static int GetX();
		static int GetY();
		static int GetWidth();
		static int GetHeight();

		static int _x;
		static int _y;
		static int _width;
		static int _height;
	};
}

