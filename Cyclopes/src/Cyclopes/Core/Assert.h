#pragma once

#include "Cycdefs.h"
#include "Log.h"

#ifndef CYC_STRINGIFY
	#define CYC_STRINGIFY(a) #a
#endif

#include <string.h>

#ifdef CYC_PLATFORM_WINDOWS
	#if defined(CYC_DEBUG) || defined(CYC_RELEASE)
		#define CYC_ASSERT_WIN32MSG(cond, win, error)					    \
			if (!(cond))													\
			{																\
				win->ErrorMessageBox(										\
					CYC_STRINGIFY(Cyclopes Core Assertion Failed) L"\n\n"	\
					error L"\n\n"											\
					CYC_STRINGIFY([FILE] ) __FILE__ L"\n\n"					\
					CYC_STRINGIFY([LINE] ) + std::to_wstring(__LINE__)		\
				);															\
				CYC_DEBUGBREAK;											    \
			}																\

	#else
		#define CYC_CORE_ASSERT_WIN32MSG(cond, win, error)	

	#endif

#endif

#if defined(CYC_DEBUG) || defined(CYC_RELEASE)

#define CYC_ASSERT(cond, ...)												\
			if (!(cond))													\
			{																\
				CYC_ERROR(__VA_ARGS__);								        \
				CYC_DEBUGBREAK;												\
			}																\


#define CYC_ASSERT_NOBREAK(cond, ...)										\
			if (!(cond))													\
				{															\
					CYC_ERROR(__VA_ARGS__);									\
				}															\

#else

#define CYC_CORE_ASSERT(cond, ...)
#define CYC_ASSERT(cond, ...)	
#define CYC_MEMORY_CHECK(cond, ...)
#define CYC_MEMORY_CHECK_NOBREAK(cond, ...)

#endif