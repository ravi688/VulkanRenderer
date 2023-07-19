#pragma once

#include <renderer/defines.h>

typedef u64 hash_t;
typedef hash_t (*hash_function_t)(void* key);

BEGIN_CPP_COMPATIBLE

RENDERER_API hash_t string_hash(void* v);
RENDERER_API hash_t ptr_hash(void* v);
RENDERER_API hash_t s8_hash(void* v);
#define char_hash(void_ptr) s8_hash(void_ptr)
RENDERER_API hash_t s16_hash(void* v);
RENDERER_API hash_t s32_hash(void* v);
RENDERER_API hash_t s64_hash(void* v);
RENDERER_API hash_t u8_hash(void* v);
RENDERER_API hash_t u16_hash(void* v);
RENDERER_API hash_t u32_hash(void* v);
RENDERER_API hash_t u64_hash(void* v);
RENDERER_API hash_t float_hash(void* v);
RENDERER_API hash_t double_hash(void* v);

END_CPP_COMPATIBLE
