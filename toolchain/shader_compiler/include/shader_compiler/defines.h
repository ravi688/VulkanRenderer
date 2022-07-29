
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t u16; 
typedef int16_t s16;

typedef uint32_t u32;
typedef int32_t s32; 

typedef uint64_t u64; 
typedef int64_t s64; 

typedef uint8_t u8; 
typedef int8_t s8; 

#ifdef SC_STATIC_LIBRARY
#	define SC_API
#elif SC_DYNAMIC_LIBRARY
#	define SC_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SC_API __declspec(dllexport)
#else
#	define SC_API
#endif
