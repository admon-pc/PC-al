#ifndef _AL_BASE_H_
#define _AL_BASE_H_

#ifdef _WIN32
#define AL_PLATFORM_WIN32
#else
#error This platform is not supported
#endif

#ifdef _MSC_VER
#define AL_FORCE_INLINE __forceinline
#define AL_CDECL _cdecl
#ifdef AL_EXPORT
#define AL_API _declspec(dllexport)
#else
#define AL_API _declspec(dllimport)
#endif
#else
#define AL_FORCE_INLINE inline
#define AL_API
#define AL_CDECL
#endif

#ifdef AL_PLATFORM_WIN32
#ifdef _WIN64
#define AL_PLATFORMBIT64
#endif
#else
#error This platform is not supported
#endif

#ifdef _DEBUG
#define AL_DEBUG
#endif

#ifdef AL_DEBUG
#define AL_DEBUGBREAK __debugbreak();
#else
#define AL_DEBUGBREAK
#endif

#define AL_FILE __FILE__
#define AL_LINE __LINE__
#define AL_FUNCTION __FUNCTION__

#define AL_QUOTE(s) #s

#ifdef AL_PLATFORM_WIN32
#ifdef AL_PLATFORMBIT64
#define AL_LL_PLATFORM "_x64"
#else
#define AL_LL_PLATFORM "_x86"
#endif
#define AL_LL_TOOLSET "_v142"
#define AL_LL_CONFIGURATION "_Debug"

#define AL_LINK_LIBRARY(x) \
 __pragma(comment(lib, #x AL_LL_PLATFORM AL_LL_TOOLSET AL_LL_CONFIGURATION ".lib"))

#endif

using float32_t = float;
using float64_t = double;
using real_t = float64_t;

#define AL_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((uint32_t)(uint8_t)(ch0)|((uint32_t)(uint8_t)(ch1)<<8)|\
	((uint32_t)(uint8_t)(ch2)<<16)|((uint32_t)(uint8_t)(ch3)<<24))

#ifndef BIT
#define BIT(x)0x1<<(x)
#endif

#endif
