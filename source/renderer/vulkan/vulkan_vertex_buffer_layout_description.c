/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_vertex_buffer_layout_description.c is a part of VulkanRenderer

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

/* here order matters, as vulkan_vertex_buffer_layout_description.h defines only VkFormat if vulkan/vulkan.h is not included,
 * but if it is include then the inclusion of vulkan/vulkan.h must come first to let the header define VULKAN_CORE_H_ 
 * here vulkan/vulkan_renderer.h includes vulkan/vulkan.h under the hood, hence it should come first. */
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>

#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <bufferlib/buffer.h>

#define create_buffer(allocator, type) __create_buffer(allocator, sizeof(type))
static BUFFER* __create_buffer(memory_allocator_t* allocator,  u32 size)
{
	BUFFER* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_BUFFER, BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

RENDERER_API void vulkan_vertex_buffer_layout_description_begin(memory_allocator_t* allocator,  vulkan_vertex_buffer_layout_description_t* description, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	description->binding = binding_number;
	description->input_rate = input_rate;
	description->size = stride;

	description->attribute_locations = CAST_TO(u32*, create_buffer(allocator, u32));
	description->attribute_formats = CAST_TO(VkFormat*, create_buffer(allocator, VkFormat));
	description->attribute_offsets = CAST_TO(u32*, create_buffer(allocator, u32));
}

RENDERER_API void vulkan_vertex_buffer_layout_description_add_attribute(vulkan_vertex_buffer_layout_description_t* description, u32 location, VkFormat format, u32 offset)
{
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_locations), location);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_formats), format);
	buf_push_auto(CAST_TO(BUFFER*, description->attribute_offsets), offset);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_end(memory_allocator_t* allocator,  vulkan_vertex_buffer_layout_description_t* description)
{
	BUFFER* buffer = CAST_TO(BUFFER*, description->attribute_locations);
	description->attribute_count = buf_get_element_count(buffer);
	description->attribute_locations = buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_formats);
	description->attribute_formats = buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
	buffer = CAST_TO(BUFFER*, description->attribute_offsets);
	description->attribute_offsets = buf_get_ptr(buffer);
	memory_allocator_dealloc(allocator, buffer);
}

RENDERER_API void vulkan_vertex_buffer_layout_description_destroy_allocations(memory_allocator_t* allocator,  vulkan_vertex_buffer_layout_description_t* description)
{
	if(description->attribute_count > 0)
	{
		_debug_assert__(description->attribute_locations != NULL);
		memory_allocator_dealloc(allocator, description->attribute_locations);
		_debug_assert__(description->attribute_formats != NULL);
		memory_allocator_dealloc(allocator, description->attribute_formats);
		_debug_assert__(description->attribute_offsets != NULL);
		memory_allocator_dealloc(allocator, description->attribute_offsets);
	}
}