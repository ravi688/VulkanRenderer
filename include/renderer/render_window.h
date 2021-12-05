
#pragma once

#include <defines.h>
#include <buffer.h>

typedef struct render_window_t
{
	void* handle;
	BUFFER* resize_event;
	u32 width;
	u32 height;
	void* user_data;
} render_window_t;

render_window_t* render_window_init(u32 width, u32 height, const char* title, bool full_screen);
bool render_window_should_close(render_window_t* window);
void render_window_poll_events(render_window_t* window);
void render_window_destroy(render_window_t* window);
void render_window_subscribe_on_resize(render_window_t* window, void (*callback)(render_window_t* window, void* user_data), void* user_data);
void render_window_unsubscribe_on_resize(render_window_t* window, void (*callback)(render_window_t* window, void* user_data));

//API specific extensions
void render_window_get_vulkan_surface(render_window_t* window, void* vulkan_instance, void* out_surface);

