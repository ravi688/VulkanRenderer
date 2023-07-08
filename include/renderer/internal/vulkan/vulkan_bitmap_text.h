#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_instance_buffer.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/dictionary.h>
#include <renderer/rect.h> // rect2d_t
#include <hpml/vec3.h>
#include <hpml/mat4.h>
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

#define __glsl_sizeof_glsl_glyph_texcoord_t 		(4 * glsl_sizeof(glsl_vec2_t))

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
} glsl_glyph_render_data_t ALIGN_AS(GLSL_TYPE_VEC4_ALIGN);

#define __glsl_sizeof_glsl_glyph_render_data_t 		(2 * __glsl_sizeof_glsl_vec4_t)

/* element of Text String Transform (TST) buffer */
typedef glsl_mat4_t text_string_transform_t;

/* character buffer to store the characters in a string */
typedef BUFFER char_buffer_t;
typedef buf_ucount_t vulkan_bitmap_text_string_handle_t;
typedef dictionary_t vulkan_bitmap_glyph_sub_buffer_handle_map_t;

typedef struct vulkan_bitmap_text_string_t
{
	/* handle to the next free string if this string is in the free list or inuse string if this is in the inuse list */
	vulkan_bitmap_text_string_handle_t next;
	/* holds list of subbuffers mapped to each elements in the set 'Union over the character array of this string', i.e. unique characters */
	vulkan_bitmap_glyph_sub_buffer_handle_map_t glyph_sub_buffer_handles;
	/* string */
	char_buffer_t chars;
	/* a rectangle in a 3D space
	 * holds the position and the extents of the text */
	rect2d_t rect;
	/* transform matrix applied to this string */
	mat4_t transform;
} vulkan_bitmap_text_string_t;

typedef vulkan_instance_buffer_t vulkan_host_multibuffered_buffer_t;

/* Glyph Texture Coordinate (GTC) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_glyph_texcoord_buffer_t;
/* Glyph Render Data (GRD) buffer */
typedef vulkan_host_multibuffered_buffer_t  vulkan_bitmap_glyph_render_data_buffer_t;
/* Text String Transform (TST) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_text_string_transform_buffer_t;

typedef struct vulkan_bitmap_glyph_atlas_texture_t vulkan_bitmap_glyph_atlas_texture_t;

typedef BUFFER vulkan_bitmap_text_string_list_t;
/* u16:glyph, vulkan_bitmap_glyph_render_data_buffer_t: render data */
typedef dictionary_t vulkan_bitmap_glyph_render_data_map_t;

typedef struct vulkan_bitmap_text_t
{
	vulkan_renderer_t* renderer;

	/* CPU side */

	/* strings derived from this bitmap text */
	vulkan_bitmap_text_string_list_t text_strings;
	/* a valid index pointing in the 'strings', otherwise BUF_INVALID_INDEX */
	vulkan_bitmap_text_string_handle_t free_list;
	/* a valid index pointing in the 'strings', otherwise BUF_INVALID_INDEX */
	vulkan_bitmap_text_string_handle_t inuse_list;

	/* GPU side */

	vulkan_bitmap_glyph_atlas_texture_t* glyph_atlas_texture;
	/* GRD buffers mapped to each unique glyph across all the text strings */
	vulkan_bitmap_glyph_render_data_map_t glyph_render_data_buffers;
	/* TST buffer */
	vulkan_bitmap_text_string_transform_buffer_t text_string_transform_buffer;
	/* GTC buffer */
	vulkan_bitmap_glyph_texcoord_buffer_t glyph_texcoord_buffer;
} vulkan_bitmap_text_t;


BEGIN_CPP_COMPATIBLE

RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator);
RENDERER_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_t* bga_texture);
RENDERER_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_glyph_atlas_texture_t* bga_texture, vulkan_bitmap_text_t OUT text);
RENDERER_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text);
RENDERER_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text);

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
