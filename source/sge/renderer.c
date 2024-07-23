/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: renderer.c is a part of VulkanRenderer

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


#include <sge/sge.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/alloc.h>

#include <sge/shader_library.h>
#include <sge/material_library.h>
#include <sge/render_pass_pool.h>
#include <sge/string_builder.h>

#include <freetype/freetype.h>

#include <sge/pygen/shaders.h>

SGE_API renderer_t* renderer_init(memory_allocator_t* allocator, sge_driver_create_info_t* _create_info)
{
	renderer_t* renderer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDERER, renderer_t);
	memzero(renderer, renderer_t);

	renderer->allocator = allocator;
	IF_DEBUG( renderer->debug_log_builder = string_builder_create(allocator, 2048) );

	/* create and initialize the free type library */
	AUTO error = FT_Init_FreeType(&renderer->ft_library);
	if(error != 0)
		debug_log_error("Failed to initialize free type library");

	// create the renderer
	vulkan_renderer_create_info_t create_info = 
	{
		.renderer = renderer,
		.prefered_gpu_type = CAST_TO(vulkan_renderer_gpu_type_t, _create_info->gpu_type), 
		.width = _create_info->width, 
		.height = _create_info->height, 
		.title = _create_info->title, 
		.full_screen = _create_info->full_screen, 
		.resizable = _create_info->resizable,
		.require_bitmap_text = _create_info->require_bitmap_text,
		.max_point_lights = _create_info->max_point_lights,
		.max_spot_lights = _create_info->max_spot_lights,
		.max_far_lights = _create_info->max_far_lights
	};
	renderer->vulkan_handle = vulkan_renderer_create(&create_info);

	return renderer;
}

SGE_API void renderer_terminate(renderer_t* renderer)
{
	// terminate the renderer
	vulkan_renderer_destroy(renderer->vulkan_handle);

	/* destroy free type library context */
	FT_Done_FreeType(renderer->ft_library);

	IF_DEBUG( string_builder_destroy(renderer->debug_log_builder) );
	memory_allocator_dealloc(renderer->allocator, renderer);
}

SGE_API void renderer_update(renderer_t* renderer)
{
	vulkan_renderer_update(renderer->vulkan_handle);
}

SGE_API bool renderer_is_running(renderer_t* renderer)
{
	return vulkan_renderer_is_running(renderer->vulkan_handle);
}


SGE_API void renderer_begin_frame(renderer_t* renderer)
{
	vulkan_renderer_begin_frame(renderer->vulkan_handle);
}

SGE_API void renderer_end_frame(renderer_t* renderer)
{
	vulkan_renderer_end_frame(renderer->vulkan_handle);
}

SGE_API void renderer_wait_idle(renderer_t* renderer)
{
	vulkan_renderer_wait_idle(renderer->vulkan_handle);
}

/* getters */
static bool str_ends_with(const char* str, const char* suffix)
{
	u32 str_len = strlen(str);
	u32 sfx_len = strlen(suffix);

	/* screening case */
	if(str_len < sfx_len)
		return false;

	return strncmp(str + (str_len - sfx_len), suffix, sfx_len) == 0;
}

SGE_API const char* renderer_get_builtin_file_data(renderer_t* renderer, const char* virtual_file_path, u64 OUT data_size)
{
	_debug_assert__(strlen(virtual_file_path) > 0);
	if(virtual_file_path[0] != '/')
	{
		debug_log_error("Invalid root directory in %s", virtual_file_path);
		return NULL;
	}

	/* skip first '/' (possibly multiple adjacent) characters */
	while(*virtual_file_path == '/')
		virtual_file_path += 1;

	/* find a match */
	u32 match_index = U32_MAX;
	const shader_file_path_and_data_mapping_t* mappings = g_get_shader_mappings();
	for(u32 i = 0; i < G_SHADER_MAPPING_COUNT; i++)
	{
		if(str_ends_with(mappings[i].file_path, virtual_file_path))
		{
			if(match_index != U32_MAX)
			{
				debug_log_error("Multiple matches were found, returning the first match: %s", mappings[i].file_path);
				break;
			}
			else match_index = i;
		}
	}

	if(match_index == U32_MAX)
	{
		debug_log_warning("No match found for virtual file path: %s, returning NULL", virtual_file_path);
		return NULL;
	}

	_debug_assert__(match_index < G_SHADER_MAPPING_COUNT);

	if(data_size != NULL)
		OUT data_size = CAST_TO(u64, mappings[match_index].data_size);

	return mappings[match_index].data;
}

SGE_API render_window_t* renderer_get_window(renderer_t* renderer)
{
	return vulkan_renderer_get_window(renderer->vulkan_handle);
}

SGE_API shader_library_t* renderer_get_shader_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->shader_library;
}

SGE_API material_library_t* renderer_get_material_library(renderer_t* renderer)
{
	return renderer->vulkan_handle->material_library;
}

SGE_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer)
{
	return renderer->vulkan_handle->render_pass_pool;
}

SGE_API camera_system_t* renderer_get_camera_system(renderer_t* renderer)
{
	return renderer->vulkan_handle->camera_system;
}
