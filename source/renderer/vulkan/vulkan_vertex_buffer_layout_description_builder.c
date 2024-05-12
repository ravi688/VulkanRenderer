/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_vertex_buffer_layout_description_builder.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description_builder.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_vertex_buffer_layout_description_builder_t* vulkan_vertex_buffer_layout_description_builder_create(memory_allocator_t* allocator)
{
	vulkan_vertex_buffer_layout_description_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_DESCRIPTION_BUILDER, vulkan_vertex_buffer_layout_description_builder_t);
	memzero(builder, vulkan_vertex_buffer_layout_description_builder_t);
	builder->allocator = allocator;
	builder->description_array = memory_allocator_buf_new(allocator, vulkan_vertex_buffer_layout_description_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API vulkan_vertex_buffer_layout_description_builder_t* vulkan_vertex_buffer_layout_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_vertex_buffer_layout_description_t* descriptions, u32 description_count)
{
	NOT_IMPLEMENTED_FUNCTION();
	vulkan_vertex_buffer_layout_description_builder_t* builder = vulkan_vertex_buffer_layout_description_builder_create(allocator);
	// vulkan_vertex_buffer_layout_description_builder_add(builder, description_count);
	// for(u32 i = 0; i < description_count; i++)
	// {
		// vulkan_vertex_buffer_layout_description_builder_bind(builder, i);
	// }
	return builder;
}

RENDERER_API void vulkan_vertex_buffer_layout_description_builder_destroy(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	u32 count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO description = buf_get_ptr_at_typeof(&builder->description_array, vulkan_vertex_buffer_layout_description_t, i);
		vulkan_vertex_buffer_layout_description_destroy_allocations(builder->allocator, description);
	}
	buf_free(&builder->description_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_vertex_buffer_layout_description_builder_add(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->description_array, count);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_builder_bind(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	builder->bind_index = index;
}


RENDERER_API vulkan_vertex_buffer_layout_description_t* vulkan_vertex_buffer_layout_description_builder_get(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	if(buf_get_element_count(&builder->description_array) == 0)
		return NULL;
	return CAST_TO(vulkan_vertex_buffer_layout_description_t*, buf_get_ptr(&builder->description_array));
}

RENDERER_API u32 vulkan_vertex_buffer_layout_description_builder_get_count(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->description_array));
}


static vulkan_vertex_buffer_layout_description_t* get_description(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	return buf_get_ptr_at_typeof(&builder->description_array, vulkan_vertex_buffer_layout_description_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_vertex_buffer_layout_description_t* get_bound_description(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	return get_description(builder, builder->bind_index);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_builder_begin(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	AUTO description = get_bound_description(builder);
	vulkan_vertex_buffer_layout_description_begin(builder->allocator, description, stride, input_rate, binding_number);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_builder_add_attribute(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 location, VkFormat format, u32 offset)
{
	AUTO description = get_bound_description(builder);
	vulkan_vertex_buffer_layout_description_add_attribute(description, location, format, offset);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_builder_end(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_vertex_buffer_layout_description_end(builder->allocator, description);
}
