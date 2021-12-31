
#pragma once

#include <renderer/stage_shader.h>
#include <renderer/defines.h>
#include <buffer.h>

typedef struct shader_t
{
	stage_shader_t** stage_shaders;
	u8 stage_count;
} shader_t;

#define shader_load(renderer, file_path) shader_load_and_create(renderer, file_path)

shader_t* shader_create(renderer_t* renderer, BUFFER* buffer);
shader_t* shader_load_and_create(renderer_t* renderer, const char* file_path);
void shader_destroy(shader_t* shader, renderer_t* renderer);
void shader_release_resources(shader_t* shader);
