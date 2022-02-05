
#pragma once

#include <renderer/defines.h>
#include <calltrace/calltrace.h>

typedef struct png_t
{
	void* data;
	u32 width;
	u32 height; 
} png_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define png_load(...) define_alias_function_macro(png_load, __VA_ARGS__)
RENDERER_API function_signature(png_t, png_load, const char* file_path);
void png_destroy(png_t png);

#ifdef __cplusplus
}
#endif // __cplusplus
