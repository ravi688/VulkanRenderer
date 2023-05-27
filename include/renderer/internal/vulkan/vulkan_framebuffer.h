/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_framebuffer.h is a part of VulkanRenderer

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


#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>
#include <bufferlib/buffer.h>

typedef struct vulkan_render_pass_t vulkan_render_pass_t;

/* structure to store create information for vulkan framebuffer object */
typedef struct vulkan_framebuffer_create_info_t
{
	vulkan_render_pass_t* render_pass;
	/* index of this framebuffer in the list */
	u32 id;	
	/* width of the framebuffer */
	u32 width;
	/* height of the framebuffer */
	u32 height;
	u32 attachment_count;
	VkImageView* attachments;
} vulkan_framebuffer_create_info_t;

typedef struct vulkan_framebuffer_t
{
	vulkan_renderer_t* renderer;
	VkFramebuffer vo_handle;

	/* used in vulkan_render_pass_begin() to check render_pass compatiblity */
	vulkan_render_pass_t* render_pass;

	/* width of this framebuffer */
	u32 width;
	/* height of this framebuffer */
	u32 height;
	u32 id; 		// index of this frambuffer in the list of the framebuffers (in case of double or tripple buffering)
} vulkan_framebuffer_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator);
RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info);
RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer);
RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer);
RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer);

END_CPP_COMPATIBLE

