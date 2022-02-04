
#pragma once


#include <stdint.h>
#include <stdbool.h>

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int64_t s64;
typedef int16_t s16;
typedef int8_t s8;

typedef float f32;
typedef double f64;
typedef u32 uint;



#define INTERNAL

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define S8_MAX INT8_MAX
#define S8_MIN INT8_MIN
#define S16_MAX INT16_MAX
#define S16_MIN INT16_MIN
#define S32_MAX INT32_MAX
#define S32_MIN INT32_MIN
#define S64_MAX INT64_MAX
#define S64_MIN INT64_MIN


#define CHAR_MAX S8_MAX
#define CHAR_MIN S8_MIN
#define SHORT_MAX S16_MAX
#define SHORT_MIN S16_MIN
#define INT_MAX S32_MAX
#define INT_MIN S32_MIN
#define LONG_MAX S64_MAX
#define LONG_MIN S64_MIN

#define UCHAR_MAX U8_MAX
#define UCHAR_MIN U8_MIN
#define USHORT_MAX U16_MAX
#define USHORT_MIN U16_MIN
#define UINT_MAX U32_MAX
#define UINT_MIN U32_MIN
#define ULONG_MAX U64_MAX
#define ULONG_MIN U64_MIN


#ifdef GLOBAL_DEBUG
#	define DEBUG_BLOCK(x) x
#else
#	define DEBUG_BLOCK(x)
#endif /*GLOBAL_DEBUG*/



#define DEPRECATED __attribute__((deprecated))
