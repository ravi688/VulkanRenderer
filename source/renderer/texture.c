#include <renderer/renderer.h>
#include <renderer/texture.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/bmp.h>
#include <renderer/assert.h> 	// LOG_FETAL_ERR
#include <stdarg.h> 		// va_start, va_end, va_list

RENDERER_API texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info)
{
	// determine the data count (texture_count) and the vulkan type
	vulkan_texture_type_t vulkan_type;
	u32 texture_count;
	switch(create_info->type)
	{
		case TEXTURE_TYPE_ALBEDO:
			vulkan_type = VULKAN_TEXTURE_TYPE_ALBEDO;
			texture_count = 1;
			break;
		case TEXTURE_TYPE_NORMAL:
			vulkan_type = VULKAN_TEXTURE_TYPE_NORMAL;
			texture_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_COMBINED:
			vulkan_type = VULKAN_TEXTURE_TYPE_CUBE;
			texture_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_SEPARATED:
			vulkan_type = VULKAN_TEXTURE_TYPE_CUBE;
			texture_count = 6;
			break;
		default:
			LOG_FETAL_ERR("Unrecognized texture_type\n");
	};

	// check if the expected texture count doesn't matched with create_info->data_count
	if(texture_count != create_info->data_count)
		LOG_FETAL_ERR("texture_count != create_info->data_count\n");

	// copy texture_data_t object's data to vulkan_texture_data_t object
	vulkan_texture_data_t data[texture_count];
	for(u32 i = 0; i < texture_count; i++)
	{
		data[i].data = create_info->data[i].data;
		data[i].width = create_info->data[i].width;
		data[i].height = create_info->data[i].height;
		data[i].depth = 1;
		data[i].channel_count = create_info->data[i].channel_count;
	}

	// create vulkan texture
	vulkan_texture_create_info_t vulkan_create_info  =
	{
		.data_count = texture_count,
		.data = &data[0],
		.type = vulkan_type
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
	// determine vulkan texture type and number of textures to load
	vulkan_texture_type_t vulkan_type;
	u32 file_path_count;
	switch(type)
	{
		case TEXTURE_TYPE_ALBEDO:
			vulkan_type = VULKAN_TEXTURE_TYPE_ALBEDO;
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_NORMAL:
			vulkan_type = VULKAN_TEXTURE_TYPE_NORMAL;
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_COMBINED:
			vulkan_type = VULKAN_TEXTURE_TYPE_CUBE;
			file_path_count = 1;
			break;
		case TEXTURE_TYPE_CUBE_SEPARATED:
			vulkan_type = VULKAN_TEXTURE_TYPE_CUBE;
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
		bmp_data[i] = bmp_load(va_arg(file_paths, const char*));
		data[i].data = bmp_data[i].data;
		data[i].width = bmp_data[i].width;
		data[i].height = bmp_data[i].height;
		data[i].depth = 1;
		data[i].channel_count = bmp_data[i].channel_count;
	}

	// create vulkan texture
	vulkan_texture_create_info_t create_info =
	{
		.data_count = file_path_count,
		.data = &data[0],
		.type = vulkan_type
	};
	vulkan_texture_t* texture = vulkan_texture_create(renderer->handle, &create_info);
	
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
