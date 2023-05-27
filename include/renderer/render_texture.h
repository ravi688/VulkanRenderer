/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_texture.h is a part of VulkanRenderer

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


#pragma once

typedef struct renderer_t renderer_t;

#if defined(RENDERER_VULKAN_DRIVER)
	typedef struct vulkan_texture_t vulkan_texture_t;
	typedef vulkan_texture_t vulkan_render_texture_t;
	typedef vulkan_render_texture_t render_texture_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_texture_t opengl_texture_t;
	typedef opengl_texture_t opengl_render_texture_t;
	typedef opengl_render_texture_t render_texture_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_texture_t directx_texture_t;
	typedef directx_texture_t directx_render_texture_t;
	typedef directx_render_texture_t render_texture_t;
#elif defined(RENDERER_METAL_DRIVER)
	typedef struct metal_texture_t metal_texture_t;
	typedef metal_texture_t metal_render_texture_t;
	typedef metal_render_texture_t render_texture_t;
#endif

#include <renderer/defines.h>

/* type of the render texture */
typedef enum render_texture_type_t
{
	RENDER_TEXTURE_TYPE_ALBEDO,		/* for rendering sRGB colors to the render texture */
	RENDER_TEXTURE_TYPE_COLOR,		/* for rendering linear color values to the render texture */
	RENDER_TEXTURE_TYPE_DEPTH,		/* for rendering depth values to the render texture */
	RENDER_TEXTURE_TYPE_ALBEDO_CUBE,	/* for rendering sRGB colors to the render texture having 6 faces (cube) */
	RENDER_TEXTURE_TYPE_COLOR_CUBE,	/* for rendering linear color values to the render texture having 6 faces (cube) */
	RENDER_TEXTURE_TYPE_DEPTH_CUBE	/* for rendering depth values to the render texture */
} render_texture_type_t;

/* create info structure for creating vulkan_render_texture_t object */
typedef struct render_texture_create_info_t
{
	u32 width;							/* width of the texture (width of each face in case of a cube render texture) */
	u32 height;							/* height of the texture, (hight of each face in case of a cube render texture) */
	u32 depth;							/* depth of the texture, (depth of each face in case of a cube render texture) */
	u8 channel_count; 					/* number of channels in the texture (for now it should be 4) */
	render_texture_type_t type; 		/* type of the render texture */
} render_texture_create_info_t;

BEGIN_CPP_COMPATIBLE

/*	
	description:
		creates a render texture (that can be used as a render target for a camera)
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created render texture object
 */
RENDERER_API render_texture_t* render_texture_create(renderer_t* renderer, render_texture_create_info_t* create_info);
RENDERER_API void render_texture_create_no_alloc(renderer_t* renderer, render_texture_create_info_t* create_info, render_texture_t OUT texture);

/*
	description:
		destroys internal API objects
	params:
		texture: pointer to the valid render_texture_t object
	returns:
		nothing
 */
RENDERER_API void render_texture_destroy(render_texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid render_texture_t object
	returns:
		nothing
 */
RENDERER_API void render_texture_release_resources(render_texture_t* texture);

END_CPP_COMPATIBLE
