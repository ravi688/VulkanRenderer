
#pragma once

#include <defines.h>

typedef struct render_window_t
{
	void* handle;
	void (*resize_callback)(void* window);
	void (*resize_callback1)(void* window);
	u32 width;
	u32 height;
	void* user_data;
} render_window_t;

render_window_t* render_window_init(u32 width, u32 height, const char* title);
bool render_window_should_close(render_window_t* window);
void render_window_poll_events(render_window_t* window);
void render_window_destroy(render_window_t* window);
void __render_window_set_resize_callback(render_window_t* window, void (*callback)(void* window));

#define render_window_set_resize_callback(window, callback) __render_window_set_resize_callback(window, (void (*)(void*))(callback))

//API specific extensions
void render_window_get_vulkan_surface(render_window_t* window, void* vulkan_instance, void* out_surface);

