
#pragma once

#include <renderer/defines.h>

#if   defined(RENDERER_VULKAN_DRIVER)
	typedef struct vulkan_bitmap_glyph_atlas_texture_t vulkan_bitmap_glyph_atlas_texture_t;
	typedef vulkan_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_bitmap_glyph_atlas_texture_t opengl_bitmap_glyph_atlas_texture_t;
	typedef opengl_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_bitmap_glyph_atlas_texture_t directx_bitmap_glyph_atlas_texture_t;
	typedef directx_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#elif defined(RENDERER_METAL_DRIVER)
	typedef struct metal_bitmap_glyph_atlas_texture_t metal_bitmap_glyph_atlas_texture_t;
	typedef metal_bitmap_glyph_atlas_texture_t bitmap_glyph_atlas_texture_t;
#endif

#include <renderer/font.h> // font_t and utf32_t
#include <renderer/bitmap_glyph_pool.h> // bitmap_glyph_pool_t and glyph_texcoord_t

typedef struct bitmap_glyph_atlas_texture_create_info_t
{
	/* initial width of the texture */
	u32 width;
	/* initial height of the texture */
	u32 height;
	/* font used to rasterize glyphs */
	font_t* font;
} bitmap_glyph_atlas_texture_create_info_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_new(memory_allocator_t* allocator);
RENDERER_API bitmap_glyph_atlas_texture_t* bitmap_glyph_atlas_texture_create(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info);
RENDERER_API void bitmap_glyph_atlas_texture_create_no_alloc(renderer_t* renderer, bitmap_glyph_atlas_texture_create_info_t* create_info, bitmap_glyph_atlas_texture_t OUT texture);
RENDERER_API void bitmap_glyph_atlas_texture_destroy(bitmap_glyph_atlas_texture_t* texture);
RENDERER_API void bitmap_glyph_atlas_texture_release_resources(bitmap_glyph_atlas_texture_t* texture);

/* returns pointer to the bitmap_glyph_pool_t object */
RENDERER_API bitmap_glyph_pool_t* bitmap_glyph_atlas_texture_get_pool(bitmap_glyph_atlas_texture_t* texture);
/* returns the font used by this BGA texture */
RENDERER_API font_t* bitmap_glyph_atlas_texture_get_font(bitmap_glyph_atlas_texture_t* texture);
/* flushes the host side font bitmap to the GPU side memory (it might also destroy and create VkDeviceMemory) 
 * is_resized: true if internal GPU and host memory has been resized
 * returns: true if internal API objects recreated or GPU side memory has been updated */
RENDERER_API bool bitmap_glyph_atlas_texture_commit(bitmap_glyph_atlas_texture_t* texture, bool OUT is_resized);
/* quality: quality of the rasterized glyph, ranges from 0 to 255 inclusive 
 * unicode: glyph's unicode value to rasterize
 * texcoords: the texture coordinates (list of 4 vec2(s)), filled by this function if the glyph has graphical representation
 * returns: true if the glyph has graphical representation and there are no errors */
RENDERER_API bool bitmap_glyph_atlas_texture_get_texcoord(bitmap_glyph_atlas_texture_t* texture, utf32_t unicode, glyph_texcoord_t OUT texcoord);
#ifdef GLOBAL_DEBUG
RENDERER_API void bitmap_glyph_atlas_texture_dump(bitmap_glyph_atlas_texture_t* texture, const char* file_path);
#endif /* GLOBAL_DEBUG */
END_CPP_COMPATIBLE
