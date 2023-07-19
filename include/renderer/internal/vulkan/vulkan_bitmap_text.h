#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_instance_buffer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/dictionary.h>
#include <renderer/event.h> // event_subscription_handle_t
#include <renderer/rect.h> // rect2d_t
#include <hpml/mat4.h> 	// mat4_t
#include <hpml/vec3.h> 	// vec3_t
#include <glslcommon/glsl_types.h>


/* element of Glyph Texture Coordinate (GTC) buffer */
typedef struct glsl_glyph_texcoord_t
{
	/* per_instance [BTM_TXT_TLTC_BND, BTM_TXT_TLTC_LOC, BTM_TXT_TLTC_COMP] in vec2 tltc; */
	glsl_vec2_t tltc;
	/* per_instance [BTM_TXT_TRTC_BND, BTM_TXT_TRTC_LOC, BTM_TXT_TRTC_COMP] in vec2 trtc; */
	glsl_vec2_t trtc;
	/* per_instance [BTM_TXT_BRTC_BND, BTM_TXT_BRTC_LOC, BTM_TXT_BRTC_COMP] in vec2 brtc; */
	glsl_vec2_t brtc;
	/* per_instance [BTM_TXT_BLTC_BND, BTM_TXT_BLTC_LOC, BTM_TXT_BLTC_COMP] in vec2 bltc; */
	glsl_vec2_t bltc;
} glsl_glyph_texcoord_t ALIGN_AS(GLSL_TYPE_VEC4_ALIGN);

#define __glsl_sizeof_glsl_glyph_texcoord_t 		(4 * __glsl_sizeof_glsl_vec2_t)

/* element of Glyph Render Data (GRD) buffer */
typedef struct glsl_glyph_render_data_t
{
	/* offset of this glyph instance from the very first glyph instance in the string */
	/* per_instance [BTM_TXT_OFST_BND, BTM_TXT_OFST_LOC, BTM_TXT_OFST_COMP] in vec3 ofst; */
	glsl_vec3_t ofst;
	/* index of the texture coordinates of this glyph instance into the GTC buffer */
	/* per_instance [BTM_TXT_INDX_BND, BTM_TXT_INDX_LOC, BTM_TXT_INDX_COMP] in uint indx; */
	glsl_uint_t indx;
	/* rotation of this glyph instance */
	/* per_instance [BTM_TXT_ROTN_BND, BTM_TXT_ROTN_LOC, BTM_TXT_ROTN_COMP] in vec3 rotn; */
	glsl_vec3_t rotn;
	/* index of the string's transform to which this glyph instance belongs into the String Transform Buffer (ST Buffer) */
	/* per_instance [BTM_TXT_STID_BND, BTM_TXT_STID_LOC, BTM_TXT_STID_COMP] in uint stid; */
	glsl_uint_t stid;
	/* scale of this glyph instance 
	 * per_instance [BTM_TXT_SCAL_BND, BTM_TXT_SCAL_LOC, BTM_TXT_SCAL_COMP] in vec3 scal; */
	glsl_vec3_t scal;
} glsl_glyph_render_data_t ALIGN_AS(GLSL_TYPE_VEC4_ALIGN);

#define __glsl_sizeof_glsl_glyph_render_data_t 		(3 * __glsl_sizeof_glsl_vec4_t)

/* element of Text String Transform (TST) buffer */
typedef glsl_mat4_t text_string_transform_t;

/* character buffer to store the characters in a string */
typedef buffer_t char_buffer_t;
typedef buf_ucount_t vulkan_bitmap_text_string_handle_t;
typedef dictionary_t vulkan_bitmap_glyph_sub_buffer_handle_table_t;

typedef struct vulkan_bitmap_text_string_t
{
	/* handle to the next free string if this string is in the free list or inuse string if this is in the inuse list */
	vulkan_bitmap_text_string_handle_t next;
	sub_buffer_handle_t render_data_handle;
	/* string */
	char_buffer_t chars;
	/* a rectangle in a 3D space
	 * holds the position and the extents of the text */
	struct { offset3d_t offset; extent2d_t extent; } rect;
	/* transform matrix applied to this string */
	mat4_t transform;
	/* id assigned to this string */
	u32 index;
} vulkan_bitmap_text_string_t;

typedef struct vulkan_bitmap_glyph_atlas_texture_t vulkan_bitmap_glyph_atlas_texture_t;

typedef struct vulkan_bitmap_text_create_info_t
{
	vulkan_bitmap_glyph_atlas_texture_t* texture;
} vulkan_bitmap_text_create_info_t;

typedef vulkan_instance_buffer_t vulkan_host_multibuffered_buffer_t;

/* Glyph Texture Coordinate (GTC) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_glyph_texcoord_buffer_t;
/* Glyph Render Data (GRD) buffer */
typedef vulkan_host_multibuffered_buffer_t vulkan_bitmap_glyph_render_data_buffer_t;
/* Text String Transform (TST) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_text_string_transform_buffer_t;

typedef buffer_t vulkan_bitmap_text_string_list_t;

typedef dictionary_t glyph_texcoord_index_table_t;

typedef struct vulkan_material_t vulkan_material_t;

typedef struct vulkan_bitmap_text_t
{
	vulkan_renderer_t* renderer;
	vulkan_material_t* material;

	/* CPU side */

	/* strings derived from this bitmap text */
	vulkan_bitmap_text_string_list_t text_strings;
	/* a valid index pointing in the 'strings', otherwise BUF_INVALID_INDEX */
	vulkan_bitmap_text_string_handle_t free_list;
	/* a valid index pointing in the 'strings', otherwise BUF_INVALID_INDEX */
	vulkan_bitmap_text_string_handle_t inuse_list;

	/* GPU side */

	union 
	{ 
		vulkan_bitmap_glyph_atlas_texture_t* glyph_atlas_texture;
		vulkan_bitmap_glyph_atlas_texture_t* texture;
	};
	/* GRD buffers mapped to each unique glyph across all the text strings */
	vulkan_bitmap_glyph_render_data_buffer_t glyph_render_data_buffer;
	/* GTC buffer */
	vulkan_bitmap_glyph_texcoord_buffer_t glyph_texcoord_buffer;
	/* TST buffer */
	vulkan_bitmap_text_string_transform_buffer_t text_string_transform_buffer;

	glyph_texcoord_index_table_t glyph_texcoord_index_table;

	/* instances of this quad will be drawn to lay out the glyph bitmaps */
	vulkan_mesh_t quad_mesh;

	event_subscription_handle_t bga_texture_update_handle;

} vulkan_bitmap_text_t;

#define VULKAN_BITMAP_TEXT(ptr) CAST_TO(vulkan_bitmap_text_t*, ptr)


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator);
RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info);
RENDERER_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info, vulkan_bitmap_text_t OUT text);
RENDERER_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text);
RENDERER_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text);

RENDERER_API void vulkan_bitmap_text_draw(vulkan_bitmap_text_t* text);
RENDERER_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material);

RENDERER_API vulkan_bitmap_text_string_handle_t vulkan_bitmap_text_string_create(vulkan_bitmap_text_t* text);
RENDERER_API void vulkan_bitmap_text_string_destroyH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);

/* setters */
RENDERER_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, const char* string);
RENDERER_API void vulkan_bitmap_text_string_set_scaleH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, vec3_t scale);
RENDERER_API void vulkan_bitmap_text_string_set_positionH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, vec3_t position);
RENDERER_API void vulkan_bitmap_text_string_set_rotationH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, vec3_t rotation);
RENDERER_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, mat4_t transform);

/* setters */
RENDERER_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API vec3_t vulkan_bitmap_text_string_get_scaleH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API vec3_t vulkan_bitmap_text_string_get_positionH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API vec3_t vulkan_bitmap_text_string_get_rotationH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);


END_CPP_COMPATIBLE
