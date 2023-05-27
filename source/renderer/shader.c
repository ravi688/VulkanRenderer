/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: shader.c is a part of VulkanRenderer

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


#include <renderer/shader.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader.h>

RENDERER_API shader_t* shader_create(renderer_t* renderer, shader_create_info_t* create_info)
{
	return CAST_TO(shader_t*, vulkan_shader_create(renderer->vulkan_handle, CAST_TO(vulkan_shader_create_info_t*, create_info)));
}

RENDERER_API shader_t* shader_load(renderer_t* renderer, const char* file_path)
{
	// TODO: there might be a possibility to check for the compatible shaders across the APIs,
	// for example a shader compiled for vulkan might not be compatible for opengl or directx.
	// but for now lets go with vulkan only.
	vulkan_shader_load_info_t load_info =
	{
		.path = file_path,
		.is_vertex_attrib_from_file = true
	};
	return CAST_TO(shader_t*, vulkan_shader_load(renderer->vulkan_handle, &load_info));
}

RENDERER_API void shader_destroy(shader_t* shader)
{
	vulkan_shader_destroy(CAST_TO(vulkan_shader_t*, shader));
}

RENDERER_API void shader_release_resources(shader_t* shader)
{
	vulkan_shader_release_resources(CAST_TO(vulkan_shader_t*, shader));
}
