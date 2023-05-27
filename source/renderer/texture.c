/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: texture.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

#include <renderer/renderer.h>
#include <renderer/texture.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/bmp.h>
#include <renderer/debug.h>
#include <stdarg.h> 		// va_start, va_end, va_list

RENDERER_API texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info)
{
	vulkan_texture_type_t vulkan_type;
	switch(create_info->type)
	{
		case TEXTURE_TYPE_ALBEDO:
			vulkan_type = VULKAN_TEXTURE_TYPE_ALBEDO;
			break;
		case TEXTURE_TYPE_NORMAL:
			vulkan_type = VULKAN_TEXTURE_TYPE_NORMAL;
			break;
		case TEXTURE_TYPE_CUBE_COMBINED:
			vulkan_type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE;
			break;
		case TEXTURE_TYPE_CUBE_SEPARATED:
			vulkan_type = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE;
			break;
		default:
			LOG_FETAL_ERR("Unrecognized texture_type\n");
	};

	// create vulkan texture
	vulkan_texture_create_info_t vulkan_create_info  =
	{
		.width = create_info->width, 
		.height = create_info->height,
		.depth = create_info->depth, 
		.channel_count = create_info->channel_count, 
		.type = vulkan_type,
		.initial_usage = VULKAN_TEXTURE_USAGE_TRANSFER_DST,
		.usage = VULKAN_TEXTURE_USAGE_NONE,
		.final_usage = VULKAN_TEXTURE_USAGE_SAMPLED
	};

	return vulkan_texture_create(renderer->handle, &vulkan_create_info);
}

RENDERER_API texture_t* texture_load(renderer_t* renderer, texture_type_t type, ...)
{
	va_list file_paths;
	va_start(file_paths, type);
	texture_t* texture = texture_loadv(renderer, type, file_paths);
	va_end(file_paths);
	return texture;
}

RENDERER_API texture_t* texture_loadv(renderer_t* renderer, texture_type_t type, va_list file_paths)
{
	u32 file_path_count;
	switch(type)
	{
		case TEXTURE_TYPE_ALBEDO:
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_NORMAL:
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_COMBINED:
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_SEPARATED:
			file_path_count = 6;
			break;
		default:
			LOG_FETAL_ERR("Unrecognized texture_type\n");
	};

	// load all the textures from disk to memory
	vulkan_texture_data_t data[file_path_count];
	bmp_t bmp_data[file_path_count];
	for(u32 i = 0; i < file_path_count; i++)
	{
		bmp_data[i] = bmp_load(renderer->allocator, va_arg(file_paths, const char*));
		data[i].data = bmp_data[i].data;
		data[i].width = bmp_data[i].width;
		data[i].height = bmp_data[i].height;
		data[i].depth = 1;
		data[i].channel_count = bmp_data[i].channel_count;
	}
	
	// create texture
	texture_create_info_t create_info = 
	{
		.width = data[0].width,
		.height = data[0].height,
		.depth = data[0].depth,
		.channel_count = data[0].channel_count,
		.type = type
	};
	texture_t* texture = texture_create(renderer, &create_info);

	vulkan_texture_upload_data(texture, file_path_count, data);
	vulkan_texture_set_usage_stage(texture, VULKAN_TEXTURE_USAGE_STAGE_FINAL);
	
	// unload the loaded texture data from host memory
	for(u32 i = 0; i < file_path_count; i++)
		bmp_destroy(bmp_data[i]);

	return texture;
}

RENDERER_API void texture_destroy(texture_t* texture)
{
	vulkan_texture_destroy(texture);
}

RENDERER_API void texture_release_resources(texture_t* texture)
{
	vulkan_texture_release_resources(texture);
}
