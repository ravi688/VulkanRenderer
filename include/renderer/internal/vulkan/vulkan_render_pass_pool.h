/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_pool.h is a part of VulkanRenderer

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


#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_graph.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef struct vulkan_subpass_input_info_t
{
	u32* input_attachments;
	u32 input_attachment_count;
} vulkan_subpass_input_info_t;

typedef struct vulkan_render_pass_input_info_t
{
	u32* input_attachments;
	u32 input_attachment_count;
	vulkan_subpass_input_info_t* subpass_inputs;
} vulkan_render_pass_input_info_t;

typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_render_pass_create_info_t vulkan_render_pass_create_info_t;
typedef struct vulkan_render_pass_create_info_builder_t vulkan_render_pass_create_info_builder_t;

typedef struct vulkan_render_pass_pool_slot_t
{
	/* ptr to the vulkan_render_pass_t object */
	vulkan_render_pass_t* render_pass;
	/* for comparison */
	vulkan_render_pass_create_info_t* create_info;
	vulkan_render_pass_create_info_builder_t* create_info_builder;
	/* holds attachments input information from the previous pass */
	vulkan_render_pass_input_info_t input;
	/* handle to this slot in the pool/library */
	vulkan_render_pass_handle_t handle;
} vulkan_render_pass_pool_slot_t;

typedef struct vulkan_render_pass_pool_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	BUFFER relocation_table; 				// relocation table
	BUFFER slots; 							// list of vulkan_render_pass_pool_slot_t
	vulkan_render_pass_graph_t pass_graph;
	vulkan_render_pass_handle_t prev_pass_handle;
} vulkan_render_pass_pool_t;

#define VULKAN_RENDER_PASS_POOL(ptr) VULKAN_OBJECT_UP_CAST(vulkan_render_pass_pool_t*, VULKAN_OBJECT_TYPE_RENDER_PASS_POOL, ptr)
#define VULKAN_RENDER_PASS_POOL_CONST(ptr) VULKAN_OBJECT_UP_CAST(const vulkan_render_pass_pool_t*, VULKAN_OBJECT_TYPE_RENDER_PASS_POOL, ptr)


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_render_pass_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_pool_t OUT pool);
RENDERER_API void vulkan_render_pass_pool_destroy(vulkan_render_pass_pool_t* pool);
RENDERER_API void vulkan_render_pass_pool_release_resources(vulkan_render_pass_pool_t* pool);

/* logic functions */
RENDERER_API void vulkan_render_pass_pool_create_path(vulkan_render_pass_pool_t* pool);
RENDERER_API vulkan_render_pass_handle_t vulkan_render_pass_pool_create_pass(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info, vulkan_render_pass_input_info_t* input);
RENDERER_API vulkan_render_pass_pool_slot_t* vulkan_render_pass_pool_get_slotH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle);
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_pool_getH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle);
static FORCE_INLINE buf_ucount_t vulkan_render_pass_pool_get_count(vulkan_render_pass_pool_t* pool)
{
	return buf_get_element_count(&pool->slots);
}

static FORCE_INLINE vulkan_render_pass_pool_slot_t* vulkan_render_pass_pool_get_slot_at(vulkan_render_pass_pool_t* pool, buf_ucount_t index)
{
	return CAST_TO(vulkan_render_pass_pool_slot_t*, buf_get_ptr_at(&pool->slots, index));
}
static FORCE_INLINE vulkan_render_pass_t* vulkan_render_pass_pool_get_at(vulkan_render_pass_pool_t* pool, buf_ucount_t index)
{
	return vulkan_render_pass_pool_get_slot_at(pool, index)->render_pass;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_render_pass_handle_t vulkan_render_pass_pool_get_prev_pass_handle(vulkan_render_pass_pool_t* pool)
{
	return pool->prev_pass_handle;
}

END_CPP_COMPATIBLE
