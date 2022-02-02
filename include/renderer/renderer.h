
#pragma once

#include <renderer/defines.h>

typedef struct render_window_t render_window_t;
typedef struct renderer_t renderer_t;

typedef enum renderer_gpu_type_t
{
	RENDERER_GPU_TYPE_AUTO = 0,
	RENDERER_GPU_TYPE_INTEGRATED,
	RENDERER_GPU_TYPE_DISCRETE
} renderer_gpu_type_t;

renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen);
void renderer_terminate(renderer_t* renderer);
void renderer_update(renderer_t* renderer);
bool renderer_is_running(renderer_t* renderer);

void renderer_begin_frame(renderer_t* renderer, float r, float g, float b, float a);
void renderer_end_frame(renderer_t* renderer);


render_window_t* renderer_get_window(renderer_t* renderer);
