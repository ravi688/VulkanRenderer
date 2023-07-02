#pragma once

#include <renderer/defines.h>

BEGIN_CPP_COMPATIBLE

typedef bool (*comparer_t)(void* lhs, void* rhs);

RENDERER_API bool string_equal_to(void* lhs, void* rhs);
RENDERER_API bool ptr_equal_to(void* lhs, void* rhs);
RENDERER_API bool s8_equal_to(void* lhs, void* rhs);
RENDERER_API bool s16_equal_to(void* lhs, void* rhs);
RENDERER_API bool s32_equal_to(void* lhs, void* rhs);
RENDERER_API bool s64_equal_to(void* lhs, void* rhs);
RENDERER_API bool u8_equal_to(void* lhs, void* rhs);
RENDERER_API bool u16_equal_to(void* lhs, void* rhs);
RENDERER_API bool u32_equal_to(void* lhs, void* rhs);
RENDERER_API bool u64_equal_to(void* lhs, void* rhs);
RENDERER_API bool float_equal_to(void* lhs, void* rhs);
RENDERER_API bool double_equal_to(void* lhs, void* rhs);

RENDERER_API bool string_greater_than(void* lhs, void* rhs);
RENDERER_API bool ptr_greater_than(void* lhs, void* rhs);
RENDERER_API bool s8_greater_than(void* lhs, void* rhs);
RENDERER_API bool s16_greater_than(void* lhs, void* rhs);
RENDERER_API bool s32_greater_than(void* lhs, void* rhs);
RENDERER_API bool s64_greater_than(void* lhs, void* rhs);
RENDERER_API bool u8_greater_than(void* lhs, void* rhs);
RENDERER_API bool u16_greater_than(void* lhs, void* rhs);
RENDERER_API bool u32_greater_than(void* lhs, void* rhs);
RENDERER_API bool u64_greater_than(void* lhs, void* rhs);
RENDERER_API bool float_greater_than(void* lhs, void* rhs);
RENDERER_API bool double_greater_than(void* lhs, void* rhs);

RENDERER_API bool string_less_than(void* lhs, void* rhs);
RENDERER_API bool ptr_less_than(void* lhs, void* rhs);
RENDERER_API bool s8_less_than(void* lhs, void* rhs);
RENDERER_API bool s16_less_than(void* lhs, void* rhs);
RENDERER_API bool s32_less_than(void* lhs, void* rhs);
RENDERER_API bool s64_less_than(void* lhs, void* rhs);
RENDERER_API bool u8_less_than(void* lhs, void* rhs);
RENDERER_API bool u16_less_than(void* lhs, void* rhs);
RENDERER_API bool u32_less_than(void* lhs, void* rhs);
RENDERER_API bool u64_less_than(void* lhs, void* rhs);
RENDERER_API bool float_less_than(void* lhs, void* rhs);
RENDERER_API bool double_less_than(void* lhs, void* rhs);

END_CPP_COMPATIBLE
