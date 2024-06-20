/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: texture.h is a part of VulkanRenderer

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

typedef struct renderer_t renderer_t;

#if   defined(RENDERER_VULKAN_DRIVER)
#	include <renderer/internal/vulkan/vulkan_texture.h>
	typedef vulkan_texture_t texture_t;
#	define TEXTURE(ptr) VULKAN_TEXTURE(ptr)
#elif defined(RENDERER_OPENGL_DRIVER)
#	include <renderer/internal/opengl/opengl_texture.h>
	typedef opengl_texture_t texture_t;
#	define TEXTURE(ptr) OPENGL_TEXTURE(ptr)
#elif defined(RENDERER_DIRECTX_DRIVER)
#	include <renderer/internal/directx/directx_texture.h>
	typedef directx_texture_t texture_t;
#	define TEXTURE(ptr) DIRECTX_TEXTURE(ptr)
#elif defined(RENDERER_METAL_DRIVER)
#	include <renderer/internal/metal/metal_texture.h>
	typedef metal_texture_t texture_t;
#	define TEXTURE(ptr) METAL_TEXTURE(ptr)
#endif


#include <renderer/defines.h>
#include <stdarg.h>

typedef enum texture_type_t
{
	TEXTURE_TYPE_ALBEDO = 0,									// to be used when the texture is meant to represent color
	TEXTURE_TYPE_NORMAL = 1,									// to be used when the texture is meant to represent normals
	TEXTURE_TYPE_CUBE_COMBINED = 2,								// to be used when cube map texture is just a single image
	TEXTURE_TYPE_CUBE_SEPARATED = 3,							// to be used when cube map texture is separated into multiple images for each faces
	TEXTURE_TYPE_CUBE = TEXTURE_TYPE_CUBE_SEPARATED				// by default the cube map would be 6 separate textures
} texture_type_t;

typedef enum texture_usage_t
{
	TEXTURE_USAGE_NONE = 0,
	TEXTURE_USAGE_SAMPLED = BIT32(0),		// when the texture used in the shader
	TEXTURE_USAGE_TRANSFER_DST = BIT32(1), 	// when the texture to be filled with data
	TEXTURE_USAGE_TRANSFER_SRC = BIT32(2), 	// when the texture data has to be copied somewhere
	TEXTURE_USAGE_PRESENT = BIT32(3), 		// when the texture used as a post-processing intermediate image, and finally present it
	TEXTURE_USAGE_RENDER_TARGET = BIT32(4), 	// when the texture used as a render target to draw over it
	TEXTURE_USAGE_ATTACHMENT = BIT32(5), 	// when the texture used as an attachment in the framebuffer
	TEXTURE_USAGE_COPY = TEXTURE_USAGE_TRANSFER_SRC,
	TEXTURE_USAGE_PASTE = TEXTURE_USAGE_TRANSFER_DST
} texture_usage_t;

typedef enum texture_usage_stage_t
{
	TEXTURE_USAGE_STAGE_UNDEFINED,
	TEXTURE_USAGE_STAGE_INITIAL,
	TEXTURE_USAGE_STAGE_USAGE,
	TEXTURE_USAGE_STAGE_FINAL
} texture_usage_stage_t;

typedef struct texture_data_t
{
	void* data;				// pointer to the bytes (texel values)
	u32 width;				// width of the texture in texels
	u32 height;				// height of the texture in texels
	u32 depth;
	u8 channel_count;		// number of channels present in the texture, for now it must be 4 (RGBA)
} texture_data_t;

typedef struct texture_create_info_t
{
	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count;
	texture_type_t type; 						// type of the this texture
	texture_usage_t initial_usage;
	texture_usage_t usage;
	texture_usage_t final_usage;
} texture_create_info_t;

BEGIN_CPP_COMPATIBLE
/*	
	description:
		creates a texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created texture object
 */
RENDERER_API texture_t* texture_create(renderer_t* renderer, texture_create_info_t* create_info);

/*
	description:
		creates a texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		type: type of the texture, must be a valid texture_type_t
		variable arguements: file path strings i.e. "path/textures/front.bmp", "path/textures/back.bmp", ...
	returns:
		pointer to the newly created texture object
 */
RENDERER_API texture_t* texture_load(renderer_t* renderer, texture_type_t type, ...);

/*
	description:
		creates a texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		type: type of the texture, must be a valid texture_type_t
		file_paths: file path strings i.e. "path/textures/front.bmp", "path/textures/back.bmp", ...
	returns:
		pointer to the newly created texture object
 */
RENDERER_API texture_t* texture_loadv(renderer_t* renderer, texture_type_t type, va_list file_paths);

/*
	description:
		uploads pixel data buffer supplied by 'data' to the GPU side memory
	params:
		texture: pointer to a valid texture_t object
		data: pointer to texture_data_t object referencing valid pixel data buffer and dimensions
	returns:
		nothing
 */
RENDERER_API void texture_upload_data(texture_t* texture, u32 data_count, texture_data_t* data);

/* 
	description:
		performs required image layout transitions based on the passed usage stage 'stage'
	params:
		texture: pointer to a valid texture_t object
		stage: the usage stage to transition to
	returns:
		nothing
 */
RENDERER_API void texture_set_usage_stage(texture_t* texture, texture_usage_stage_t stage);

/*
	description:
		destroys api specific objects
	params:
		texture: pointer to the valid texture_t object
	returns:
		nothing
 */
RENDERER_API void texture_destroy(texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid texture_t object
	returns:
		nothing
 */
RENDERER_API void texture_release_resources(texture_t* texture);

END_CPP_COMPATIBLE
