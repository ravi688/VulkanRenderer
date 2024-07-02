/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_description_builder.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_render_pass_description_builder.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description_builder.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

SGE_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create(memory_allocator_t* allocator)
{
	vulkan_render_pass_description_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_DESCRIPTION_BUILDER, vulkan_render_pass_description_builder_t);
	memzero(builder, vulkan_render_pass_description_builder_t);
	builder->allocator = allocator;
	builder->description_array = memory_allocator_buf_new(allocator, vulkan_render_pass_description_t);
	builder->build_info_array = memory_allocator_buf_new(allocator, vulkan_render_pass_description_build_info_t);
	builder->bind_index = U32_MAX;
	return builder;
}

SGE_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_render_pass_description_t* descriptions, u32 description_count)
{
	NOT_IMPLEMENTED_FUNCTION();
	vulkan_render_pass_description_builder_t* builder = vulkan_render_pass_description_builder_create(allocator);
	// vulkan_render_pass_description_builder_add(builder, description_count);
	// for(u32 i = 0; i < description_count; i++)
	// {
		// vulkan_render_pass_description_builder_bind(builder, i);
	// }
	return builder;
}

SGE_API void vulkan_render_pass_description_builder_destroy(vulkan_render_pass_description_builder_t* builder)
{
	u32 count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO description = buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, i);
		vulkan_render_pass_description_destroy_allocations(builder->allocator, description);
	}
	buf_free(&builder->description_array);
	_debug_assert__(count == buf_get_element_count(&builder->build_info_array));
	for(u32 i = 0; i < count; i++)
	{
		AUTO vbld_builders = &buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_render_pass_description_build_info_t, i)->vbld_builders;
		u32 build_count = buf_get_element_count(vbld_builders);
		for(u32 j = 0; j < build_count; j++)
		{
			AUTO traits = buf_get_ptr_at_typeof(vbld_builders, vulkan_vertex_buffer_layout_description_builder_traits_t, j);
			if(traits->builder == NULL)
				/* no vertex info has been assigned to this subpass */
				continue;
			if(traits->is_destroy)
				vulkan_vertex_buffer_layout_description_builder_destroy(traits->builder);
		}
		buf_free(vbld_builders);
	}
	buf_free(&builder->build_info_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


SGE_API void vulkan_render_pass_description_builder_add(vulkan_render_pass_description_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->description_array, count);
	buf_push_pseudo(&builder->build_info_array, count);
}

SGE_API void vulkan_render_pass_description_builder_bind(vulkan_render_pass_description_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	builder->bind_index = index;
}


SGE_API vulkan_render_pass_description_t* vulkan_render_pass_description_builder_get(vulkan_render_pass_description_builder_t* builder)
{
	if(buf_get_element_count(&builder->description_array) == 0)
		return NULL;
	_debug_assert__(buf_get_element_count(&builder->description_array) == buf_get_element_count(&builder->build_info_array));
	AUTO render_passes = CAST_TO(vulkan_render_pass_description_t*, buf_get_ptr(&builder->description_array));
	AUTO count = vulkan_render_pass_description_builder_get_count(builder);
	for(u32 i = 0; i < count; i++)
	{
		AUTO vbld_builders = &buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_render_pass_description_build_info_t, i)->vbld_builders;
		u32 build_count = buf_get_element_count(vbld_builders);
		AUTO render_pass = &render_passes[i];
		_debug_assert__(build_count == render_pass->subpass_count);
		for(u32 j = 0; j < build_count; j++)
		{
			AUTO traits = buf_get_ptr_at_typeof(vbld_builders, vulkan_vertex_buffer_layout_description_builder_traits_t, j);
			if(traits->builder == NULL)
				/* no vertex info has been assigned to this subpass */
				continue;
			render_pass->subpass_descriptions[j].vertex_info_count = vulkan_vertex_buffer_layout_description_builder_get_count(traits->builder);
			if(render_pass->subpass_descriptions[j].vertex_info_count > 0)
				render_pass->subpass_descriptions[j].vertex_infos = vulkan_vertex_buffer_layout_description_builder_get(traits->builder);
		}
	}
	return render_passes;
}

SGE_API u32 vulkan_render_pass_description_builder_get_count(vulkan_render_pass_description_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->description_array));
}

static vulkan_render_pass_description_build_info_t* get_build_info(vulkan_render_pass_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_render_pass_description_build_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_render_pass_description_build_info_t* get_bound_build_info(vulkan_render_pass_description_builder_t* builder)
{
	return get_build_info(builder, builder->bind_index);
}

static vulkan_render_pass_description_t* get_description(vulkan_render_pass_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	return buf_get_ptr_at_typeof(&builder->description_array, vulkan_render_pass_description_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_render_pass_description_t* get_bound_description(vulkan_render_pass_description_builder_t* builder)
{
	return get_description(builder, builder->bind_index);
}


SGE_API void vulkan_render_pass_description_builder_begin_pass(vulkan_render_pass_description_builder_t* builder, vulkan_render_pass_type_t type)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_begin(builder->allocator, description, type);
	AUTO build_info = get_bound_build_info(builder);
	build_info->vbld_builders = memory_allocator_buf_new(builder->allocator, vulkan_vertex_buffer_layout_description_builder_traits_t);
}

SGE_API void vulkan_render_pass_description_builder_add_input(vulkan_render_pass_description_builder_t* builder, glsl_type_t type, u32 index, u32 binding)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_input(builder->allocator, description, type, index, binding);
}

SGE_API void vulkan_render_pass_description_builder_add_attachment(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_type_t type)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_attachment(description, type);
}


SGE_API void vulkan_render_pass_description_builder_begin_subpass(vulkan_render_pass_description_builder_t* builder, u32 pipeline_index)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_begin_subpass(builder->allocator, description, pipeline_index);
	AUTO build_info = get_bound_build_info(builder);
	buf_push_pseudo(&build_info->vbld_builders, 1);
}

SGE_API void vulkan_render_pass_description_builder_set_vertex_infos_builder(vulkan_render_pass_description_builder_t* builder, vulkan_vertex_buffer_layout_description_builder_t* vbld_builder, bool is_destroy)
{
	AUTO build_info = get_bound_build_info(builder);
	AUTO traits = CAST_TO(vulkan_vertex_buffer_layout_description_builder_traits_t*, buf_peek_ptr(&build_info->vbld_builders));
	traits->builder = vbld_builder;
	traits->is_destroy = is_destroy;
}

SGE_API void vulkan_render_pass_description_builder_add_attachment_reference(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_attachment_reference(builder->allocator, description, type, reference, binding);
}

SGE_API void vulkan_render_pass_description_builder_end_subpass(vulkan_render_pass_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_end_subpass(builder->allocator, description);
}


SGE_API void vulkan_render_pass_description_builder_add_subpass_dependency(vulkan_render_pass_description_builder_t* builder, VkSubpassDependency* dependency)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_add_subpass_dependency(description, dependency);
}

SGE_API void vulkan_render_pass_description_builder_end_pass(vulkan_render_pass_description_builder_t* builder)
{
	AUTO description = get_bound_description(builder);
	vulkan_render_pass_description_end(builder->allocator, description);
}
