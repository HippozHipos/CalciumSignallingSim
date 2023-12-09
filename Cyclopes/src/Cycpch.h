#pragma once

#include <chrono>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include <type_traits>

#include <locale>

#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
	#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 
#endif
#include <codecvt>

#include <vector>
#include <string>
#include <deque>
#include <queue>
#include <stack>
#include <array>

#include <bitset>
#include <cmath>

#include <memory>
#include <functional>

#include <filesystem>

#include <thread>

#include "Cycdefs.h"
#ifdef CYC_PLATFORM_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
#endif
