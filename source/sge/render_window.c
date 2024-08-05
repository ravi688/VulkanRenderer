/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#include <sge/render_window.h>
#include <sge/debug.h>
#include <sge/assert.h>
#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_instance.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory.h>


#if GLOBAL_DEBUG
static void glfwErrorCallback(int code, const char* description)
{
	printf("GLFW: %d, %s", code, description);
}
#endif

static void glfwOnWindowResizeCallback(GLFWwindow* glfw_window, int width, int height)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	render_window_get_framebuffer_extent(window, &window->width, &window->height);
	event_publish(window->on_resize_event);
}

static void glfwOnCursorMoveCallback(GLFWwindow* glfw_window, double x, double y)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->cursor_pos.x = x;
	window->cursor_pos.y = y;
	if(window->on_cursor_move_event != NULL)
		event_publish(window->on_cursor_move_event);
}

static void glfwOnScrollCallback(GLFWwindow* glfw_window, double x, double y)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->scroll_delta.x = x;
	window->scroll_delta.y = y;
	if(window->on_scroll_event != NULL)
		event_publish(window->on_scroll_event);
}

static key_event_type_t get_key_event_type(int event_type)
{
	switch(event_type)
	{
		case GLFW_PRESS: return KEY_EVENT_TYPE_PRESS;
		case GLFW_RELEASE: return KEY_EVENT_TYPE_RELEASE;
		default:
		{
			debug_log_error("Unknown glfw event type");
			return KEY_EVENT_TYPE_UNDEFINED;
		}
	}
}

static mouse_button_type_t get_mouse_button_type(int button)
{
	switch(button)
	{
		case GLFW_MOUSE_BUTTON_LEFT: return MOUSE_BUTTON_TYPE_LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT: return MOUSE_BUTTON_TYPE_RIGHT;
		case GLFW_MOUSE_BUTTON_MIDDLE: return MOUSE_BUTTON_TYPE_MIDDLE;
		default:
		{
			debug_log_error("Uknown glfw mouse button type");
			return MOUSE_BUTTON_TYPE_UNDEFINED;
		}
	}
}

static void glfwOnMouseButtonCallback(GLFWwindow* glfw_window, int button, int event_type, int modifiers)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->mouse_button_type = get_mouse_button_type(button);
	window->key_event_type = get_key_event_type(event_type);
	if(window->on_mouse_button_event != NULL)
		event_publish(window->on_mouse_button_event);
}

SGE_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	render_window_t* window = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDER_WINDOW, render_window_t);
	memzero(window, render_window_t);
	OBJECT_INIT(window, OBJECT_TYPE_RENDER_WINDOW, OBJECT_NATIONALITY_INTERNAL);
	window->allocator = allocator;
	window->on_resize_event = event_create(allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Resize"));
	glfwInit();
#if GLOBAL_DEBUG
	glfwSetErrorCallback(glfwErrorCallback);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	if(full_screen)
		window->handle = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
	else
		window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetFramebufferSizeCallback(window->handle, glfwOnWindowResizeCallback);
	glfwSetCursorPosCallback(window->handle, glfwOnCursorMoveCallback);
	glfwSetMouseButtonCallback(window->handle, glfwOnMouseButtonCallback);
	glfwSetScrollCallback(window->handle, glfwOnScrollCallback);
	glfwSetWindowUserPointer(window->handle, window);
	render_window_get_framebuffer_extent(window, &window->width, &window->height);
	log_msg("Render window created successfully\n");
	return window;
}

SGE_API bool render_window_should_close(render_window_t* window)
{
	return glfwWindowShouldClose(window->handle);
}

SGE_API void render_window_poll_events(render_window_t* window)
{
	glfwPollEvents();
}

SGE_API void render_window_destroy(render_window_t* window)
{
	_debug_assert__(OBJECT_IS_INTERNAL(window));

	if(window->on_cursor_move_event != NULL)
	{
		event_destroy(window->on_cursor_move_event);
		event_release_resources(window->on_cursor_move_event);
	}
	if(window->on_mouse_button_event != NULL)
	{
		event_destroy(window->on_mouse_button_event);
		event_release_resources(window->on_mouse_button_event);
	}
	if(window->on_scroll_event != NULL)
	{
		event_destroy(window->on_scroll_event);
		event_release_resources(window->on_scroll_event);
	}
	event_destroy(window->on_resize_event);
	event_release_resources(window->on_resize_event);
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	memory_allocator_dealloc(window->allocator, window);
	log_msg("Render window destroyed successfully\n");
}

SGE_API event_t* render_window_get_on_cursor_move_event(render_window_t* window)
{
	if(window->on_cursor_move_event == NULL)
		window->on_cursor_move_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Cursor-Move"));
	return window->on_cursor_move_event;
}

SGE_API event_t* render_window_get_on_mouse_button_event(render_window_t* window)
{
	if(window->on_mouse_button_event == NULL)
		window->on_mouse_button_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Mouse-Button"));
	return window->on_mouse_button_event;
}

SGE_API event_t* render_window_get_on_scroll_event(render_window_t* window)
{
	if(window->on_scroll_event == NULL)
		window->on_scroll_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Mouse-Scroll"));
	return window->on_scroll_event;
}

SGE_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height)
{
	int height, width;
	glfwGetFramebufferSize(window->handle, &width, &height);
	*out_width = width;
	*out_height = height;
}

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, vulkan_renderer_t* driver)
{
	VkSurfaceKHR surface;
	vkCall(glfwCreateWindowSurface(driver->instance->handle, window, VULKAN_ALLOCATION_CALLBACKS(driver), &surface));
	log_msg("Vulkan surface created successfully\n");
	return surface;
}

SGE_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface)
{
	VkSurfaceKHR surface = glfw_get_vulkan_surface(window->handle, CAST_TO(vulkan_renderer_t*, vk_driver));
	memcpy(out_surface, &surface, sizeof(VkSurfaceKHR));
}

SGE_API void render_window_get_glfw_window_ptr(render_window_t* window, void* out_ptr)
{
	memcpy(out_ptr, &window->handle, sizeof(void*));
}
