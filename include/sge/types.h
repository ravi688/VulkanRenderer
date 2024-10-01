/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: types.h is a part of VulkanRenderer

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

typedef enum text_render_space_type_t
{
	/* selects the 'screen projection matrix' from the camera's projection matrices */
	TEXT_RENDER_SPACE_TYPE_2D,
	/* selects the 'orthographic/perspective matrix' from the camera's projection matrices */
	TEXT_RENDER_SPACE_TYPE_3D
} text_render_space_type_t;

/* WARNING: if you're thinking to change anything inside this struct, make sure to inspect every source file where it used.
 * because it also used in unions as a convenient alias to older interface. */
typedef struct texture_attributes_t
{
	u32 width;				// width of the texture in texels
	u32 height;				// height of the texture in texels
	u32 depth;
	u8 channel_count;		// number of channels present in the texture, for now it must be 4 (RGBA)
} texture_attributes_t;
