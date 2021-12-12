
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


#define bmp_load(...) define_alias_function_macro(bmp_load, __VA_ARGS__)
#define bmp_destroy(...) define_alias_function_macro(bmp_destroy, __VA_ARGS__)

function_signature(bmp_t, bmp_load, const char* file_path);
function_signature(void, bmp_destroy, bmp_t bmp);
