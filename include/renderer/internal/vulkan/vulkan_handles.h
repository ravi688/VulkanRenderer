
#pragma once

#include <bufferlib/buffer.h>

typedef buf_ucount_t vulkan_material_handle_t;
#define VULKAN_MATERIAL_HANDLE_INVALID (~0ULL)

typedef buf_ucount_t vulkan_render_pass_handle_t;
#define VULKAN_RENDER_PASS_HANDLE_INVALID (~0ULL)

typedef buf_ucount_t vulkan_shader_handle_t;
#define VULKAN_SHADER_HANDLE_INVALID (~0ULL)

typedef struct vulkan_render_object_t vulkan_render_object_t;
typedef vulkan_render_object_t* vulkan_render_object_handle_t;
#define VULKAN_RENDER_OBJECT_HANDLE_INVALID NULL

typedef buf_ucount_t vulkan_render_queue_handle_t;
#define VULKAN_RENDER_QUEUE_HANDLE_INVALID (~0ULL)

typedef struct vulkan_render_scene_object_handle_t
{
	vulkan_render_object_handle_t object_handle;	 // handle to the render object a render queue
	vulkan_render_queue_handle_t queue_handle; 	 // handle to the render queue in the render scene
} vulkan_render_scene_object_handle_t;

#define VULKAN_RENDER_SCENE_OBJECT_HANDLE_INVALID (vulkan_render_scene_object_handle_t) { VULKAN_RENDER_OBJECT_HANDLE_INVALID, VULKAN_RENDER_QUEUE_HANDLE_INVALID }
