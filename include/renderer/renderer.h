
#pragma once

#include <renderer/defines.h>

typedef struct renderer_t renderer_t;

renderer_t* renderer_init(u32 width, u32 height, const char* title);
void renderer_terminate(renderer_t* renderer);
void renderer_update(renderer_t* renderer);
bool renderer_is_running(renderer_t* renderer);

