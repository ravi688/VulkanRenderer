/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: io.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

typedef struct file_info_t
{
	char* path;
	u64 size;
} file_info_t;


typedef enum directory_search_type_t
{
	DIRECTORY_SERACH_TYPE_DEFAULT = 0,
	DIRECTORY_SEARCH_TYPE_RECURSIVE
} directory_search_type_t;

/*
	description: searches a list of files with filter in a directory
	params:
		directory: directory to search in
		filter: filter to be applied
		search_type: type of the search
	returns:
		BUFFER, list of file_info_t objects
 */
BUFFER directory_search_files(const char* directory, const char* filter, directory_search_type_t search_type);

