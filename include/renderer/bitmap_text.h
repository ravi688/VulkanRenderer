#pragma once

#include <renderer/defines.h>

#if   defined(RENDERER_VULKAN_DRIVER)
	#include <renderer/internal/vulkan/vulkan_bitmap_text.h>
	typedef vulkan_bitmap_text_t bitmap_text_t;
	typedef vulkan_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef vulkan_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	#include <renderer/internal/opengl/opengl_bitmap_text.h>
	typedef opengl_bitmap_text_t bitmap_text_t;
	typedef opengl_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef opengl_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	#include <renderer/internal/directx/directx_bitmap_text.h>
	typedef directx_bitmap_text_t bitmap_text_t;
	typedef directx_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef directx_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_METAL_DRIVER)
	#include <renderer/internal/metal/metal_bitmap_text.h>
	typedef metal_bitmap_text_t bitmap_text_t;
	typedef metal_bitmap_text_string_handle_t bitmap_text_string_handle_t;
	typedef metal_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#endif

#include <renderer/rect.h> // rect2d_t
#include <hpml/mat4.h> 	// mat4_t
#include <hpml/vec3.h> 	// vec3_t

BEGIN_CPP_COMPATIBLE

RENDERER_API bitmap_text_t* bitmap_text_new(memory_allocator_t* allocator);
RENDERER_API bitmap_text_t* bitmap_text_create(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture);
RENDERER_API void bitmap_text_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_t* texture, bitmap_text_t OUT text);
RENDERER_API void bitmap_text_destroy(bitmap_text_t* text);
RENDERER_API void bitmap_text_release_resources(bitmap_text_t* text);

RENDERER_API bitmap_text_string_handle_t bitmap_text_string_create(bitmap_text_t* text);
RENDERER_API void bitmap_text_string_destroyH(bitmap_text_t* text, bitmap_text_string_handle_t handle);

/* setters */
RENDERER_API void bitmap_text_string_setH(bitmap_text_t* text, bitmap_text_string_handle_t handle, const char* string);
RENDERER_API void bitmap_text_string_set_scaleH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t scale);
RENDERER_API void bitmap_text_string_set_positionH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t position);
RENDERER_API void bitmap_text_string_set_rotationH(bitmap_text_t* text, bitmap_text_string_handle_t handle, vec3_t rotation);
RENDERER_API void bitmap_text_string_set_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle, mat4_t transform);

/* setters */
RENDERER_API const char* bitmap_text_string_getH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
RENDERER_API vec3_t bitmap_text_string_get_scaleH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
RENDERER_API vec3_t bitmap_text_string_get_positionH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
RENDERER_API vec3_t bitmap_text_string_get_rotationH(bitmap_text_t* text, bitmap_text_string_handle_t handle);
RENDERER_API mat4_t bitmap_text_string_get_transformH(bitmap_text_t* text, bitmap_text_string_handle_t handle);

END_CPP_COMPATIBLE
