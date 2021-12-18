
#pragma once

#include <renderer/defines.h>

#include <ttf2mesh.h>

typedef struct font_t font_t;


typedef struct font_t
{
	ttf_t* handle;

} font_t;


font_t* font_create(void* bytes, u64 length);
font_t* font_load_and_create(const char* file_name);
void font_destroy(font_t* font);
void font_release_resources(font_t* font);
