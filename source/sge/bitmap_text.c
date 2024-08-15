/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bitmap_text.c is a part of VulkanRenderer

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

#include <sge/bitmap_text.h>
#include <sge/internal/vulkan/vulkan_bitmap_text.h>
#include <sge/memory_allocator.h>
#include <sge/sge.h>

SGE_API bitmap_text_t* bitmap_text_new(memory_allocator_t* allocator)
{
	return vulkan_bitmap_text_new(allocator);
}

SGE_API bitmap_text_t* bitmap_text_create(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_text_create_info_t vk_create_info = { .texture = texture };
	return vulkan_bitmap_text_create(VULKAN_RENDERER(renderer), &vk_create_info);
}

SGE_API void bitmap_text_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture, bitmap_text_t OUT text)
{
	vulkan_bitmap_text_create_info_t vk_create_info = { .texture = texture };
	vulkan_bitmap_text_create_no_alloc(VULKAN_RENDERER(renderer), &vk_create_info, text);
}

SGE_API void bitmap_text_destroy(bitmap_text_t* text)
{
	vulkan_bitmap_text_destroy(text);
}

SGE_API void bitmap_text_release_resources(bitmap_text_t* text)
{
	vulkan_bitmap_text_release_resources(text);
}

SGE_API void bitmap_text_draw(bitmap_text_t* text)
{
	vulkan_bitmap_text_draw(text);
}

SGE_API bitmap_text_string_handle_t bitmap_text_string_create(bitmap_text_t* text)
{
	return vulkan_bitmap_text_string_create(text);
}

SGE_API void bitmap_text_string_destroyH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	vulkan_bitmap_text_string_destroyH(text, handle);
}

/* setters */
SGE_API void bitmap_text_set_point_size(bitmap_text_t* text, u32 point_size)
{
	vulkan_bitmap_text_set_point_size(text, point_size);
}

SGE_API void bitmap_text_set_material(bitmap_text_t* text, material_t* material)
{
	vulkan_bitmap_text_set_material(text, material);
}

SGE_API void bitmap_text_set_render_space_type(bitmap_text_t* text, bitmap_text_render_space_type_t space_type)
{
	vulkan_bitmap_text_render_space_type_t vk_space_type;
	switch(space_type)
	{
		case BITMAP_TEXT_RENDER_SPACE_TYPE_2D:
			vk_space_type = VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_2D;
			break;
		case BITMAP_TEXT_RENDER_SPACE_TYPE_3D:
			vk_space_type = VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_3D;
			break;
		default:
			debug_log_error("Invalid bitmap text render space type: %lu", space_type);
			break;
	}
	vulkan_bitmap_text_set_render_space_type(text, vk_space_type);
}

SGE_API void bitmap_text_set_render_surface_type(bitmap_text_t* text, bitmap_text_render_surface_type_t surface_type)
{
	vulkan_bitmap_text_render_surface_type_t vk_surface_type;
	switch(surface_type)
	{
		case BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA:
			vk_surface_type = VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA;
			break;
		case BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN:
			vk_surface_type = VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN;
			break;
		default:
			debug_log_error("Invalid bitmap text render surface type: %lu", surface_type);
			break;
	}
	vulkan_bitmap_text_set_render_surface_type(text, vk_surface_type);
}

SGE_API void bitmap_text_string_setH(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char* string)
{
	vulkan_bitmap_text_string_setH(text, handle, string);
}

SGE_API void bitmap_text_string_set_point_sizeH(bitmap_text_t* text, bitmap_text_string_handle_t handle, u32 point_size)
{
	vulkan_bitmap_text_string_set_point_sizeH(text, handle, point_size);
}

SGE_API void bitmap_text_string_set_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle, mat4_t transform)
{
	vulkan_bitmap_text_string_set_transformH(text, handle, transform);
}

SGE_API void bitmap_text_string_set_color(bitmap_text_t* text, bitmap_text_string_handle_t handle, color_t color)
{
	vulkan_bitmap_text_string_set_color(VULKAN_BITMAP_TEXT_CAST(text), handle, color);
}

SGE_API void bitmap_text_string_set_char_attr_color(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char_attr_color_range_t* ranges, u32 range_count)
{
	vulkan_bitmap_text_string_set_char_attr_color(VULKAN_BITMAP_TEXT_CAST(text), handle, ranges, range_count);
}

/* getters */
SGE_API u32 bitmap_text_get_point_size(bitmap_text_t* text)
{
	return vulkan_bitmap_text_get_point_size(text);
}

SGE_API const char* bitmap_text_string_getH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_getH(text, handle);
}

SGE_API u32 bitmap_text_string_get_point_sizeH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_point_sizeH(text, handle);
}

SGE_API u32 bitmap_text_string_get_lengthH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_lengthH(text, handle);
}

SGE_API f32 bitmap_text_string_get_zcoord_from_glyph_index(bitmap_text_t* text, bitmap_text_string_handle_t handle, u32 index)
{
	return vulkan_bitmap_text_string_get_zcoord_from_glyph_index(VULKAN_BITMAP_TEXT_CAST(text), handle, index);
}

SGE_API u32 bitmap_text_string_get_glyph_index_from_zcoord(bitmap_text_t* text, bitmap_text_string_handle_t handle, f32 zcoord)
{
	return vulkan_bitmap_text_string_get_glyph_index_from_zcoord(VULKAN_BITMAP_TEXT_CAST(text), handle, zcoord);
}

SGE_API mat4_t bitmap_text_string_get_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_transformH(text, handle);
}
