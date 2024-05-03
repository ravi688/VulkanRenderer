/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_attachment_pool.h is a part of VulkanRenderer

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

/* TODO: Part of TID-49 
 * for now lets merge the branc TID-49/RenderGraph-Optimization */
#pragma once

typedef struct vulkan_renderer_t vulkan_renderer_t;

/* structure to hold vulkan attachment pool create information(s) */
typedef struct vulkan_attachment_pool_create_info_t
{

} vulkan_attachment_pool_create_info_t;

/* creates framebuffer attachmens upon request and minimizes the number of vulkan attachments (VkImagView(s)) */
typedef struct vulkan_attachment_pool_t
{
	
} vulkan_attachment_pool_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_attachment_pool_t* vulkan_attachment_pool_new(memory_allocator_t* allocator);
RENDERER_API vulkan_attachment_pool_t* vulkan_attachment_pool_create(vulkan_renderer_t* renderer, vulkan_attachment_pool_create_info_t* create_info);
RENDERER_API void vulkan_attachment_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_pool_create_info_t* create_info, vulkan_attachment_pool_t OUT pool);
RENDERER_API void vulkan_attachment_pool_destroy(vulkan_attachment_pool_t* pool);
RENDERER_API void vulkan_attachment_pool_release_resources(vulkan_attachment_pool_t* pool);

END_CPP_COMPATIBLE
