/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_object.h is a part of VulkanRenderer

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

#include <vulkan/vulkan.h>
#include <sge/defines.h>

#include <sge/internal/vulkan/vulkan_descriptor_set.h> 	// for vulkan_descriptor_set_t
#include <sge/internal/vulkan/vulkan_buffer.h> 			// for vulkan_buffer_t
#include <sge/internal/vulkan/vulkan_handles.h> 			// for vulkan_render_object_handle_t
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/struct_descriptor.h> 						// for struct_descriptor_t


#include <hpml/mat4.h>

typedef struct vulkan_material_t vulkan_material_t;

// typedef struct vulkan_object_info_t
// {
// 	mat4_t transform;
// 	mat4_t normal;
// } vulkan_object_info_t;

typedef enum vulkan_render_object_type_t
{
	VULKAN_RENDER_OBJECT_TYPE_MESH = 1,
	VULKAN_RENDER_OBJECT_TYPE_TEXT_MESH = 2,
	VULKAN_RENDER_OBJECT_TYPE_TEXT = 3
} vulkan_render_object_type_t;

typedef void (*draw_call_handler_t)(void* object);
typedef void (*material_set_handler_t)(void* object, vulkan_material_t* material);

typedef struct vulkan_render_object_create_info_t
{
	vulkan_material_t* material; 		// optional
	void* user_data; 					// optional
	draw_call_handler_t draw_handler; 	// optional if type is valid
	vulkan_render_object_type_t type;	// optional if draw_handler is valid
} vulkan_render_object_create_info_t;

typedef struct vulkan_render_queue_t vulkan_render_queue_t;
typedef struct vulkan_render_scene_t vulkan_render_scene_t;

typedef struct vulkan_render_object_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;

	/* pointer to the queue in which this resides, it may be NULL if not added in any queue 
	 * it will be assigned non-null value whenever you call vulkan_render_queue_add or vulkan_render_scene_create_object 
	 * no writes are performed in this class (vuklan render object) */
	vulkan_render_queue_t* queue;

	vulkan_material_t* material;
	void* user_data;
	draw_call_handler_t draw;
	material_set_handler_t set_material;
	vulkan_descriptor_set_t object_set;
	vulkan_buffer_t buffer;

	// TODO: Since this is identical for all the render objects, 
	// 		it would be part of vulkan_renderer_t [Global Data] to save some memory
	struct_descriptor_t struct_definition;

	struct_field_handle_t transform_handle;
	struct_field_handle_t normal_handle;
} vulkan_render_object_t;

/* performs dynamic casting (expensive), use only when you don't know the source type */
#define VULKAN_RENDER_OBJECT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_render_object_t*, VULKAN_OBJECT_TYPE_RENDER_OBJECT, ptr)
#define VULKAN_RENDER_OBJECT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_render_object_t*, VULKAN_OBJECT_TYPE_RENDER_OBJECT, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_RENDER_OBJECT) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_RENDER_OBJECT_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_render_object_t*, VULKAN_OBJECT_TYPE_RENDER_OBJECT, ptr)
#define VULKAN_RENDER_OBJECT_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_render_object_t*, VULKAN_OBJECT_TYPE_RENDER_OBJECT, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructurs */
SGE_API vulkan_render_object_t* vulkan_render_object_new(memory_allocator_t* allocator);
SGE_API vulkan_render_object_t* vulkan_render_object_create(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info);
SGE_API void vulkan_render_object_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_object_create_info_t* create_info, vulkan_render_object_t OUT object);
SGE_API void vulkan_render_object_destroy(vulkan_render_object_t* object);
SGE_API void vulkan_render_object_release_resources(vulkan_render_object_t* object);

SGE_API void vulkan_render_object_attach(vulkan_render_object_t* object, void* user_data);
SGE_API void vulkan_render_object_draw(vulkan_render_object_t* object);
SGE_API void vulkan_render_object_set_material(vulkan_render_object_t* object, vulkan_material_t* material);
SGE_API vulkan_material_t* vulkan_render_object_get_material(vulkan_render_object_t* object);
SGE_API void vulkan_render_object_set_transform(vulkan_render_object_t* object, mat4_t transform);
SGE_API mat4_t vulkan_render_object_get_transform(vulkan_render_object_t* object);
SGE_API mat4_t vulkan_render_object_get_normal(vulkan_render_object_t* object);

END_CPP_COMPATIBLE
