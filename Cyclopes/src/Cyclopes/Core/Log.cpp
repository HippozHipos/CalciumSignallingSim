#include <Cycpch.h>
#include "Log.h"

#include "Cycdefs.h"

namespace cyc {

	CycShared<spdlog::logger> Logger::m_Logger;

	void Logger::Init()
	{
#if (defined(CYC_DEBUG) || defined(CYC_RELEASE)) && defined(CYC_PLATFORM_WINDOWS)
		//https ://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;

		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
		FILE* hf_in = _fdopen(hCrt, "r");
		setvbuf(hf_in, NULL, _IONBF, 128);
		*stdin = *hf_in;
#endif

		spdlog::set_pattern("%^[%T] %n: %v%$");

		m_Logger = spdlog::stdout_color_mt("CYC ENGINE");
		m_Logger->set_level(spdlog::level::trace);
	}

	CycShared<spdlog::logger>& Logger::Get()
	{
		return m_Logger;
	}
}