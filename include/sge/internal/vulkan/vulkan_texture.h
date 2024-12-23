/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_texture.h is a part of VulkanRenderer

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
/* for texture_attributes_t */
#include <sge/types.h>

#include <sge/internal/vulkan/vulkan_image.h>			 // vulkan_image_t
#include <sge/internal/vulkan/vulkan_image_view.h> 	 // vulkan_image_view_t
#include <sge/internal/vulkan/vulkan_types.h> 			 // vulkan_render_target_technique_t
#include <sge/internal/vulkan/vulkan_object.h>

typedef enum vulkan_texture_type_t
{
	VULKAN_TEXTURE_TYPE_ALBEDO = 0, // to be used when the texture is meant to represent color on the display
	VULKAN_TEXTURE_TYPE_NORMAL = 1, // to be used when the texture is meant to represent normals
	VULKAN_TEXTURE_TYPE_COLOR = 2,  // when the texture meant to store/represent color but not on the display
	VULKAN_TEXTURE_TYPE_DEPTH = 3,  // when the texture meant to store a single channel, typically depth values
	VULKAN_TEXTURE_TYPE_CUBE = BIT32(4),	// to be used when the texture (or textures) is meant to represent a cube map
	VULKAN_TEXTURE_TYPE_RENDER_TARGET = BIT32(5),

	VULKAN_TEXTURE_TYPE_ALBEDO_CUBE = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE,
	VULKAN_TEXTURE_TYPE_COLOR_CUBE = VULKAN_TEXTURE_TYPE_COLOR | VULKAN_TEXTURE_TYPE_CUBE,
	VULKAN_TEXTURE_TYPE_DEPTH_CUBE = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_CUBE,

	VULKAN_TEXTURE_TYPE_ALBEDO_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_ALBEDO_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
	VULKAN_TEXTURE_TYPE_COLOR_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_COLOR_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
	VULKAN_TEXTURE_TYPE_DEPTH_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_DEPTH_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET
} vulkan_texture_type_t;

typedef enum vulkan_texture_usage_t
{
	VULKAN_TEXTURE_USAGE_NONE = 0,
	VULKAN_TEXTURE_USAGE_SAMPLED = BIT32(0),		// when the texture used in the shader
	VULKAN_TEXTURE_USAGE_TRANSFER_DST = BIT32(1), 	// when the texture to be filled with data
	VULKAN_TEXTURE_USAGE_TRANSFER_SRC = BIT32(2), 	// when the texture data has to be copied somewhere
	VULKAN_TEXTURE_USAGE_PRESENT = BIT32(3), 		// when the texture used as a post-processing intermediate image, and finally present it
	VULKAN_TEXTURE_USAGE_RENDER_TARGET = BIT32(4), 	// when the texture used as a render target to draw over it
	VULKAN_TEXTURE_USAGE_ATTACHMENT = BIT32(5), 	// when the texture used as an attachment in the framebuffer
	VULKAN_TEXTURE_USAGE_COPY = VULKAN_TEXTURE_USAGE_TRANSFER_SRC,
	VULKAN_TEXTURE_USAGE_PASTE = VULKAN_TEXTURE_USAGE_TRANSFER_DST
} vulkan_texture_usage_t;

typedef enum vulkan_texture_usage_stage_t
{
	VULKAN_TEXTURE_USAGE_STAGE_UNDEFINED = 0,
	VULKAN_TEXTURE_USAGE_STAGE_INITIAL,
	VULKAN_TEXTURE_USAGE_STAGE_USAGE,
	VULKAN_TEXTURE_USAGE_STAGE_FINAL
} vulkan_texture_usage_stage_t;

typedef struct vulkan_texture_data_t
{
	void* data;				// pointer to the bytes (texel values)
	u32 width;				// width of the texture in texels
	u32 height;				// height of the texture in texels
	u32 depth; 				// depth of the texture in texels
	u8 channel_count;		// number of channels present in the texture, valid range [1, 4]
} vulkan_texture_data_t;

typedef struct vulkan_texture_create_info_t
{
	sge_vulkan_flags_t flags;
	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count; 		// ignored if this is a depth texture, otherwise valid values are in the range [1, 4]

	vulkan_texture_type_t type;						// type of the final vulkan texure i.e VULKAN_TEXTURE_TYPE_ALBEDO, VULKAN_TEXTURE_TYPE_NORMAL or VULKAN_TEXTURE_TYPE_CUBE
	vulkan_texture_usage_t initial_usage; 			// intial usage of the texture
	vulkan_texture_usage_t usage; 					// runtime usage of the texture
	vulkan_texture_usage_t final_usage; 			// final usage of the texture
	vulkan_render_target_technique_t technique; 	// ignored if this texture is not being used as a render target
} vulkan_texture_create_info_t;

typedef struct vulkan_texture_recreate_info_t
{
	u32 width;
	u32 height;
	u32 depth;
	u32 channel_count;
} vulkan_texture_recreate_info_t;

typedef struct vulkan_texture_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer; 							// pointer to the vulkan_renderer_t object
	vulkan_image_t image;									// vulkan image object

	/* image view: for sampling the texture or writing to the 2D render texture */
	vulkan_image_view_t image_view; 						// vulkan image view object
	
	VkSampler vo_image_sampler; 							// vulkan image sampler object

	// internal use only
	VkDescriptorType vo_descriptor_type;	 				// type of descriptor to which this texture would be bound

	/* image views: for writing/renderingg to the texture, typically Cube render texture */
	vulkan_image_view_t* image_views;
	u32 image_view_count;

	union
	{
		texture_attributes_t attributes;
		struct
		{
			u32 width;
			u32 height;
			u32 depth;
			u8 channel_count;
		};
	};

	vulkan_texture_type_t type;

	vulkan_texture_usage_t initial_usage;
	vulkan_texture_usage_t usage;
	vulkan_texture_usage_t final_usage;
	vulkan_render_target_technique_t technique;

	vulkan_texture_usage_stage_t current_stage;
} vulkan_texture_t;

#define VULKAN_TEXTURE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_texture_t*, VULKAN_OBJECT_TYPE_TEXTURE, ptr)
#define VULKAN_TEXTURE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_texture_t*, VULKAN_OBJECT_TYPE_TEXTURE, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_TEXTURE) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_TEXTURE_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_texture_t*, VULKAN_OBJECT_TYPE_TEXTURE, ptr)
#define VULKAN_TEXTURE_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_texture_t*, VULKAN_OBJECT_TYPE_TEXTURE, ptr)

BEGIN_CPP_COMPATIBLE

/* creates a new vulkan_texture_t object, ready to be used in vulkan_texture_create_no_alloc function */
SGE_API vulkan_texture_t* vulkan_texture_new(memory_allocator_t* allocator);

/*	
	description:
		creates a vulkan texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created vulkan texture object
 */
SGE_API vulkan_texture_t* vulkan_texture_create(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info);
SGE_API void vulkan_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info, vulkan_texture_t OUT texture);
SGE_API void vulkan_texture_recreate(vulkan_texture_t* texture, vulkan_texture_recreate_info_t* recreate_info);

/*
	description:
		destroys vulkan objects
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
SGE_API void vulkan_texture_destroy(vulkan_texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
SGE_API void vulkan_texture_release_resources(vulkan_texture_t* texture);

/* transitions from initial usage state to the runtime usage state */
SGE_API void vulkan_texture_set_usage_stage(vulkan_texture_t* texture, vulkan_texture_usage_stage_t stage);

/* uploads the data to the GPU local memory */
SGE_API void vulkan_texture_upload_data(vulkan_texture_t* texture, u32 data_count, vulkan_texture_data_t* data);

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_texture_is_cube(vulkan_texture_t* texture)
{
	return HAS_FLAG(texture->type, VULKAN_TEXTURE_TYPE_CUBE);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_texture_is_3d(vulkan_texture_t* texture)
{
	return (texture->width > 1) && (texture->height > 1) && (texture->depth > 1);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_texture_is_2d(vulkan_texture_t* texture)
{
	return texture->depth == 1;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_texture_is_1d(vulkan_texture_t* texture)
{
	return (texture->height == 1) && (texture->depth == 1);
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_image_view_t* vulkan_texture_get_image_views(vulkan_texture_t* texture)
{
	return vulkan_texture_is_cube(texture) ? texture->image_views : &texture->image_view;
}

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE u32 vulkan_texture_get_image_view_count(vulkan_texture_t* texture)
{
	return vulkan_texture_is_cube(texture) ? texture->image_view_count : 1;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_texture_usage_stage_t vulkan_texture_get_usage_stage(vulkan_texture_t* texture) { return texture->current_stage; }


static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const texture_attributes_t* vulkan_texture_get_attributes(const vulkan_texture_t* texture) { return &texture->attributes; }

END_CPP_COMPATIBLE
