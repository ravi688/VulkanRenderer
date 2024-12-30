/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_bitmap_text.h is a part of VulkanRenderer

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

#pragma once

#include <sge/defines.h>
#include <bufferlib/buffer.h>
#include <sge/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <sge/internal/vulkan/vulkan_instance_buffer.h>
#include <sge/internal/vulkan/vulkan_mesh.h>
#include <sge/internal/vulkan/vulkan_text_types.h>
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/dictionary.h>
#include <sge/types.h>
#include <sge/event.h> // event_subscription_handle_t
#include <sge/rect.h> // rect2d_t
#include <hpml/mat4.h> 	// mat4_t
#include <hpml/vec3.h> /* vec3_t */
#include <glslcommon/glsl_types.h>

typedef enum vulkan_bitmap_text_render_space_type_t
{
	/* selects the 'screen projection matrix' from the camera's projection matrices */
	VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_2D,
	/* selects the 'orthographic/perspective matrix' from the camera's projection matrices */
	VULKAN_BITMAP_TEXT_RENDER_SPACE_TYPE_3D
} vulkan_bitmap_text_render_space_type_t;

typedef enum vulkan_bitmap_text_render_surface_type_t
{
	/* the text will be rendered on to the camera's view */
	VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_CAMERA,
	/* the text will be rendered on to the screen (i.e. will not be affected by the any camera's transformations or projections) */
	VULKAN_BITMAP_TEXT_RENDER_SURFACE_TYPE_SCREEN
} vulkan_bitmap_text_render_surface_type_t;

/* element of Glyph Render Data (GRD) buffer */
typedef struct vulkan_bitmap_text_glsl_glyph_render_data_t
{
	/* offset of this glyph instance from the very first glyph instance in the string */
	/* per_instance [BTM_TXT_OFST_BND, BTM_TXT_OFST_LOC, BTM_TXT_OFST_COMP] in vec3 ofst; */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t ofst;
	/* index of the texture coordinates of this glyph instance into the GTC buffer */
	/* per_instance [BTM_TXT_INDX_BND, BTM_TXT_INDX_LOC, BTM_TXT_INDX_COMP] in uint indx; */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 indx;
	/* rotation of this glyph instance */
	/* per_instance [BTM_TXT_ROTN_BND, BTM_TXT_ROTN_LOC, BTM_TXT_ROTN_COMP] in vec3 rotn; */
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t rotn;
	/* index of the string's transform to which this glyph instance belongs into the String Transform Buffer (ST Buffer) */
	/* per_instance [BTM_TXT_STID_BND, BTM_TXT_STID_LOC, BTM_TXT_STID_COMP] in uint stid; */
	ALIGN_AS(GLSL_STD140_UINT_ALIGN) u32 stid;
	/* scale of this glyph instance
	 * per_instance [BTM_TXT_SCAL_BND, BTM_TXT_SCAL_LOC, BTM_TXT_SCAL_COMP] in vec3 scal; */
	ALIGN_AS(GLSL_STD140_VEC4_ALIGN) vec4_t colr;
} vulkan_bitmap_text_glsl_glyph_render_data_t ALIGN_AS(U32_MAX_OF(GLSL_STD140_UINT_ALIGN, GLSL_STD140_VEC4_ALIGN));

#define SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T (16 * 3)
#define STRIDE_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T, ALIGN_OF(vulkan_bitmap_text_glsl_glyph_render_data_t))

/* character buffer to store the characters in a string */
typedef buffer_t vulkan_bitmap_text_char_buffer_t;
typedef buffer_t /* element_type: f32 */ f32_buffer_t;
typedef buffer_t /* element_type: u32 */ u32_buffer_t;
typedef buf_ucount_t vulkan_bitmap_text_string_handle_t;
#define VULKAN_BITMAP_TEXT_STRING_HANDLE_INVALID BUF_INVALID_INDEX
typedef dictionary_t vulkan_bitmap_glyph_sub_buffer_handle_table_t;

/* the range also takes whitespaces into account */
typedef struct char_attr_color_range_t
{
	/* inclusive */
	u32 begin;
	/* exclusive */
	u32 end;
	/* 32-bit x 4 color */
	color_t color;
} char_attr_color_range_t;

typedef buffer_t /* element_type: char_attr_color_range_t */ char_attr_color_range_buffer_t;
typedef buffer_t /* element_type: vulkan_bitmap_text_glsl_glyph_render_data_t */ vulkan_bitmap_glyph_render_data_stage_buffer_t;
typedef struct font_t font_t;

typedef struct vulkan_bitmap_text_string_t
{
	/* handle to the next free string if this string is in the free list or inuse string if this is in the inuse list */
	vulkan_bitmap_text_string_handle_t next;
	/* handle to this string */
	vulkan_bitmap_text_string_handle_t handle;
	sub_buffer_handle_t render_data_handle;
	/* list of offsets, with reference to the left side of very first glyph in the string, along x-axis for each glyph in the chronological order */
	f32_buffer_t glyph_offsets;
	/* list of index mappings, since 'chars' includes whitespaces but grd buffer not,
	 * we need this mapping table to index into the grd buffer if we only have indices of 'chars'. */
	u32_buffer_t index_mappings;
	/* string */
	vulkan_bitmap_text_char_buffer_t chars;
	/* populated when vulkan_bitmap_text_string_set_char_attr_color is called,
	 * and used for applying the color ranges again whenever set_point_size is called. */
	char_attr_color_range_buffer_t color_ranges;
	/* populated when text_string_set is called which is also called by vulkan_bitmap_text_string_setH() and vulkan_bitmap_text_string_set_point_sizeH() or other such functions
	 * basically any function which would cause bitmap text's grd buffer (common to all text strings) to be modified.
	 * This stage buffer is used to preserve the grd values so that the bitmap text's grd buffer could be patched again when this text string gets active again.
	 * When this text string is set inactive, then the glyph render data(s) associated with this text string are removed from the bitmap text's grd buffer,
	 * while grd_stage_buffer still preserves the grd values to patch the bitmap text's grd buffer again.  */
	vulkan_bitmap_glyph_render_data_stage_buffer_t grd_stage_buffer;
	/* a rectangle in a 3D space
	 * holds the position and the extents of the text */
	struct { offset3d_t offset; extent2d_t extent; } rect;
	/* transform matrix applied to this string */
	mat4_t transform;
	/* point size for this text string at which the glyphs will be rasterized.
	 * This 'point_size' value will be affected by vulkan_bitmap_text_set_point_size and
	 * vulkan_bitmap_text_string_set_point_sizeH. */
	u32 point_size;
	/* default color used for shading the glyphs */
	vec4_t color;
	/* holds true, if this string is visible (active), otherwise false. 
	 * inactive means, the characters to render associated with this string will not be part of the GRD buffer. */
	bool is_active;
	/* font used to rasterize the characters in this text string 
	 *
	 * MEMORY OPT: We can also book-keep the pointer to font_t objects in vulkan_bitmap_text_t object itself as an array of pointers, 
	 * and only store index to a font pointer here. Since, usually we don't have much number of unique fonts, we can keep the integer only 8-bit
	 * or 16-bit wide. Thus, saving 7 or 6 bytes of memory per text string instance as compared to the otherwise case. */
	font_t* font;
} vulkan_bitmap_text_string_t;

typedef struct vulkan_bitmap_glyph_atlas_texture_t vulkan_bitmap_glyph_atlas_texture_t;

typedef struct vulkan_bitmap_text_create_info_t
{
	vulkan_bitmap_glyph_atlas_texture_t* texture;
	/* default font, which will be used by all of the text strings created from this bitmap text object. 
	* if provided NULL, then bitmap text will try to use the font used by bga texture if it has a valid pointer to font_t object.
	* if none of contains a valid pointer to font_t object, a warning will be issued. */
	font_t* font;
} vulkan_bitmap_text_create_info_t;

typedef vulkan_instance_buffer_t vulkan_host_multibuffered_buffer_t;

/* Glyph Render Data (GRD) buffer */
typedef vulkan_host_multibuffered_buffer_t vulkan_bitmap_glyph_render_data_buffer_t;
/* Text String Transform (TST) buffer */
typedef vulkan_host_buffered_buffer_t vulkan_bitmap_text_string_transform_buffer_t;

typedef buffer_t vulkan_bitmap_text_string_list_t;

typedef struct vulkan_material_t vulkan_material_t;

typedef struct font_t font_t;

/* <begin> typedefs facilitating text layout */

typedef vulkan_text_glyph_layout_data_t vulkan_bitmap_text_glyph_layout_data_t;

typedef vulkan_text_glyph_layout_data_buffer_t vulkan_bitmap_text_glyph_layout_data_buffer_t;

typedef vulkan_text_glyph_info_t vulkan_bitmap_text_glyph_info_t;

typedef vulkan_text_glyph_layout_handler_input_data_t vulkan_bitmap_text_glyph_layout_handler_input_data_t;

typedef vulkan_text_glyph_layout_handler_t vulkan_bitmap_text_glyph_layout_handler_t;

typedef vulkan_text_glyph_strikethrough_handler_t vulkan_bitmap_text_glyph_strikethrough_handler_t;

typedef vulkan_text_glyph_strikethrough_handler_t vulkan_bitmap_text_glyph_strikethrough_handler_t;

typedef vulkan_text_glyph_underline_handler_t vulkan_bitmap_text_glyph_underline_handler_t;

typedef vulkan_text_glyph_layout_handler_void_ptr_pair_t vulkan_bitmap_text_glyph_layout_handler_void_ptr_pair_t;
typedef vulkan_text_glyph_strikethrough_handler_void_ptr_pair_t vulkan_bitmap_text_glyph_strikethrough_handler_void_ptr_pair_t;
typedef vulkan_text_glyph_underline_handler_void_ptr_pair_t vulkan_bitmap_text_glyph_underline_handler_void_ptr_pair_t;

/* <end> typedefs facilitating text layout */

typedef struct vulkan_bitmap_text_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_material_t* material;

	/* point size for this bitmap text, if a vulkan_bitmap_text_set_point_size is called then it will
	 * affect all the text strings.
	 * But if the client is calling vulkan_bitmap_text_string_set_point_sizeH with point size different
	 * from this 'point size' then it will have value F32_MAX - which would indicate an invalid value. */
	u32 point_size;

	/* default font used by all the text string instances derived/inherited from this bitmap text object. 
	 * that means, calling vulkan_bitmap_text_string_create() would create a text string with its 'font' field initialized with this default font. */
	font_t* font;

	/* render space type of this text */
	vulkan_bitmap_text_render_space_type_t render_space_type;
	/* render surface type of this text */
	vulkan_bitmap_text_render_surface_type_t render_surface_type;

	/* <begin> fields facilitating text layout */

		/* called whenever vulkan_bitmap_text_string_setH is called */
		vulkan_bitmap_text_glyph_layout_handler_void_ptr_pair_t glyph_layout_handler;
		/* called whenever vulkan_bitmap_text_glyph_layout_data_t::is_strikethrough is true */
		vulkan_bitmap_text_glyph_strikethrough_handler_void_ptr_pair_t glyph_strikethrough_handler;
		/* called whenever vulkan_bitmap_text_glyph_layout_data_t::is_underline is true */
		vulkan_bitmap_text_glyph_underline_handler_void_ptr_pair_t glyph_underline_handler;

		/* holds post processed info for each glyph */
		vulkan_bitmap_text_glyph_layout_data_buffer_t glyph_layout_data_buffer;

	/* <end> fields facilitating text layout */

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
	/* TST buffer */
	vulkan_bitmap_text_string_transform_buffer_t text_string_transform_buffer;

	/* instances of this quad will be drawn to lay out the glyph bitmaps */
	vulkan_mesh_t quad_mesh;

	event_subscription_handle_t bga_texture_update_handle;

} vulkan_bitmap_text_t;

#define VULKAN_BITMAP_TEXT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)
#define VULKAN_BITMAP_TEXT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)
#define VULKAN_BITMAP_TEXT_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)
#define VULKAN_BITMAP_TEXT_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)

BEGIN_CPP_COMPATIBLE

SGE_API vulkan_bitmap_text_t* vulkan_bitmap_text_new(memory_allocator_t* allocator);
SGE_API vulkan_bitmap_text_t* vulkan_bitmap_text_create(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info);
SGE_API void vulkan_bitmap_text_create_no_alloc(vulkan_renderer_t* renderer, vulkan_bitmap_text_create_info_t* create_info, vulkan_bitmap_text_t OUT text);
SGE_API void vulkan_bitmap_text_destroy(vulkan_bitmap_text_t* text);
SGE_API void vulkan_bitmap_text_release_resources(vulkan_bitmap_text_t* text);

SGE_API void vulkan_bitmap_text_draw(vulkan_bitmap_text_t* text);
SGE_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material);

SGE_API vulkan_bitmap_text_string_handle_t vulkan_bitmap_text_string_create(vulkan_bitmap_text_t* text);
SGE_API void vulkan_bitmap_text_string_destroyH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);

/* setters */
SGE_API void vulkan_bitmap_text_set_glyph_layout_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_layout_handler_t handler, void* user_data);
SGE_API void vulkan_bitmap_text_set_glyph_strikethrough_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_strikethrough_handler_t handler, void* user_data);
SGE_API void vulkan_bitmap_text_set_glyph_underline_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_underline_handler_t handler, void* user_data);
SGE_API void vulkan_bitmap_text_set_point_size(vulkan_bitmap_text_t* text, u32 point_size);
SGE_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material);
SGE_API void vulkan_bitmap_text_set_render_space_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_space_type_t space_type);
SGE_API void vulkan_bitmap_text_set_render_surface_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_surface_type_t surface_type);
/* TODO
 * vulkan_bitmap_text_string_appendH()
 * vulkan_bitmap_text_string_insertH()
 * vulkan_bitmap_text_string_removeH()
 */
/* NOTE: it is not threadsafe */
SGE_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, const char* string);
SGE_API void vulkan_bitmap_text_string_set_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 point_size);
SGE_API void vulkan_bitmap_text_string_set_fontH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, font_t* font);
SGE_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, mat4_t transform);
SGE_API	void vulkan_bitmap_text_string_set_activeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, bool is_active);
SGE_API void vulkan_bitmap_text_string_set_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, color_t color);
SGE_API void vulkan_bitmap_text_set_font(vulkan_bitmap_text_t* text, font_t* font);
/* internally calls vulkan_bitmap_text_set_font(),
 * and applies the passed font to all of its text string instances discarding their previous font assignments if any. */
SGE_API void vulkan_bitmap_text_set_font_update_all(vulkan_bitmap_text_t* text, font_t* font);
/* sets color attribute for each characters lying the range list passed to this function 
 * ranges: is the list of ranges where each range consists of 'begin' and 'end' index
 * range_count: is the number of ranges */
SGE_API void vulkan_bitmap_text_string_set_char_attr_color(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, const char_attr_color_range_t* ranges, const u32 range_count);
/* TODO:
 * vulkan_bitmap_text_string_set_char_attr_underline(..., const u32* ranges, u32 range_count)
 * vulkan_bitmap_text_string_set_char_attr_bold(..., const u32* ranges, u32 range_count)
 * vulkan_bitmap_text_string_set_char_attr_italic(..., const u32* ranges, u32 range_count)
 */

/* getters */
SGE_API u32 vulkan_bitmap_text_get_point_size(vulkan_bitmap_text_t* text);
SGE_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
SGE_API u32 vulkan_bitmap_text_string_get_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
SGE_API font_t* vulkan_bitmap_text_string_get_fontH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
/* NOTE: this returns number of characters exclusive of null character at the end of 'chars' buffer */
SGE_API u32 vulkan_bitmap_text_string_get_lengthH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
SGE_API f32 vulkan_bitmap_text_string_get_zcoord_from_glyph_index(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 index);
SGE_API u32 vulkan_bitmap_text_string_get_glyph_index_from_zcoord(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, f32 zcoord);
SGE_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
SGE_API bool vulkan_bitmap_text_string_is_activeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
SGE_API font_t* vulkan_bitmap_text_get_font(vulkan_bitmap_text_t* text);

END_CPP_COMPATIBLE
