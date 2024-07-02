/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_graphics_pipeline_description_builder.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_graphics_pipeline_description_builder.h>
#include <sge/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <sge/memory_allocator.h>
#include <sge/assert.h>

SGE_API vulkan_graphics_pipeline_description_builder_t* vulkan_graphics_pipeline_description_builder_create(memory_allocator_t* allocator)
{
	vulkan_graphics_pipeline_description_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_DESCRIPTION_BUILDER, vulkan_graphics_pipeline_description_builder_t);
	memzero(builder, vulkan_graphics_pipeline_description_builder_t);
	builder->allocator = allocator;
	builder->description_array = memory_allocator_buf_new(allocator, vulkan_graphics_pipeline_description_t);
	builder->bind_index = U32_MAX;
	return builder;
}

SGE_API vulkan_graphics_pipeline_description_builder_t* vulkan_graphics_pipeline_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_graphics_pipeline_description_t* descriptions, u32 description_count)
{
	NOT_IMPLEMENTED_FUNCTION();
	vulkan_graphics_pipeline_description_builder_t* builder = vulkan_graphics_pipeline_description_builder_create(allocator);
	// vulkan_graphics_pipeline_description_builder_add(builder, description_count);
	// for(u32 i = 0; i < description_count; i++)
	// {
		// vulkan_graphics_pipeline_description_builder_bind(builder, i);
	// }
	return builder;
}

SGE_API void vulkan_graphics_pipeline_description_builder_destroy(vulkan_graphics_pipeline_description_builder_t* builder)
{
	u32 count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO description = buf_get_ptr_at_typeof(&builder->description_array, vulkan_graphics_pipeline_description_t, i);
		vulkan_graphics_pipeline_description_destroy_allocations(builder->allocator, description);
	}
	buf_free(&builder->description_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


SGE_API void vulkan_graphics_pipeline_description_builder_add(vulkan_graphics_pipeline_description_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->description_array, count);
}

SGE_API void vulkan_graphics_pipeline_description_builder_bind(vulkan_graphics_pipeline_description_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	builder->bind_index = index;
}


SGE_API vulkan_graphics_pipeline_description_t* vulkan_graphics_pipeline_description_builder_get(vulkan_graphics_pipeline_description_builder_t* builder)
{
	if(buf_get_element_count(&builder->description_array) == 0)
		return NULL;
	return CAST_TO(vulkan_graphics_pipeline_description_t*, buf_get_ptr(&builder->description_array));
}

SGE_API u32 vulkan_graphics_pipeline_description_builder_get_count(vulkan_graphics_pipeline_description_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->description_array));
}


static vulkan_graphics_pipeline_description_t* get_description(vulkan_graphics_pipeline_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	return buf_get_ptr_at_typeof(&builder->description_array, vulkan_graphics_pipeline_description_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_graphics_pipeline_description_t* get_bound_description(vulkan_graphics_pipeline_description_builder_t* builder)
{
	return get_description(builder, builder->bind_index);
}

SGE_API void vulkan_graphics_pipeline_description_builder_begin_pipeline(vulkan_graphics_pipeline_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_graphics_pipeline_description_begin(builder->allocator, description);
}

SGE_API void vulkan_graphics_pipeline_description_builder_add_color_blend_state(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_blend_enable)
{
	AUTO description = get_bound_description(builder);
	vulkan_graphics_pipeline_description_add_color_blend_state(description, is_blend_enable);
}

SGE_API void vulkan_graphics_pipeline_description_builder_add_shader(vulkan_graphics_pipeline_description_builder_t* builder, const char* file_path, vulkan_shader_type_t shader_type)
{
	AUTO description = get_bound_description(builder);
	vulkan_graphics_pipeline_description_add_shader(description, file_path, shader_type);
}

SGE_API void vulkan_graphics_pipeline_description_builder_set_depth_stencil(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_depth_write, VkBool32 is_depth_test)
{
	AUTO description = get_bound_description(builder);
	vulkan_graphics_pipeline_description_set_depth_stencil(description, is_depth_write, is_depth_test);
}

SGE_API void vulkan_graphics_pipeline_description_builder_end_pipeline(vulkan_graphics_pipeline_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_graphics_pipeline_description_end(builder->allocator, description);
}
