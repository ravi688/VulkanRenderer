	
#pragma once

#include <common/defines.h>

#define INTERNAL

#ifdef RENDERER_STATIC_LIBRARY
#	define RENDERER_API
#elif RENDERER_DYNAMIC_LIBRARY
#	define RENDERER_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define RENDERER_API __declspec(dllexport)
#else
#	define RENDERER_API
#endif

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_renderer_t vulkan_renderer_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_renderer_t opengl_renderer_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_renderer_t directx_renderer_t;
#elif defined(RENDERER_METAL_DRIVER)
	typedef struct metal_renderer_t metal_renderer_t;
#endif

typedef struct renderer_t renderer_t;
typedef struct memory_allocator_t memory_allocator_t;

#include <renderer/assert.h>
#define NOT_IMPLEMENTED_FUNCTION() LOG_FETAL_ERR("You're trying to call \"%s\" which isn't implemented yet!\n", __FUNCTION__);


/* platform identification */
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#	define RENDERER_PLATFORM_WINDOWS
#elif defined(__LINUX__) || defined(LINUX)
#	define RENDERER_PLATFORM_LINUX
#else
#	define RENDERER_PLATFORM_LINUX
#endif
// TODO: RENDERER_PLATFORM_MACOS, RENDERER_PLATFORM_ANDROID


/* compiler identification */
#if defined(__MINGW32__)
#	define RENDERER_COMPILER_MINGW32	 		// MING32
#elif defined(__MINGW64__)
#	define RENDERER_COMPILER_MINGW64 			// MINGW64
#elif defined(__clang__)
#	define RENDERER_COMPILER_CLANG 				// CLANG
#elif defined(__GNUC__)
#	define RENDERER_COMPILER_GCC				// GCC
#elif defined(_MSC_VER)
#	define RENDERER_COMPILER_MSVC 				// MSVC
#endif

#if defined(RENDERER_COMPILER_MINGW64) || defined(RENDERER_COMPILER_MINGW32)
#	define RENDERER_COMPILER_MINGW
#endif // MINGW

#ifdef GLOBAL_DEBUG
#	define INLINE_IF_RELEASE_MODE INLINE
#	define IF_DEBUG_MODE(x) x
#else
#	define INLINE_IF_RELEASE_MODE /* no inline */
#	define IF_DEBUG_MODE(x) 
#endif /* GLOBAL_DEBUG */
