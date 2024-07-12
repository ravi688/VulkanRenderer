/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: display.c is a part of VulkanRenderer

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

#include <sge/system/display.h>
#include <math.h>

SGE_API iextent2d_t display_get_resolution()
{
	/* width = 1920,  height = 1080 */
	return iextent2d(1920, 1080);
}

SGE_API iextent2d_t display_get_aspect_ratio()
{
	/* aspect ratio = width (in inches) : height (in inches) */
	return iextent2d(16, 9);
}

SGE_API f32 display_get_diagonal_size()
{
	/* 22.0f inches */
	return 22.0f;
}

SGE_API extent2d_t display_get_dpi()
{
	AUTO pixels = display_get_resolution();
	AUTO inches = display_get_inches();
	return extent2d(pixels.width / inches.width, pixels.height / inches.height);
}

SGE_API extent2d_t display_get_inches()
{
	f32 diagonal = display_get_diagonal_size();
	iextent2d_t aspect_ratio = display_get_aspect_ratio();
	u32 squared_sum = aspect_ratio.width * aspect_ratio.width + aspect_ratio.height * aspect_ratio.height;
	return extent2d(diagonal * aspect_ratio.width / sqrt(squared_sum), diagonal * aspect_ratio.height / sqrt(squared_sum));
}
