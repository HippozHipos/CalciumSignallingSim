#pragma once

#include "Cycdefs.h"

#ifdef CYC_ENTRYPOINT_DEFINED
	#error "Cyclopes/Core/EntryPoint included more than once. This file can only be included once as the entry point"
#endif

#ifndef CYC_ENTRYPOINT_DEFINED
	#define CYC_ENTRYPOINT_DEFINED
#endif

cyc::Application* cyc::CreateApplication();

#ifdef CYC_PLATFORM_WINDOWS

int CALLBACK WinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow)
{
	cyc::Application* app = cyc::CreateApplication();
	app->_Run();
	delete app;
}

#endif