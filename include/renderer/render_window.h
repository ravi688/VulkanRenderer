
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/event.h>
#include <renderer/type_system.h>

static const type_t TYPE_ID(render_window_t) = TYPE_ID_CREATE(1);

typedef struct render_window_t
{
	memory_allocator_t* allocator;
	void* handle;
	// BUFFER* resize_event;
	u32 width;
	u32 height;
	// void* user_data;
	event_t* on_resize_event;
} render_window_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
RENDERER_API bool render_window_should_close(render_window_t* window);
RENDERER_API void render_window_poll_events(render_window_t* window);
RENDERER_API void render_window_destroy(render_window_t* window);

// getters
RENDERER_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height);

//API specific extensions
RENDERER_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface);

END_CPP_COMPATIBLE
