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
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/system/display.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <hpml/affine_transformation.h>
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

static void update_screen_info(render_window_t* window)
{
	/* set the window size value */
	AUTO size = iextent2d(window->width, window->height);
	struct_descriptor_set_uvec2(&window->screen_info_struct, window->size_field, CAST_TO(uint*, &size));

	/* set the screen matrix */
	AUTO matrix = mat4_ortho_projection(-0.04f, 100, size.height, (f32)size.width / (f32)size.height);
	mat4_move(matrix, mat4_transpose(matrix));
	struct_descriptor_set_mat4(&window->screen_info_struct, window->matrix_field, CAST_TO(f32*, &matrix));
}

static void glfwOnWindowResizeCallback(GLFWwindow* glfw_window, int width, int height)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	render_window_get_framebuffer_extent(window, &window->width, &window->height);
	/* update the screen matrix and window size in the global set */
	update_screen_info(window);
	event_publish(window->on_resize_event);
}

RENDERER_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	render_window_t* window = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDER_WINDOW, render_window_t);
	memzero(window, render_window_t);
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
	_debug_assert__(window->api_buffer != NULL);

	/* unmap the gpu buffer's data before destroying it */
	vulkan_buffer_unmap(VULKAN_BUFFER(window->api_buffer));
	vulkan_buffer_destroy(VULKAN_BUFFER(window->api_buffer));

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

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, vulkan_renderer_t* driver)
{
	VkSurfaceKHR surface;
	vkCall(glfwCreateWindowSurface(driver->instance->handle, window, VULKAN_ALLOCATION_CALLBACKS(driver), &surface));
	log_msg("Vulkan surface created successfully\n");
	return surface;
}

RENDERER_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface)
{
	VkSurfaceKHR surface = glfw_get_vulkan_surface(window->handle, CAST_TO(vulkan_renderer_t*, vk_driver));
	memcpy(out_surface, &surface, sizeof(VkSurfaceKHR));
}

RENDERER_API void render_window_initialize_api_buffer(render_window_t* window, void* vk_driver)
{
	_debug_assert_wrn__(window->api_buffer == NULL);

	/* definition of the 'screen info' struct */
	struct_descriptor_begin(window->allocator, &window->screen_info_struct, "screen_info", 0);
		struct_descriptor_add_field_uvec2(&window->screen_info_struct, "resolution");
		struct_descriptor_add_field_uvec2(&window->screen_info_struct, "dpi");
		struct_descriptor_add_field_uvec2(&window->screen_info_struct, "size");
		struct_descriptor_add_field_mat4(&window->screen_info_struct, "matrix");
	struct_descriptor_end(window->allocator, &window->screen_info_struct);

	/* get the field handles, to later use them to modify the field values */
	window->resolution_field = struct_descriptor_get_field_handle(&window->screen_info_struct, "resolution");
	window->dpi_field = struct_descriptor_get_field_handle(&window->screen_info_struct, "dpi");
	window->size_field = struct_descriptor_get_field_handle(&window->screen_info_struct, "size");
	window->matrix_field = struct_descriptor_get_field_handle(&window->screen_info_struct, "matrix");

	/* calculate the size of the struct and offsets of the fields inside it */
	struct_descriptor_recalculate(&window->screen_info_struct);

	/* create gpu buffer with the required size */
	vulkan_buffer_create_info_t create_info =
	{
		.size = struct_descriptor_sizeof(&window->screen_info_struct),
		.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	vulkan_buffer_t* buffer = vulkan_buffer_create(CAST_TO(vulkan_renderer_t*, vk_driver), &create_info);
	window->api_buffer = CAST_TO(void*, buffer);

	/* write to the descriptor with binding VULKAN_DESCRIPTOR_BINDING_SCREEN */
	vulkan_descriptor_set_write_uniform_buffer(&CAST_TO(vulkan_renderer_t*, vk_driver)->global_set, VULKAN_DESCRIPTOR_BINDING_SCREEN, buffer);

	/* map the gpu buffer to the host memory */
	struct_descriptor_map(&window->screen_info_struct, vulkan_buffer_map(buffer));

	/* set the display resolution value */
	AUTO resolution = display_get_resolution();
	struct_descriptor_set_uvec2(&window->screen_info_struct, window->size_field, CAST_TO(uint*, &resolution));

	/* set the display dpi value */
	AUTO dpi = display_get_dpi();
	struct_descriptor_set_uvec2(&window->screen_info_struct, window->dpi_field, CAST_TO(uint*, &dpi));

	/* set the screen matrix and window size */
	update_screen_info(window);
}
