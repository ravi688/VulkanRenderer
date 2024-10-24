/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_text_types.h is a part of VulkanRenderer

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

#include <hpml/vec3.h> // vec3_t
#include <sge/color.h> // icolor4_t
#include <sge/rect.h> // rect3d_t
#include <sge/font.h> // utf32_t
#include <bufferlib/buffer.h> // buffer_t

/* holds the post processed layout data for each glyph */
typedef struct vulkan_text_glyph_layout_data_t
{
	/* index of the glyph in the string */
	u32 index;
	/* offset added to this glyph, in pixel coordinates if space is 2D, otherwise in world coordinates */
	vec3_t offset;
	/* color of this glyph (8 bit per component)*/
	icolor4_t color;
	/* if true, the text renderer chooses italic family of the font face */
	bool is_italic;
	/* if true, the text renderer chooses bold family of the font face
	 * NOTE: if is_italic and is_bold both are true, then the text renderer chooses bold-italic family of the font face */
	bool is_bold;
	/* if true, the text renderer draw a thin rectangle just underneath of this glyph via a strikethrough callback */
	bool is_underline;
	/* if true, the text renderer draws a thin rectangle through the middle of this glyph via a strikethrough callback */
	bool is_strikethrough;
	/* intermediate data object for communcation (idoc) */
	void* idoc;
} vulkan_text_glyph_layout_data_t;

typedef buffer_t /* vulkan_text_glyph_layout_data_t */ vulkan_text_glyph_layout_data_buffer_t;

/* holds precursor data to create post processed glyph layout data (vulkan_text_glyph_layout_data_t) */
typedef struct vulkan_text_glyph_info_t
{
	/* unicode of this glyph */
	utf32_t unicode;
	/* bounding box of this glyph in pixel coords, if 2D, otherwise world space coords (if 3D) */
	rect3d_t rect;
} vulkan_text_glyph_info_t;

/* holds input data for the glyph layout handler */
typedef struct vulkan_text_glyph_layout_handler_input_data_t
{
	/* stores list of precursor glyph layout data */
	vulkan_text_glyph_info_t* glyphs;
	u32 glyph_count;
	/* anchored position of the text string in pixel coords if the space 2D, otherwise in worldspace coords */
	vec3_t anchor_pos;
} vulkan_text_glyph_layout_handler_input_data_t;

typedef bool (*vulkan_text_glyph_layout_handler_t)(vulkan_text_glyph_layout_data_buffer_t* output, const vulkan_text_glyph_layout_handler_input_data_t* input, void* user_data);

typedef struct vulkan_render_object_t vulkan_render_object_t;

typedef vulkan_render_object_t* (*vulkan_text_glyph_strikethrough_handler_t)(vec3_t start, vec3_t end, void* idoc, void* user_data);
typedef vulkan_render_object_t* (*vulkan_text_glyph_underline_handler_t)(vec3_t start, vec3_t end, void* idoc, void* user_data);

typedef_pair_t(vulkan_text_glyph_layout_handler_t, void_ptr_t);
typedef_pair_t(vulkan_text_glyph_strikethrough_handler_t, void_ptr_t);
typedef_pair_t(vulkan_text_glyph_underline_handler_t, void_ptr_t);

typedef pair_t(vulkan_text_glyph_layout_handler_t, void_ptr_t) vulkan_text_glyph_layout_handler_void_ptr_pair_t;
typedef pair_t(vulkan_text_glyph_strikethrough_handler_t, void_ptr_t) vulkan_text_glyph_strikethrough_handler_void_ptr_pair_t;
typedef pair_t(vulkan_text_glyph_underline_handler_t, void_ptr_t) vulkan_text_glyph_underline_handler_void_ptr_pair_t;
