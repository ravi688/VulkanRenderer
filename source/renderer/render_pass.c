/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_pass.c is a part of VulkanRenderer

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


#include <renderer/render_pass.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

/* constructors and destructors */
SGE_API render_pass_t* render_pass_new(memory_allocator_t* allocator)
{
	return vulkan_render_pass_new(allocator);
}

SGE_API render_pass_t* render_pass_create(renderer_t* renderer, render_pass_create_info_t* create_info)
{
	return vulkan_render_pass_create(renderer->vulkan_handle, create_info);
}

SGE_API void render_pass_create_no_alloc(renderer_t* renderer, render_pass_create_info_t* create_info, render_pass_t OUT pass)
{
	vulkan_render_pass_create_no_alloc(renderer->vulkan_handle, create_info, pass);
}

SGE_API void render_pass_destroy(render_pass_t* pass)
{
	vulkan_render_pass_destroy(pass);
}

SGE_API void render_pass_release_resources(render_pass_t* pass)
{
	vulkan_render_pass_release_resources(pass);
}

SGE_API void render_pass_set_clear(render_pass_t* pass, color_t color, float depth)
{
	vulkan_render_pass_set_clear(pass, color, depth);
}

SGE_API void render_pass_begin(render_pass_t* pass, void* api_specific, camera_t* camera)
{
	u32 framebuffer_index;
	if(api_specific == NULL)
		framebuffer_index = VULKAN_RENDER_PASS_FRAMEBUFFER_INDEX_SWAPCHAIN;
	else
		framebuffer_index = DEREF_TO(u32, api_specific);
	vulkan_render_pass_begin(pass, framebuffer_index, camera);
}

SGE_API void render_pass_end(render_pass_t* pass)
{
	vulkan_render_pass_end(pass);
}
