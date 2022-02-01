
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

#define U8_MAX 0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFFUL
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL

#define S8_MAX 127
#define S8_MIN -128
#define S16_MAX 32767
#define S16_MIN -32768
#define S32_MAX 2147483647L
#define S32_MIN -2147483648L
#define S64_MAX 9223372036854775807LL
#define S64_MIN -9223372036854775808LL


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

