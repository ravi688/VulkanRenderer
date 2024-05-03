/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_texture.c is a part of VulkanRenderer

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

#include <renderer/render_texture.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_render_texture.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>

RENDERER_API render_texture_t* render_texture_create(renderer_t* renderer, render_texture_create_info_t* create_info)
{
	return vulkan_render_texture_create(renderer->handle, CAST_TO(vulkan_render_texture_create_info_t*, create_info));
}

RENDERER_API void render_texture_create_no_alloc(renderer_t* renderer, render_texture_create_info_t* create_info, render_texture_t OUT texture)
{
	vulkan_render_texture_create_no_alloc(renderer->handle, CAST_TO(vulkan_render_texture_create_info_t*, create_info), CAST_TO(vulkan_render_texture_t OUT, texture));
}

RENDERER_API void render_texture_destroy(render_texture_t* texture)
{
	vulkan_render_texture_destroy(CAST_TO(vulkan_render_texture_t*, texture));
}

RENDERER_API void render_texture_release_resources(render_texture_t* texture)
{
	vulkan_render_texture_release_resources(CAST_TO(vulkan_render_texture_t*, texture));
}

