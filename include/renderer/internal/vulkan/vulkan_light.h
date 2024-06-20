/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_light.h is a part of VulkanRenderer

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
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/internal/vulkan/vulkan_object.h>

#include <hpml/vec3.h>
#include <hpml/mat4.h>

typedef enum vulkan_light_type_t
{
	VULKAN_LIGHT_TYPE_UNDEFINED = 0,
	VULKAN_LIGHT_TYPE_DIRECTIONAL = 1,
	VULKAN_LIGHT_TYPE_POINT = 2,
	VULKAN_LIGHT_TYPE_OMIN = VULKAN_LIGHT_TYPE_POINT,
	VULKAN_LIGHT_TYPE_SPOT = 3,
	VULKAN_LIGHT_TYPE_AREA = 4,
	VULKAN_LIGHT_TYPE_CURVE = 5,
	VULKAN_LIGHT_TYPE_VOLUME = 6,
	VULKAN_LIGHT_TYPE_AMBIENT = 7
} vulkan_light_type_t;

typedef struct vulkan_light_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	vulkan_buffer_t buffer;
	struct_descriptor_t struct_definition;

	/* internal use only, however can be used externally also! */
	vulkan_light_type_t type;

	struct_field_handle_t projection_handle;	 // STRUCT_FIELD_INVALID_HANDLE in case of ambient light
	struct_field_handle_t view_handle; 			 // STRUCT_FIELD_INVALID_HANDLE in case of ambient light
	struct_field_handle_t color_handle;
	struct_field_handle_t intensity_handle;

	// cpu side cache to reduce GPU memory access
	mat4_t projection;
	mat4_t view;
	vec3_t color;
	float intensity;

	vec3_t position;
	mat4_t rotation;
	vec3_t euler_rotation;

} vulkan_light_t;

#define VULKAN_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)
#define VULKAN_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_light_t*, VULKAN_OBJECT_TYPE_LIGHT, ptr)

typedef vulkan_light_t vulkan_ambient_light_t;

typedef struct vulkan_directional_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
	struct_field_handle_t direction_handle;
	vec3_t direction;
} vulkan_directional_light_t;

#define VULKAN_DIRECTIONAL_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_directional_light_t*, VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT, ptr)
#define VULKAN_DIRECTIONAL_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_directional_light_t*, VULKAN_OBJECT_TYPE_DIRECTIONAL_LIGHT, ptr)

typedef struct vulkan_point_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
	struct_field_handle_t position_handle;
} vulkan_point_light_t;

#define VULKAN_POINT_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_point_light_t*, VULKAN_OBJECT_TYPE_POINT_LIGHT, ptr)
#define VULKAN_POINT_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_point_light_t*, VULKAN_OBJECT_TYPE_POINT_LIGHT, ptr)

typedef struct vulkan_spot_light_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_light_t);
	struct_field_handle_t angle_handle;
	struct_field_handle_t direction_handle;
	struct_field_handle_t position_handle;
	float angle;
	vec3_t direction;
} vulkan_spot_light_t;

#define VULKAN_SPOT_LIGHT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_spot_light_t*, VULKAN_OBJECT_TYPE_SPOT_LIGHT, ptr)
#define VULKAN_SPOT_LIGHT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_spot_light_t*, VULKAN_OBJECT_TYPE_SPOT_LIGHT, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_light_t* vulkan_light_new(memory_allocator_t* allocator, vulkan_light_type_t type);
RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type);
RENDERER_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light);
RENDERER_API void vulkan_light_destroy(vulkan_light_t* light);
RENDERER_API void vulkan_light_release_resources(vulkan_light_t* light);

/* setters */
RENDERER_API void vulkan_light_set_intensity(vulkan_light_t* light, float intensity);
RENDERER_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t color);
RENDERER_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t position);
RENDERER_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t rotation);
RENDERER_API void vulkan_light_set_spot_angle(vulkan_light_t* light, float angle);


END_CPP_COMPATIBLE
