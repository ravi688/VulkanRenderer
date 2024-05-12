/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_create_info_builder.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_shader_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_shader.h> // vulkan_shader_create_info_t
#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description_builder.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description_builder.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description_builder.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_shader_create_info_builder_t* vulkan_shader_create_info_builder_create(memory_allocator_t* allocator)
{
	vulkan_shader_create_info_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_CREATE_INFO_BUILDER, vulkan_shader_create_info_builder_t);
	memzero(builder, vulkan_shader_create_info_builder_t);
	builder->allocator = allocator;
	builder->create_info_array = memory_allocator_buf_new(allocator, vulkan_shader_create_info_t);
	builder->build_info_array = memory_allocator_buf_new(allocator, vulkan_shader_create_info_build_info_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API void vulkan_shader_create_info_builder_destroy(vulkan_shader_create_info_builder_t* builder)
{
	buf_free(&builder->create_info_array);
	u32 build_info_count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < build_info_count; i++)
	{
		AUTO build_info = buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_shader_create_info_build_info_t, i);
		if((build_info->msb_builder != NULL) && build_info->is_destroy_msb_builder)
			vulkan_shader_resource_description_builder_destroy(build_info->msb_builder);
		if((build_info->vbld_builder != NULL) && build_info->is_destroy_vbld_builder)
			vulkan_vertex_buffer_layout_description_builder_destroy(build_info->vbld_builder);
		if((build_info->rpd_builder != NULL) && build_info->is_destroy_rpd_builder)
			vulkan_render_pass_description_builder_destroy(build_info->rpd_builder);
		if((build_info->gfx_pipes_builder != NULL) && build_info->is_destroy_gfx_pipes_builder)
			vulkan_graphics_pipeline_description_builder_destroy(build_info->gfx_pipes_builder);
	}
	buf_free(&builder->build_info_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_shader_create_info_builder_add(vulkan_shader_create_info_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->create_info_array, count);
	buf_push_pseudo(&builder->build_info_array, count);
}

RENDERER_API void vulkan_shader_create_info_builder_bind(vulkan_shader_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	builder->bind_index = index;
}

static vulkan_shader_create_info_build_info_t* get_build_info(vulkan_shader_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->build_info_array));
	return buf_get_ptr_at_typeof(&builder->build_info_array, vulkan_shader_create_info_build_info_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_shader_create_info_build_info_t* get_bound_build_info(vulkan_shader_create_info_builder_t* builder)
{
	return get_build_info(builder, builder->bind_index);
}

static vulkan_shader_create_info_t* get_create_info(vulkan_shader_create_info_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->create_info_array));
	return buf_get_ptr_at_typeof(&builder->create_info_array, vulkan_shader_create_info_t, index);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_shader_create_info_t* get_bound_create_info(vulkan_shader_create_info_builder_t* builder)
{
	return get_create_info(builder, builder->bind_index);
}

RENDERER_API vulkan_shader_create_info_t* vulkan_shader_create_info_builder_get(vulkan_shader_create_info_builder_t* builder)
{
	if(buf_get_element_count(&builder->create_info_array) == 0)
		return NULL;
	_debug_assert__(buf_get_element_count(&builder->build_info_array) == buf_get_element_count(&builder->create_info_array));
	u32 count = buf_get_element_count(&builder->build_info_array);
	for(u32 i = 0; i < count; i++)
	{
		AUTO build_info = get_build_info(builder, i);
		AUTO create_info = get_create_info(builder, i);
		if(build_info->msb_builder != NULL)
		{
			create_info->material_set_binding_count = vulkan_shader_resource_description_builder_get_count(build_info->msb_builder);
			if(create_info->material_set_binding_count > 0)
				create_info->material_set_bindings = vulkan_shader_resource_description_builder_get(build_info->msb_builder);
		}
		if(build_info->vbld_builder != NULL)
		{
			create_info->vertex_info_count = vulkan_vertex_buffer_layout_description_builder_get_count(build_info->vbld_builder);
			if(create_info->vertex_info_count > 0)
				create_info->vertex_infos = vulkan_vertex_buffer_layout_description_builder_get(build_info->vbld_builder);
		}
		if(build_info->rpd_builder != NULL)
		{
			create_info->render_pass_description_count = vulkan_render_pass_description_builder_get_count(build_info->rpd_builder);
			if(create_info->render_pass_description_count > 0)
				create_info->render_pass_descriptions = vulkan_render_pass_description_builder_get(build_info->rpd_builder);
		}
		if(build_info->gfx_pipes_builder != NULL)
		{
			create_info->pipeline_description_count = vulkan_graphics_pipeline_description_builder_get_count(build_info->gfx_pipes_builder);
			if(create_info->pipeline_description_count > 0)
				create_info->pipeline_descriptions = vulkan_graphics_pipeline_description_builder_get(build_info->gfx_pipes_builder);
		}
	}
	return CAST_TO(vulkan_shader_create_info_t*, buf_get_ptr(&builder->create_info_array));
}

RENDERER_API u32 vulkan_shader_create_info_builder_get_count(vulkan_shader_create_info_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->create_info_array));
}

RENDERER_API void vulkan_shader_create_info_builder_set_material_set_bindings_builder(vulkan_shader_create_info_builder_t* builder, vulkan_shader_resource_description_builder_t* srd_builder, bool is_destroy)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_msb_builder = true;
	build_info->is_destroy_msb_builder = is_destroy;
	build_info->msb_builder = srd_builder;
}

RENDERER_API void vulkan_shader_create_info_builder_set_vertex_buffer_layout_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_vertex_buffer_layout_description_builder_t* vbld_builder, bool is_destroy)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_vbld_builder = true;
	build_info->is_destroy_vbld_builder = is_destroy;
	build_info->vbld_builder = vbld_builder;
}

RENDERER_API void vulkan_shader_create_info_builder_set_render_pass_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_render_pass_description_builder_t* rpd_builder, bool is_destroy)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_rpd_builder = true;
	build_info->is_destroy_rpd_builder = is_destroy;
	build_info->rpd_builder = rpd_builder;
}

RENDERER_API void vulkan_shader_create_info_builder_set_graphics_pipeline_descriptions_builder(vulkan_shader_create_info_builder_t* builder, vulkan_graphics_pipeline_description_builder_t* gfx_pipes_builder, bool is_destroy)
{
	AUTO build_info = get_bound_build_info(builder);
	build_info->is_use_gfx_pipes_builder = true;
	build_info->is_destroy_gfx_pipes_builder = is_destroy;
	build_info->gfx_pipes_builder = gfx_pipes_builder;
}
