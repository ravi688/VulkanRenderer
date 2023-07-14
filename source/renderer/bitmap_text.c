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

RENDERER_API bitmap_text_string_handle_t bitmap_text_string_create(bitmap_text_t* text)
{
	return vulkan_bitmap_text_string_create(text);
}

RENDERER_API void bitmap_text_string_destroyH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	vulkan_bitmap_text_string_destroyH(text, handle);
}

/* setters */
RENDERER_API void bitmap_text_string_setH(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char* string)
{
	vulkan_bitmap_text_string_setH(text, handle, string);
}

RENDERER_API void bitmap_text_string_set_scaleH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t scale)
{
	vulkan_bitmap_text_string_set_scaleH(text, handle, scale);
}

RENDERER_API void bitmap_text_string_set_positionH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t position)
{
	vulkan_bitmap_text_string_set_positionH(text, handle, position);
}

RENDERER_API void bitmap_text_string_set_rotationH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t rotation)
{
	vulkan_bitmap_text_string_set_rotationH(text, handle, rotation);
}

RENDERER_API void bitmap_text_string_set_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle, mat4_t transform)
{
	vulkan_bitmap_text_string_set_transformH(text, handle, transform);
}

/* setters */
RENDERER_API const char* bitmap_text_string_getH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_getH(text, handle);
}

RENDERER_API vec3_t bitmap_text_string_get_scaleH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_scaleH(text, handle);
}

RENDERER_API vec3_t bitmap_text_string_get_positionH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_positionH(text, handle);
}

RENDERER_API vec3_t bitmap_text_string_get_rotationH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_rotationH(text, handle);
}

RENDERER_API mat4_t bitmap_text_string_get_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle)
{
	return vulkan_bitmap_text_string_get_transformH(text, handle);
}
