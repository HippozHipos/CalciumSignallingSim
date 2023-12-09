#include "Cycpch.h"
#include "Window.h"

#include "Cyclopes/Core/Log.h"

#ifdef CYC_PLATFORM_WINDOWS
	#include "Cyclopes/Platform/OS/Windows/WindowsWindow.h"
#endif

namespace cyc {

	CycUnique<Window> Window::Create(const WindowProperties& props)
	{
#ifdef CYC_PLATFORM_WINDOWS
		return CycMakeUnique<WindowsWindow>(props);
#endif
	}

	int Display::_x = 0;
	int Display::_y = 0;
	int Display::_width = 0;
	int Display::_height = 0;

	int Display::GetX()
	{
		return _x;
	} 

	int Display::GetY()
	{
		return _y;
	}

	int Display::GetWidth()
	{
		return _width;
	}

	int Display::GetHeight()
	{
		return _height;
	}

}
