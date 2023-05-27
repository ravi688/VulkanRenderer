/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: light.c is a part of VulkanRenderer

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


#include <renderer/light.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_light.h>

/* constructors & destructors */
RENDERER_API light_t* light_new(memory_allocator_t* allocator, light_type_t type)
{
	return vulkan_light_new(allocator, type);
}

RENDERER_API light_t* light_create(renderer_t* renderer, light_type_t type)
{
	return vulkan_light_create(renderer->vulkan_handle, REINTERPRET_TO(vulkan_light_type_t, type));
}

RENDERER_API void light_create_no_alloc(renderer_t* renderer, light_type_t type, light_t OUT light)
{
	return vulkan_light_create_no_alloc(renderer->vulkan_handle, REINTERPRET_TO(vulkan_light_type_t, type), light);
}

RENDERER_API void light_destroy(light_t* light)
{
	vulkan_light_destroy(light);
}

RENDERER_API void light_release_resources(light_t* light)
{
	vulkan_light_release_resources(light);
}

/* setters */

RENDERER_API void light_set_spot_angle(light_t* light, float angle)
{
	vulkan_light_set_spot_angle(light, angle);
}

RENDERER_API void light_set_position(light_t* light, vec3_t position)
{
	vulkan_light_set_position(light, position);
}

RENDERER_API void light_set_rotation(light_t* light, vec3_t rotation)
{
	vulkan_light_set_rotation(light, rotation);
}

RENDERER_API void light_set_intensity(light_t* light, float intensity)
{
	vulkan_light_set_intensity(light, intensity);
}

RENDERER_API void light_set_color(light_t* light, vec3_t color)
{
	vulkan_light_set_color(light, color);
}

