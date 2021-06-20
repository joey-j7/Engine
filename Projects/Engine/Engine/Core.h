#pragma once

#ifdef CB_PLATFORM_WINDOWS
	#ifdef CB_BUILD_DLL
		#define Engine_API __declspec(dllexport)
	#else
		#define Engine_API __declspec(dllimport)
	#endif
#elif CB_PLATFORM_ANDROID
	#ifdef CB_BUILD_DLL
		#define Engine_API __attribute__ ((visibility("default")))
	#else
		#define Engine_API 
	#endif
#else
	#error The engine only supports Windows and Android!
#endif

#ifdef CB_DEBUG
	#define CB_ENABLE_ASSERTS
#endif

#ifdef CB_ENABLE_ASSERTS
	#if CB_PLATFORM_ANDROID
		#define CB_BREAK() __builtin_trap()
	#else
		#define CB_BREAK() __debugbreak()
	#endif

	#define CB_ASSERT(x, ...) { if(!(x)) { CB_ERROR("Assertion Failed: {0}", __VA_ARGS__); CB_BREAK(); } }
	#define CB_CORE_ASSERT(x, ...) { if(!(x)) { CB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); CB_BREAK(); } }
#else
	#define CB_ASSERT(x, ...)
	#define CB_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define CB_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
