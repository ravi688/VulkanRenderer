/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_target.h is a part of VulkanRenderer

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


/* NOTE: this should be in sync with vulkan_render_target_type_t */
typedef enum render_target_type_t
{
	RENDER_TARGET_TYPE_UNDEFINED = 0
	RENDER_TARGET_TYPE_COLOR_TEXTURE,
	RENDER_TARGET_TYPE_DEPTH_TEXTURE,
	RENDER_TARGET_TYPE_COLOR_CUBEMAP,
	RENDER_TARGET_TYPE_DEPTH_CUBEMAP
} render_target_type_t;


typedef enum render_target_render_technique_t
{
	RENDER_TARGET_RENDER_TECHNIQUE_COPY = 0,
	RENDER_TARGET_RENDER_TECHNIQUE_ATTACH
} render_target_render_technique_t;
