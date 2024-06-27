/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: light.h is a part of VulkanRenderer

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

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_light_t vulkan_light_t;
	typedef vulkan_light_t light_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_light_t opengl_light_t;
	typedef opengl_light_t light_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_light_t directx_light_t;
	typedef directx_light_t light_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_light_t metal_light_t;
	typedef metal_light_t light_t;
#endif

#include <renderer/defines.h>
#include <hpml/vec3.h>
#include <hpml/mat4.h>

typedef enum light_type_t
{
	LIGHT_TYPE_UNDEFINED = 0,
	LIGHT_TYPE_DIRECTIONAL = 1,
	LIGHT_TYPE_POINT = 2,
	LIGHT_TYPE_OMIN = LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT = 3,
	LIGHT_TYPE_AREA = 4,
	LIGHT_TYPE_CURVE = 5,
	LIGHT_TYPE_VOLUME = 6,
	LIGHT_TYPE_AMBIENT = 7
} light_type_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API light_t* light_new(memory_allocator_t* allocator, light_type_t type);
RENDERER_API light_t* light_create(renderer_t* renderer, light_type_t type);
RENDERER_API void light_create_no_alloc(renderer_t* renderer, light_type_t type, light_t OUT light);
RENDERER_API void light_destroy(light_t* light);
RENDERER_API void light_release_resources(light_t* light);

RENDERER_API void light_set_spot_angle(light_t* light, float angle);
RENDERER_API void light_set_position(light_t* light, vec3_t position);
RENDERER_API void light_set_rotation(light_t* light, vec3_t rotation);
RENDERER_API void light_set_intensity(light_t* light, float intensity);
RENDERER_API void light_set_color(light_t* light, vec3_t color);
RENDERER_API void light_set_cast_shadow(light_t* light, bool is_cast_shadows);

END_CPP_COMPATIBLE
