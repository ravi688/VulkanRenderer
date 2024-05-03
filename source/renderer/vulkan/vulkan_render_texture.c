/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_texture.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_render_texture.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/debug.h>

RENDERER_API vulkan_render_texture_t* vulkan_render_texture_create(vulkan_renderer_t* renderer, vulkan_render_texture_create_info_t* create_info)
{
	vulkan_texture_t* texture = vulkan_texture_new(renderer->allocator);
	vulkan_render_texture_create_no_alloc(renderer, create_info, texture);
	return texture;
}

RENDERER_API void vulkan_render_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_texture_create_info_t* create_info, vulkan_render_texture_t OUT texture)
{
	vulkan_texture_create_info_t _create_info = 
	{
		.width = create_info->width,
		.height = create_info->height,
		.depth = create_info->depth,
		.channel_count = create_info->channel_count
	};

	switch(create_info->type)
	{
		case VULKAN_RENDER_TEXTURE_TYPE_ALBEDO:
			_create_info.type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_RENDER_TARGET; 
			break;
		case VULKAN_RENDER_TEXTURE_TYPE_COLOR:
			_create_info.type = VULKAN_TEXTURE_TYPE_COLOR | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
			break;
		case VULKAN_RENDER_TEXTURE_TYPE_DEPTH:
			_create_info.type = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
			break;
		case VULKAN_RENDER_TEXTURE_TYPE_ALBEDO_CUBE:
			_create_info.type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
			break;
		case VULKAN_RENDER_TEXTURE_TYPE_COLOR_CUBE:
			_create_info.type = VULKAN_TEXTURE_TYPE_COLOR | VULKAN_TEXTURE_TYPE_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
			break;
		case VULKAN_RENDER_TEXTURE_TYPE_DEPTH_CUBE:
			_create_info.type = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET;
			break;
		default:
			LOG_FETAL_ERR("Unrecognized vulkan_render_texture_type_t: %lu\n", create_info->type);
	}
	_create_info.initial_usage = VULKAN_TEXTURE_USAGE_SAMPLED;
	_create_info.usage = VULKAN_TEXTURE_USAGE_RENDER_TARGET;
	_create_info.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED;
	_create_info.technique = VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH;

	vulkan_texture_create_no_alloc(renderer, &_create_info, texture);
}

RENDERER_API void vulkan_render_texture_destroy(vulkan_render_texture_t* texture)
{
	vulkan_texture_destroy(texture);
}

RENDERER_API void vulkan_render_texture_release_resources(vulkan_render_texture_t* texture)
{
	vulkan_texture_release_resources(texture);
}
