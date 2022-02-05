
#pragma once

#include <renderer/defines.h>
#include <calltrace/calltrace.h>

typedef struct bmp_t
{
	void* data;
	u32 width;
	u32 height;
	u8 channel_count;
} bmp_t;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define bmp_load(...) define_alias_function_macro(bmp_load, __VA_ARGS__)
#define bmp_destroy(...) define_alias_function_macro(bmp_destroy, __VA_ARGS__)

RENDERER_API function_signature(bmp_t, bmp_load, const char* file_path);
RENDERER_API function_signature(void, bmp_destroy, bmp_t bmp);


#ifdef __cplusplus
}
#endif // __cplusplus
