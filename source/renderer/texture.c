
#include <renderer/texture.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/bmp.h>

texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info)
{
	vulkan_texture_create_info_t vulkan_create_info  =
	{
		.data = create_info->data,
		.width = create_info->width,
		.height = create_info->height,
		.channel_count = create_info->channel_count
	};
	return vulkan_texture_create(renderer, &vulkan_create_info);
}

texture_t* texture_load(renderer_t* renderer, const char* file_path)
{
	bmp_t image = bmp_load(file_path);
	vulkan_texture_create_info_t create_info =
	{
		.data = image.data,
		.width = image.width,
		.height = image.height,
		.channel_count = image.channel_count
	};
	vulkan_texture_t* texture = vulkan_texture_create(renderer, &create_info);
	bmp_destroy(image);
	return texture;
}

void texture_destroy(texture_t* texture, renderer_t* renderer)
{
	vulkan_texture_destroy(texture, renderer);
}

void texture_release_resources(texture_t* texture)
{
	vulkan_texture_release_resources(texture);
}
