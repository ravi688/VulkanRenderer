/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: display.h is a part of VulkanRenderer

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

#include <sge/defines.h>
#include <sge/rect.h>

BEGIN_CPP_COMPATIBLE

/* BELOW SET OF THREE FUNCTIONS ARE TO QUERY SPECIFICATIONS ABOUT THE DISPLAY PROVIDED BY ITS MANUFACTURER */

/* returns display resolution in pixels { number of pixels across width, number of pixels across height } */
SGE_API iextent2d_t display_get_resolution();
/* returns aspect ratio in terms of separated { nominator (represents "primitive or reduced or factored out" width), denominator "primitive..." height }
 * for example, { 16, 9 } */
SGE_API iextent2d_t display_get_aspect_ratio();
/* returns diagonal size of the display in inches */
SGE_API f32 display_get_diagonal_size();


/* BELOW FUNCTIONS ARE DERIVED FROM THE ABOVE FUNCTIONS */

/* returns number of pixels per inch across width and height as { width_dpi, height_dpi } */
SGE_API extent2d_t display_get_dpi();
/* returns physical width and height of display in inches */
SGE_API extent2d_t display_get_inches();

END_CPP_COMPATIBLE
