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

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_instance_buffer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/internal/vulkan/vulkan_text_types.h>
#include <renderer/internal/vulkan/vulkan_object.h>
#include <renderer/dictionary.h>
#include <renderer/types.h>
#include <renderer/event.h> // event_subscription_handle_t
#include <renderer/rect.h> // rect2d_t
#include <hpml/mat4.h> 	// mat4_t
#include <hpml/vec3.h> /* vec3_t */
#include <hpml/vec2.h> /* vec2_t */
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

/* element of Glyph Texture Coordinate (GTC) buffer */
typedef struct vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t
{
	/* per_instance [BTM_TXT_TLTC_BND, BTM_TXT_TLTC_LOC, BTM_TXT_TLTC_COMP] in vec2 tltc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t tltc;
	/* per_instance [BTM_TXT_TRTC_BND, BTM_TXT_TRTC_LOC, BTM_TXT_TRTC_COMP] in vec2 trtc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t trtc;
	/* per_instance [BTM_TXT_BRTC_BND, BTM_TXT_BRTC_LOC, BTM_TXT_BRTC_COMP] in vec2 brtc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t brtc;
	/* per_instance [BTM_TXT_BLTC_BND, BTM_TXT_BLTC_LOC, BTM_TXT_BLTC_COMP] in vec2 bltc; */
	ALIGN_AS(GLSL_STD140_VEC2_ALIGN) vec2_t bltc;
} vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t ALIGN_AS(U32_NEXT_MULTIPLE(GLSL_STD140_VEC2_ALIGN, 16));

/* deterministic and minimum (portable) size of vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t
 * the C compiler may add extra padding resulting in non-portable code or undefined behaviour */
#define SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T (4 * COM_GET_STRIDE_IN_ARRAY(8, GLSL_STD140_VEC2_ALIGN))
/* deterministic (portable) size of vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t in a homogeneous array, i.e. stride to the next element.
 * same as reason as above */
#define STRIDE_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_VULKAN_BITMAP_TEXT_GLYPH_GLSL_GLYPH_TEXCOORD_T, ALIGN_OF(vulkan_bitmap_text_glyph_glsl_glyph_texcoord_t))

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
	ALIGN_AS(GLSL_STD140_VEC3_ALIGN) vec3_t scal;
} vulkan_bitmap_text_glsl_glyph_render_data_t ALIGN_AS(U32_MAX_OF(GLSL_STD140_UINT_ALIGN, GLSL_STD140_VEC3_ALIGN));

#define SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T (16 * 3)
#define STRIDE_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T_ARRAY COM_GET_STRIDE_IN_ARRAY(SIZEOF_VULKAN_BITMAP_TEXT_GLSL_GLYPH_RENDER_DATA_T, ALIGN_OF(vulkan_bitmap_text_glsl_glyph_render_data_t))

/* character buffer to store the characters in a string */
typedef buffer_t vulkan_bitmap_text_char_buffer_t;
typedef buf_ucount_t vulkan_bitmap_text_string_handle_t;
typedef dictionary_t vulkan_bitmap_glyph_sub_buffer_handle_table_t;

typedef struct vulkan_bitmap_text_string_t
{
	/* handle to the next free string if this string is in the free list or inuse string if this is in the inuse list */
	vulkan_bitmap_text_string_handle_t next;
	/* handle to this string */
	vulkan_bitmap_text_string_handle_t handle;
	sub_buffer_handle_t render_data_handle;
	/* string */
	vulkan_bitmap_text_char_buffer_t chars;
	/* a rectangle in a 3D space
	 * holds the position and the extents of the text */
	struct { offset3d_t offset; extent2d_t extent; } rect;
	/* transform matrix applied to this string */
	mat4_t transform;
	/* point size for this text string at which the glyphs will be rasterized.
	 * This 'point_size' value will be affected by vulkan_bitmap_text_set_point_size and
	 * vulkan_bitmap_text_string_set_point_sizeH. */
	u32 point_size;
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
	/* GTC buffer */
	vulkan_bitmap_glyph_texcoord_buffer_t glyph_texcoord_buffer;
	/* TST buffer */
	vulkan_bitmap_text_string_transform_buffer_t text_string_transform_buffer;

	glyph_texcoord_index_table_t glyph_texcoord_index_table;

	/* instances of this quad will be drawn to lay out the glyph bitmaps */
	vulkan_mesh_t quad_mesh;

	event_subscription_handle_t bga_texture_update_handle;

} vulkan_bitmap_text_t;

#define VULKAN_BITMAP_TEXT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)
#define VULKAN_BITMAP_TEXT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const bitmap_text_t*, VULKAN_OBJECT_TYPE_BITMAP_TEXT, ptr)


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
RENDERER_API void vulkan_bitmap_text_set_glyph_layout_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_layout_handler_t handler, void* user_data);
RENDERER_API void vulkan_bitmap_text_set_glyph_strikethrough_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_strikethrough_handler_t handler, void* user_data);
RENDERER_API void vulkan_bitmap_text_set_glyph_underline_handler(vulkan_bitmap_text_t* text, vulkan_bitmap_text_glyph_underline_handler_t handler, void* user_data);
RENDERER_API void vulkan_bitmap_text_set_point_size(vulkan_bitmap_text_t* text, u32 point_size);
RENDERER_API void vulkan_bitmap_text_set_material(vulkan_bitmap_text_t* text, vulkan_material_t* material);
RENDERER_API void vulkan_bitmap_text_set_render_space_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_space_type_t space_type);
RENDERER_API void vulkan_bitmap_text_set_render_surface_type(vulkan_bitmap_text_t* text, vulkan_bitmap_text_render_surface_type_t surface_type);
/* TODO
 * vulkan_bitmap_text_string_appendH()
 * vulkan_bitmap_text_string_insertH()
 * vulkan_bitmap_text_string_removeH()
 */
RENDERER_API void vulkan_bitmap_text_string_setH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, const char* string);
RENDERER_API void vulkan_bitmap_text_string_set_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, u32 point_size);
RENDERER_API void vulkan_bitmap_text_string_set_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle, mat4_t transform);

/* getters */
RENDERER_API u32 vulkan_bitmap_text_get_point_size(vulkan_bitmap_text_t* text);
RENDERER_API const char* vulkan_bitmap_text_string_getH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API u32 vulkan_bitmap_text_string_get_point_sizeH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API mat4_t vulkan_bitmap_text_string_get_transformH(vulkan_bitmap_text_t* text, vulkan_bitmap_text_string_handle_t handle);
RENDERER_API font_t* vulkan_bitmap_text_get_font(vulkan_bitmap_text_t* text);

END_CPP_COMPATIBLE
