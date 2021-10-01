
#pragma once
#include <stdint.h>

typedef struct renderer_t renderer_t;

renderer_t* renderer_init();
void renderer_terminate(renderer_t* renderer);
void renderer_update(renderer_t* renderer);

//Graphics API specifics
void renderer_init_surface(renderer_t* renderer, void* surface);
void* renderer_get_vulkan_instance(renderer_t* renderer);
void* renderer_get_vulkan_device(renderer_t* renderer);
void* renderer_get_vulkan_surface(renderer_t* renderer);
void* renderer_get_vulkan_swapchain(renderer_t* renderer);