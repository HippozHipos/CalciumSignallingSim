#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)	
	#ifndef CYC_PLATFORM_WINDOWS
		#define CYC_PLATFORM_WINDOWS
	#endif
//#elif __APPLE__
//#include <TargetConditionals.h>
//#if TARGET_IPHONE_SIMULATOR
//	#define CYC_PLATFORM_IPHONE_SIM
//#elif TARGET_OS_MACCATALYST
//	#define CYC_PLATFORM_MACCATALYST
//#elif TARGET_OS_IPHONE
//	#define CYC_PLATFORM_IPHONE
//#elif TARGET_OS_MAC
//	#define CYC_PLATFORM_MAC
//#else
//#   error "Unknown Apple platform"
//#endif
//#elif __ANDROID__
//	#define CYC_PLATFORM_ANDROID
//#elif __linux__
//	#define CYC_PLATFORM_LINUX
//#elif __unix__ // all unices not caught above
//	#define CYC_PLATFORM_UNIX
//#elif defined(_POSIX_VERSION)
//	#define CYC_PLATFORM_POSIX
#else
#   error "Unknown compiler"
#endif

#ifndef UNICODE
	#define UNICODE
#endif

#if defined(DEBUG) || defined(_DEBUG)
	#define CYC_DEBUG 1
#else
	#define CYC_DEBUG
#endif

#if defined(RELEASE) || defined(_RELEASE)
	#define CYC_RELEASE 1
#else
	#define CYC_RELEASE
#endif
