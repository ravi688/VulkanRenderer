/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: rdr_object.h is a part of VulkanRenderer

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


#ifdef SGE_VULKAN_DRIVER
#	include <renderer/internal/vulkan/vulkan_object.h>
#	define DEFINE_ENUM(x) RDR_##x = VULKAN_##x
#	define RDR_OBJECT_INIT(ptr, type, nationality) VULKAN_OBJECT_INIT(ptr, type, nationality)
#elif SGE_OPENGL_DRIVER
#	include <renderer/internal/opengl/opengl_object.h>
#	define DEFINE_ENUM(x) RDR_##x = OPENGL_##x
#	define RDR_OBJECT_INIT(ptr, type, nationality) OPENGL_OBJECT_INIT(ptr, type, nationality)
#elif SGE_DIRECTX_DRIVER
#	include <renderer/internal/directx/directx_object.h>
#	define DEFINE_ENUM(x) RDR_##x = DIRECTX_##x
#	define RDR_OBJECT_INIT(ptr, type, nationality) DIRECTX_OBJECT_INIT(ptr, type, nationality)
#elif SGE_METAL_DRIVER
#	include <renderer/internal/metal/metal_object.h>
#	define DEFINE_ENUM(x) RDR_##x = METAL_##x
#	define RDR_OBJECT_INIT(ptr, type, nationality) METAL_OBJECT_INIT(ptr, type, nationality)
#endif

typedef enum rdr_object_nationality_t
{
	DEFINE_ENUM(OBJECT_NATIONALITY_UNDEFINED),
	DEFINE_ENUM(OBJECT_NATIONALITY_EXTERNAL),
	DEFINE_ENUM(OBJECT_NATIONALITY_INTERNAL),
	DEFINE_ENUM(OBJECT_NATIONALITY_MAX)
} rdr_object_nationality_t;
	
typedef enum rdr_object_type_t
{
	DEFINE_ENUM(OBJECT_TYPE_UNDEFINED),
	DEFINE_ENUM(OBJECT_TYPE_BUFFER),
	DEFINE_ENUM(OBJECT_TYPE_SHADER_MODULE),
	DEFINE_ENUM(OBJECT_TYPE_INSTANCE_BUFFER),
	DEFINE_ENUM(OBJECT_TYPE_HOST_BUFFERED_BUFFER),
	DEFINE_ENUM(OBJECT_TYPE_MESH),
	DEFINE_ENUM(OBJECT_TYPE_IMAGE),
	DEFINE_ENUM(OBJECT_TYPE_IMAGE_VIEW),
	DEFINE_ENUM(OBJECT_TYPE_TEXTURE),
	DEFINE_ENUM(OBJECT_TYPE_RENDER_TEXTURE),
	DEFINE_ENUM(OBJECT_TYPE_HOST_BUFFERED_TEXTURE),
	DEFINE_ENUM(OBJECT_TYPE_BITMAP_TEXT),
	DEFINE_ENUM(OBJECT_TYPE_BITMAP_GLYPH_ATLAS_TEXTURE),
	DEFINE_ENUM(OBJECT_TYPE_MAX)
} rdr_object_type_t;

#undef DEFINE_NUM
