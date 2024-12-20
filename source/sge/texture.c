/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: texture.c is a part of VulkanRenderer

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
#include <sge/texture.h>
#include <sge/internal/vulkan/vulkan_texture.h>
#include <sge/bmp.h>
#include <sge/debug.h>
#include <stdarg.h> 		// va_start, va_end, va_list
#include <stb/stb_image.h>

static vulkan_texture_usage_t get_vulkan_texture_usage(texture_usage_t usage)
{
	switch(usage)
	{
		case TEXTURE_USAGE_NONE:
			return VULKAN_TEXTURE_USAGE_NONE;
		case TEXTURE_USAGE_SAMPLED:
			return VULKAN_TEXTURE_USAGE_SAMPLED;
		case TEXTURE_USAGE_TRANSFER_DST:
			return VULKAN_TEXTURE_USAGE_TRANSFER_DST;
		case TEXTURE_USAGE_TRANSFER_SRC:
			return VULKAN_TEXTURE_USAGE_TRANSFER_SRC;
		case TEXTURE_USAGE_PRESENT:
			return VULKAN_TEXTURE_USAGE_PRESENT;
		case TEXTURE_USAGE_RENDER_TARGET:
			return VULKAN_TEXTURE_USAGE_RENDER_TARGET;
		case TEXTURE_USAGE_ATTACHMENT:
			return VULKAN_TEXTURE_USAGE_ATTACHMENT;
		default:
			debug_log_fetal_error("Invalid texture_usage_t usage: %lu", usage);
	}
	return VULKAN_TEXTURE_USAGE_NONE;
}

SGE_API texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info)
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
		.initial_usage = get_vulkan_texture_usage(create_info->initial_usage),
		.usage = get_vulkan_texture_usage(create_info->usage),
		.final_usage = get_vulkan_texture_usage(create_info->final_usage),
		.technique = VULKAN_RENDER_TARGET_TECHNIQUE_UNDEFINED /* ignored because this texture is not a render texture */
	};

	return vulkan_texture_create(renderer->handle, &vulkan_create_info);
}

SGE_API texture_t* texture_load(renderer_t* renderer, texture_type_t type, ...)
{
	va_list file_paths;
	va_start(file_paths, type);
	texture_t* texture = texture_loadv(renderer, type, file_paths);
	va_end(file_paths);
	return texture;
}

SGE_API texture_t* texture_load_mem(renderer_t* renderer, texture_type_t type, ...)
{
	va_list datas;
	va_start(datas, type);
	texture_t* texture = texture_load_memv(renderer, type, datas);
	va_end(datas);
	return texture;
}

static texture_t* _texture_create(renderer_t* renderer, texture_type_t type, vulkan_texture_data_t* data_ptr, u32 data_count)
{
	// create texture
	texture_create_info_t create_info = 
	{
		.width = data_ptr[0].width,
		.height = data_ptr[0].height,
		.depth = data_ptr[0].depth,
		.channel_count = data_ptr[0].channel_count,
		.type = type,
		.initial_usage = TEXTURE_USAGE_TRANSFER_DST,
		.usage = TEXTURE_USAGE_SAMPLED,
		.final_usage = TEXTURE_USAGE_SAMPLED
	};
	texture_t* texture = texture_create(renderer, &create_info);

	vulkan_texture_upload_data(texture, data_count, data_ptr);
	vulkan_texture_set_usage_stage(texture, VULKAN_TEXTURE_USAGE_STAGE_FINAL);
	return texture;
}

static u32 get_texture_count_from_texture_type(texture_type_t type)
{
	u32 texture_count;
	switch(type)
	{
		case TEXTURE_TYPE_ALBEDO:
		case TEXTURE_TYPE_NORMAL:
		case TEXTURE_TYPE_CUBE_COMBINED:
			texture_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_SEPARATED:
			texture_count = 6;
			break;
		default:
			LOG_FETAL_ERR("Unrecognized texture_type\n");
	};
	return texture_count;
}

SGE_API texture_t* texture_load_pixels(renderer_t* renderer, texture_type_t type, ...)
{
	va_list datas;
	va_start(datas, type);
	texture_t* texture = texture_load_pixelsv(renderer, type, datas);
	va_end(datas);
	return texture;
}

SGE_API texture_t* texture_load_pixelsv(renderer_t* renderer, texture_type_t type, va_list pixelDatas)
{
	AUTO texture_count = get_texture_count_from_texture_type(type);
	// load all the textures from disk to memory
	vulkan_texture_data_t data[texture_count];
	for(u32 i = 0; i < texture_count; i++)
	{
		AUTO pixel_data = va_arg(pixelDatas, const immutable_texture_pixel_data_t*);
		data[i].data = CAST_TO(void*, pixel_data->pixels);
		data[i].width = pixel_data->width;
		data[i].height = pixel_data->height;
		data[i].depth = 1;
		data[i].channel_count = 4;
		com_assert(COM_DESCRIPTION(pixel_data->num_channels == 4), "For now number of channels in a texture must be 4, we will add support for variable number of channels later");
	}

	texture_t* texture = _texture_create(renderer, type, data, texture_count);

	return texture;
}

SGE_API texture_t* texture_loadv(renderer_t* renderer, texture_type_t type, va_list file_paths)
{
	AUTO file_path_count = get_texture_count_from_texture_type(type);

	// load all the textures from disk to memory
	vulkan_texture_data_t data[file_path_count];
	for(u32 i = 0; i < file_path_count; i++)
	{
		const char* file_path = va_arg(file_paths, const char*);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		data[i].data = stbi_load(file_path, &width, &height, &channels, STBI_rgb_alpha);
		if(data[i].data == NULL)
			DEBUG_LOG_FETAL_ERROR("Failed to load texture %s", file_path);
		/* I'm not sure were is the problem, but for some reasons it feels like stb's output format is BGRA instead of RGB.
		 * So, convert the BGRA to RGBA. */
		u8* data_ptr = data[i].data;
		for(int j = 0; j < (width * height * 4); j += 4)
		{
			AUTO t = data_ptr[j + 2];
			data_ptr[j + 2] = data_ptr[j];
			data_ptr[j] = t;
		}
		data[i].width = width;
		data[i].height = height;
		data[i].depth = 1;
		data[i].channel_count = 4;
	}

	texture_t* texture = _texture_create(renderer, type, data, file_path_count);
	
	// unload the loaded texture data from host memory
	for(u32 i = 0; i < file_path_count; i++)
		stbi_image_free(data[i].data);

	return texture;
}

SGE_API texture_t* texture_load_memv(renderer_t* renderer, texture_type_t type, va_list datas)
{
	AUTO tex_count = get_texture_count_from_texture_type(type);

	// load all the textures from disk to memory
	vulkan_texture_data_t data[tex_count];
	for(u32 i = 0; i < tex_count; i++)
	{
		com_immutable_data_t im_data = va_arg(datas, com_immutable_data_t);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		data[i].data = stbi_load_from_memory(im_data.bytes, im_data.size, &width, &height, &channels, STBI_rgb_alpha);
		if(data[i].data == NULL)
			DEBUG_LOG_FETAL_ERROR("Failed to load texture at index %u", i);
		/* I'm not sure were is the problem, but for some reasons it feels like stb's output format is BGRA instead of RGB.
		 * So, convert the BGRA to RGBA. */
		u8* data_ptr = data[i].data;
		for(int j = 0; j < (width * height * 4); j += 4)
		{
			AUTO t = data_ptr[j + 2];
			data_ptr[j + 2] = data_ptr[j];
			data_ptr[j] = t;
		}
		data[i].width = width;
		data[i].height = height;
		data[i].depth = 1;
		data[i].channel_count = 4;
	}

	texture_t* texture = _texture_create(renderer, type, data, tex_count);
	
	// unload the loaded texture data from host memory
	for(u32 i = 0; i < tex_count; i++)
		stbi_image_free(data[i].data);

	return texture;
}

SGE_API void texture_upload_data(texture_t* texture, u32 data_count, texture_data_t* data)
{
	vulkan_texture_data_t vk_data[data_count];
	for(u32 i = 0; i < data_count; i++)
	{
		vk_data[i] = (vulkan_texture_data_t)
		{
			.data = data[i].data,
			.width = data[i].width,
			.height = data[i].height,
			.depth = data[i].depth,
			.channel_count = data[i].channel_count
		};
	}
	vulkan_texture_upload_data(texture, data_count, vk_data);
}

SGE_API void texture_set_usage_stage(texture_t* texture, texture_usage_stage_t stage)
{
	vulkan_texture_usage_stage_t vk_stage = VULKAN_TEXTURE_USAGE_STAGE_UNDEFINED;
	switch(stage)
	{
		case TEXTURE_USAGE_STAGE_INITIAL:
			vk_stage = VULKAN_TEXTURE_USAGE_STAGE_INITIAL;
			break;
		case TEXTURE_USAGE_STAGE_USAGE:
			vk_stage = VULKAN_TEXTURE_USAGE_STAGE_USAGE;
			break;
		case TEXTURE_USAGE_STAGE_FINAL:
			vk_stage = VULKAN_TEXTURE_USAGE_STAGE_FINAL;
			break;
		default:
			debug_log_warning("The texture usage stage seems to be UNDEFINED or Invalid", stage);
			break;
	}
	vulkan_texture_set_usage_stage(texture, vk_stage);
}

SGE_API void texture_destroy(texture_t* texture)
{
	vulkan_texture_destroy(texture);
}

SGE_API void texture_release_resources(texture_t* texture)
{
	vulkan_texture_release_resources(texture);
}


SGE_API const texture_attributes_t* texture_get_attributes(const texture_t* texture)
{
	return vulkan_texture_get_attributes(VULKAN_TEXTURE_CAST_CONST(texture));
}
