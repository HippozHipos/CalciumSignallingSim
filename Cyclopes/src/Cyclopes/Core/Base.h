#pragma once

#include "Cycdefs.h"

#include <memory>
#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>
#include <xstring>
#include <signal.h>

#define BIT(n) 1 << n

#ifdef CYC_PLATFORM_WINDOWS
	//FunctionNameW version of functions in win32 api are used thoughout
	#ifndef UNICODE
		#define UNICODE
	#endif
#else
	#error Cyclopes currently only supports windows
#endif

#ifndef SIGTRAP
	#define SIGTRAP 5
#endif
#ifndef CYC_DEBUGBREAK
	#define CYC_DEBUGBREAK raise(SIGTRAP)
#endif

namespace cyc {

	template<class Type> using CycShared = std::shared_ptr<Type>;
	template<class Type> using CycUnique = std::unique_ptr<Type>;

	template<class Type> using CycVector = std::vector<Type>;

	template<class Type> using CycQueue = std::queue<Type>;
	template<class Type> using CycDeque = std::deque<Type>;

	template<class First, class Second> using CycUnorderedMap = std::unordered_map<First, Second>;

	using CycString = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
	using CycWString = std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;

	template<class Type, class... Args> 
	CycShared<Type> CycMakeShared(Args&&... args)
	{
		return CycShared<Type>(new Type(std::forward<Args>(args)...));
	}

	template<class Type, class... Args>
	CycUnique<Type> CycMakeUnique(Args&&... args)
	{
		return CycUnique<Type>(new Type(std::forward<Args>(args)...));
	}
}