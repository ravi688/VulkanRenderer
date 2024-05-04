/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_descriptor_set.h is a part of VulkanRenderer

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
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_object.h>

// NOTE: This must be in sync with v3d.h
enum
{
	VULKAN_DESCRIPTOR_SET_CAMERA 	 =  0,
	VULKAN_DESCRIPTOR_SET_GLOBAL 	 = 	1,
	VULKAN_DESCRIPTOR_SET_RENDER 	 = 	2,
	VULKAN_DESCRIPTOR_SET_SUB_RENDER =  3,
	VULKAN_DESCRIPTOR_SET_MATERIAL 	 = 	4,
	VULKAN_DESCRIPTOR_SET_OBJECT 	 =	5
};

// NOTE: This must be in sync with v3d.h
enum
{
	// bindings for CAMERA_SET
	VULKAN_DESCRIPTOR_BINDING_CAMERA_PROPERTIES = 0,
	
	// bindings for GLOBAL_SET
	VULKAN_DESCRIPTOR_BINDING_CAMERA = 0,
	VULKAN_DESCRIPTOR_BINDING_LIGHT = 1,
	/* holds information related to the screen such as:
	 * 1. display resolution (width x height, in pixels)
	 * 2. display dpi (horizontal_dpi x vertical_dpi, in pixels / inch)
	 * 3. window size (width x height, in pixels)
	 * 4. screen_matrix  */
	VULKAN_DESCRIPTOR_BINDING_SCREEN = 2,

	// bindings for SUB_RENDER_SET
	VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT0 = 0,
	VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT1 = 1,
	VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT2 = 2,

	// bindigs for MATERIAL_SET
	VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES = 0,

	// bindings for MATERIAL_SET & RENDER_SET
	VULKAN_DESCRIPTOR_BINDING_TEXTURE0 = 1,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE1 = 2,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE2 = 3,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE3 = 4,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE4 = 5,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE5 = 6,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE6 = 7,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE7 = 8,
	VULKAN_DESCRIPTOR_BINDING_TEXTURE8 = 9,

	// bindings for OBJECT_SET
	VULKAN_DESCRIPTOR_BINDING_TRANSFORM = 0
};

typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;
typedef struct vulkan_descriptor_set_layout_t vulkan_descriptor_set_layout_t;

typedef struct vulkan_descriptor_set_create_info_t
{
	VkDescriptorPool vo_pool;				// the pool from the set must be allocated
	vulkan_descriptor_set_layout_t* layout;		// the layout of the set (contains a set of different types of resource descriptors)
} vulkan_descriptor_set_create_info_t;

typedef struct vulkan_descriptor_set_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	VkDescriptorSet vo_handle;
	VkDescriptorPool vo_pool; //the pool from it has been allocated
} vulkan_descriptor_set_t;

#define VULKAN_DESCRIPTOR_SET(ptr) VULKAN_OBJECT_UP_CAST(vulkan_descriptor_set_t*, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET, ptr)
#define VULKAN_DESCRIPTOR_SET_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_descriptor_set_t*, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET, ptr)

BEGIN_CPP_COMPATIBLE

/* 
	description:
		allocates vulkan_descriptor_set_t object on the heap, zeros out the memory and then returns it
	params:
		nothing
	returns:
		pointer to newly created vulkan_descriptor_set_t object on the heap
 */
RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_new(memory_allocator_t* allocator);

/* 
	description:
		creates vulkan descriptor set (VkDescriptorSet) object,
		it internally calls vulkan_descriptor_set_new()
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_descriptor_set_create_info_t object
	returns:
		pointer to newly created vulkan_descriptor_set_t object on the heap
 */
RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_create(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info);


/* 
	description:
		creates a descriptor set (VkDescriptorSet) object,
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_descriptor_set_create_info_t object
		set: pointer to already allocated (stack or heap) vulkan_descriptor_set_t object,
				make sure to initialize the memory to zero or using vulkan_descriptor_set_new() to allocate the object on heap
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_create_no_alloc(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set);
RENDERER_API void vulkan_descriptor_set_create_no_alloc_ext(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set);

/*
	description:
		destroys (VkFreeDescriptorSets) the vulkan objects (VkDescriptorSet)
	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_destroy(vulkan_descriptor_set_t* set);

/*
	description:
		releases heap allocated memory for vulkan_descriptor_set_t object (which must have been allocated/created by vulkan_descriptor_set_new or vulkan_descriptor_set_create)
		if the vulkan_descriptor_set_t object has been allocated on stack then don't call this function.
	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create or allocated by vulkan_descriptor_set_new)
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_release_resources(vulkan_descriptor_set_t* set);

/*
 	description:
 		binds (vkCmdBindDescriptorSets) the descriptor set to the pipline
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		set_number: set number
 		pipeline_layout: compatible pipeline layout, which has been created with the same descriptor set layout from which this descriptor set has been created
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_bind(vulkan_descriptor_set_t* set, u32 set_number, vulkan_pipeline_layout_t* pipeline_layout);

/*
 	description:
 		updates the descriptor at binding "binding_index" with a texture resource
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		binding_index: binding number/index of the resource descriptor as defined in the descriptor set layout when this descriptor set has been created
		texture: pointer to a valid vulkan_texture_t object
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_write_texture(vulkan_descriptor_set_t* set, u32 binding_index, vulkan_texture_t* texture);


/*
 	description:
 		updates the descriptor at binding "binding_index" with a vulkan_buffer resource as uniform buffer
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		binding_index: binding number/index of the resource descriptor as defined in the descriptor set layout when this descriptor set has been created
		texture: pointer to a valid vulkan_buffer_t object
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_write_uniform_buffer(vulkan_descriptor_set_t* set, u32 binding_index, vulkan_buffer_t* buffer);


END_CPP_COMPATIBLE
