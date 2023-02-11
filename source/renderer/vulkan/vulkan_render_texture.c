#include <renderer/internal/vulkan/vulkan_render_texture.h>
#include <renderer/internal/vulkan/vulkan_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

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
