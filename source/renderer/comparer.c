#include <renderer/comparer.h>

#include <string.h> // strlen and strcmp

RENDERER_API bool string_equal_to(void* v1, void* v2)
{
	return strcmp(*(const char* const*)v1, *(const char* const*)v2) == 0;
}

RENDERER_API bool ptr_equal_to(void* v1, void* v2)
{
	return (*(char* const*)v1) == (*(char* const*)v2);
}

RENDERER_API bool s8_equal_to(void* v1, void* v2)
{
	return (*(const s8*)v1) == (*(const s8*)v2);
}

RENDERER_API bool s16_equal_to(void* v1, void* v2)
{
	return (*(const s16*)v1) == (*(const s16*)v2);
}

RENDERER_API bool s32_equal_to(void* v1, void* v2)
{
	return (*(const s32*)v1) == (*(const s32*)v2);
}

RENDERER_API bool s64_equal_to(void* v1, void* v2)
{
	return (*(const s64*)v1) == (*(const s64*)v2);
}

RENDERER_API bool u8_equal_to(void* v1, void* v2)
{
	return (*(const u8*)v1) == (*(const u8*)v2);
}

RENDERER_API bool u16_equal_to(void* v1, void* v2)
{
	return (*(const u16*)v1) == (*(const u16*)v2);
}

RENDERER_API bool u32_equal_to(void* v1, void* v2)
{
	return (*(const u32*)v1) == (*(const u32*)v2);
}

RENDERER_API bool u64_equal_to(void* v1, void* v2)
{
	return (*(const u64*)v1) == (*(const u64*)v2);
}

RENDERER_API bool float_equal_to(void* v1, void* v2)
{
	return (*(const f32*)v1) == (*(const f32*)v2);
}

RENDERER_API bool double_equal_to(void* v1, void* v2)
{
	return (*(const f64*)v1) == (*(const f64*)v2);
}


RENDERER_API bool string_greater_than(void* v1, void* v2)
{
	return strlen(*((const char* const*)v1)) > strlen(*((const char* const*)v2));
}

RENDERER_API bool ptr_greater_than(void* v1, void* v2)
{
	return *(char* const*)v1 > *(char* const*)v2;
}

RENDERER_API bool s8_greater_than(void* v1, void* v2)
{
	return *(const s8*)v1 > *(const s8*)v2;
}

RENDERER_API bool s16_greater_than(void* v1, void* v2)
{
	return *(const s16*)v1 > *(const s16*)v2;
}

RENDERER_API bool s32_greater_than(void* v1, void* v2)
{
	return *(const s32*)v1 > *(const s32*)v2;
}

RENDERER_API bool s64_greater_than(void* v1, void* v2)
{
	return *(const s64*)v1 > *(const s64*)v2;
}

RENDERER_API bool u8_greater_than(void* v1, void* v2)
{
	return *(const u8*)v1 > *(const u8*)v2;
}

RENDERER_API bool u16_greater_than(void* v1, void* v2)
{
	return *(const u16*)v1 > *(const u16*)v2;
}

RENDERER_API bool u32_greater_than(void* v1, void* v2)
{
	return *(const u32*)v1 > *(const u32*)v2;
}

RENDERER_API bool u64_greater_than(void* v1, void* v2)
{
	return *(const u64*)v1 > *(const u64*)v2;
}

RENDERER_API bool float_greater_than(void* v1, void* v2)
{
	return *(const f32*)v1 > *(const f32*)v2;
}

RENDERER_API bool double_greater_than(void* v1, void* v2)
{
	return *(const f64*)v1 > *(const f64*)v2;
}


RENDERER_API bool string_less_than(void* v1, void* v2)
{
	return strlen(*((const char* const*)v1)) < strlen(*((const char* const*)v2));
}

RENDERER_API bool ptr_less_than(void* v1, void* v2)
{
	return *(char* const*)v1 < *(char* const*)v2;
}

RENDERER_API bool s8_less_than(void* v1, void* v2)
{
	return *(const s8*)v1 < *(const s8*)v2;
}

RENDERER_API bool s16_less_than(void* v1, void* v2)
{
	return *(const s16*)v1 < *(const s16*)v2;
}

RENDERER_API bool s32_less_than(void* v1, void* v2)
{
	return *(const s32*)v1 < *(const s32*)v2;
}

RENDERER_API bool s64_less_than(void* v1, void* v2)
{
	return *(const s64*)v1 < *(const s64*)v2;
}

RENDERER_API bool u8_less_than(void* v1, void* v2)
{
	return *(const u8*)v1 < *(const u8*)v2;
}

RENDERER_API bool u16_less_than(void* v1, void* v2)
{
	return *(const u16*)v1 < *(const u16*)v2;
}

RENDERER_API bool u32_less_than(void* v1, void* v2)
{
	return *(const u32*)v1 < *(const u32*)v2;
}

RENDERER_API bool u64_less_than(void* v1, void* v2)
{
	return *(const u64*)v1 < *(const u64*)v2;
}

RENDERER_API bool float_less_than(void* v1, void* v2)
{
	return *(const f32*)v1 < *(const f32*)v2;
}

RENDERER_API bool double_less_than(void* v1, void* v2)
{
	return *(const f64*)v1 < *(const f64*)v2;
}
