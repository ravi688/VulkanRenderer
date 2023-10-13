#include <renderer/bitmap_text.h>
#include <renderer/internal/vulkan/vulkan_bitmap_text.h>
#include <renderer/memory_allocator.h>
#include <renderer/renderer.h>

RENDERER_API bitmap_text_t* bitmap_text_new(memory_allocator_t* allocator)
{
	return vulkan_bitmap_text_new(allocator);
}

RENDERER_API bitmap_text_t* bitmap_text_create(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture)
{
	vulkan_bitmap_text_create_info_t vk_create_info = { .texture = texture };
	return vulkan_bitmap_text_create(VULKAN_RENDERER(renderer), &vk_create_info);
}

RENDERER_API void bitmap_text_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture, bitmap_text_t OUT text)
{
	vulkan_bitmap_text_create_info_t vk_create_info = { .texture = texture };
	vulkan_bitmap_text_create_no_alloc(VULKAN_RENDERER(renderer), &vk_create_info, text);
}

RENDERER_API void bitmap_text_destroy(bitmap_text_t* text)
{
	vulkan_bitmap_text_destroy(text);
}

RENDERER_API void bitmap_text_release_resources(bitmap_text_t* text)
{
	vulkan_bitmap_text_release_resources(text);
}

RENDERER_API void bitmap_text_draw(bitmap_text_t* text)
{
	vulkan_bitmap_text_draw(text);
}

RENDERER_API bitmap_text_string_handle_t bitmap_text_string_create(bitmap_text_t* text)
{
	return vulkan_bitmap_text_string_create(text);
}

RENDERER_API void bitmap_text_string_destroyH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	vulkan_bitmap_text_string_destroyH(text, handle);
}

/* setters */
RENDERER_API void bitmap_text_set_material(bitmap_text_t* text, material_t* material)
{
	vulkan_bitmap_text_set_material(text, material);
}

RENDERER_API void bitmap_text_set_render_space_type(bitmap_text_t* text, bitmap_text_render_space_type_t space_type)
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

RENDERER_API void bitmap_text_set_render_surface_type(bitmap_text_t* text, bitmap_text_render_surface_type_t surface_type)
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

RENDERER_API void bitmap_text_string_setH(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char* string)
{
	vulkan_bitmap_text_string_setH(text, handle, string);
}

RENDERER_API void bitmap_text_string_set_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle, mat4_t transform)
{
	vulkan_bitmap_text_string_set_transformH(text, handle, transform);
}

/* getters */
RENDERER_API const char* bitmap_text_string_getH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_getH(text, handle);
}

RENDERER_API mat4_t bitmap_text_string_get_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_transformH(text, handle);
}
