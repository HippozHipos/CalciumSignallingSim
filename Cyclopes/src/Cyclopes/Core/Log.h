#pragma once

#include "Base.h"
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

namespace cyc {

	class Logger
	{
	public:
		static void Init();
		static CycShared<spdlog::logger>& Get();
		
	private:
		static CycShared<spdlog::logger> m_Logger;
	};
}

#if defined(CYC_DEBUG) || defined(CYC_RELEASE) 

#define CYC_ERROR(...)    ::cyc::Logger::Get()->error(__VA_ARGS__)
#define CYC_WARN(...)     ::cyc::Logger::Get()->warn(__VA_ARGS__)
#define CYC_CRITICAL(...) ::cyc::Logger::Get()->critical(__VA_ARGS__)
#define CYC_INFO(...)     ::cyc::Logger::Get()->info(__VA_ARGS__)
#define CYC_TRACE(...)    ::cyc::Logger::Get()->trace(__VA_ARGS__)
#else
#define CYC_ERROR(...)    
#define CYC_WARN(...)     
#define CYC_CRITICAL(...) 
#define CYC_INFO(...)     
#define CYC_TRACE(...)    

#endif
