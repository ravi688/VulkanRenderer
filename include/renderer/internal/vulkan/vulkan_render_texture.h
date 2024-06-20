/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_texture.h is a part of VulkanRenderer

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
#include <renderer/internal/vulkan/vulkan_texture.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;

/* type of the render texture */
typedef enum vulkan_render_texture_type_t
{
	VULKAN_RENDER_TEXTURE_TYPE_ALBEDO,		/* for rendering sRGB colors to the render texture */
	VULKAN_RENDER_TEXTURE_TYPE_COLOR,		/* for rendering linear color values to the render texture */
	VULKAN_RENDER_TEXTURE_TYPE_DEPTH,		/* for rendering depth values to the render texture */
	VULKAN_RENDER_TEXTURE_TYPE_ALBEDO_CUBE,	/* for rendering sRGB colors to the render texture having 6 faces (cube) */
	VULKAN_RENDER_TEXTURE_TYPE_COLOR_CUBE,	/* for rendering linear color values to the render texture having 6 faces (cube) */
	VULKAN_RENDER_TEXTURE_TYPE_DEPTH_CUBE	/* for rendering depth values to the render texture */
} vulkan_render_texture_type_t;

/* create info structure for creating vulkan_render_texture_t object */
typedef struct vulkan_render_texture_create_info_t
{
	u32 width;							/* width of the texture (width of each face in case of a cube render texture) */
	u32 height;							/* height of the texture, (hight of each face in case of a cube render texture) */
	u32 depth;							/* depth of the texture, (depth of each face in case of a cube render texture) */
	u8 channel_count; 					/* number of channels in the texture (for now it should be 4) */
	vulkan_render_texture_type_t type; 	/* type of the render texture */
} vulkan_render_texture_create_info_t;

typedef vulkan_texture_t vulkan_render_texture_t;

#define VULKAN_RENDER_TEXTURE(ptr) VULKAN_TEXTURE(ptr)
#define VULKAN_RENDER_TEXTURE_CONST(ptr) VULKAN_TEXTURE_CONST(Ptr)

BEGIN_CPP_COMPATIBLE

/*	
	description:
		creates a vulkan render texture (that can be used as a render target for a camera)
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created vulkan render texture object
 */
RENDERER_API vulkan_render_texture_t* vulkan_render_texture_create(vulkan_renderer_t* renderer, vulkan_render_texture_create_info_t* create_info);
RENDERER_API void vulkan_render_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_texture_create_info_t* create_info, vulkan_render_texture_t OUT texture);

/*
	description:
		destroys vulkan objects
	params:
		texture: pointer to the valid vulkan_render_texture_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_render_texture_destroy(vulkan_render_texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid vulkan_render_texture_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_render_texture_release_resources(vulkan_render_texture_t* texture);

END_CPP_COMPATIBLE
