/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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


#include <renderer/render_window.h>
#include <renderer/debug.h>
#include <renderer/assert.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_instance.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory.h>


static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, vulkan_renderer_t* driver);

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

RENDERER_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	render_window_t* window = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDER_WINDOW, render_window_t);
	memzero(window, render_window_t);
	window->allocator = allocator;
	window->on_resize_event = event_create(allocator, (void*)window);
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
	glfwSetWindowUserPointer(window->handle, window);
	window->width = width;
	window->height = height;
	log_msg("Render window created successfully\n");
	return window;
}

RENDERER_API bool render_window_should_close(render_window_t* window)
{
	return glfwWindowShouldClose(window->handle);
}

RENDERER_API void render_window_poll_events(render_window_t* window)
{
	glfwPollEvents();
}

RENDERER_API void render_window_destroy(render_window_t* window)
{
	event_destroy(window->on_resize_event);
	event_release_resources(window->on_resize_event);
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	memory_allocator_dealloc(window->allocator, window);
	log_msg("Render window destroyed successfully\n");
}

RENDERER_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height)
{
	glfwGetFramebufferSize(window->handle, out_width, out_height);
}

RENDERER_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface)
{
	VkSurfaceKHR surface = glfw_get_vulkan_surface(window->handle, CAST_TO(vulkan_renderer_t*, vk_driver));
	memcpy(out_surface, &surface, sizeof(VkSurfaceKHR));
}

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, vulkan_renderer_t* driver)
{
	VkSurfaceKHR surface; 
	vkCall(glfwCreateWindowSurface(driver->instance->handle, window, VULKAN_ALLOCATION_CALLBACKS(driver), &surface));
	log_msg("Vulkan surface created successfully\n");
	return surface;
}

