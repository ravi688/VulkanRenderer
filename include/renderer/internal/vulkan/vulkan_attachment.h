/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_attachment.h is a part of VulkanRenderer

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
#include <vulkan/vulkan.h>

#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_object.h>

typedef enum vulkan_attachment_next_pass_usage_t
{
	/* this attachment is of no use after completing this render pass and intermediate subpasses */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE = 0,
	/* if this attachment would be used as a Input attachment for the next subpasses */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT = 1,
	/* if this attachment would be used as a Sampled Image for the next renderpass */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED = 1 << 1,
	/* if this attachment would be used as a Present Image for the presentation engine */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT = 1 << 2
} vulkan_attachment_next_pass_usage_t;

typedef enum vulkan_attachment_type_t
{
	/* for color attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_COLOR = 0,
	/* for depth only attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_DEPTH = 1 << 1,
	/* for stencil only attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_STENCIL = 1 << 2,
	/* for depth & stencil attachment, format would be automatically decuded */
	VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL = VULKAN_ATTACHMENT_TYPE_DEPTH | VULKAN_ATTACHMENT_TYPE_STENCIL
} vulkan_attachment_type_t;

typedef struct vulkan_attachment_create_info_t
{
	/* width and height of the attachment */
	u32 width, height;
	/* type of the attachment to be created */
	vulkan_attachment_type_t type;
	/* usage of this attachment to be created */
	vulkan_attachment_next_pass_usage_t next_pass_usage;
	/* format of the attachment to be created */
	VkFormat format;
} vulkan_attachment_create_info_t;

/* NOTE: this should be in sync with vulkan_texture_t */
typedef struct vulkan_attachment_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	/* image for this attachment */
	vulkan_image_t image;
	/* image view for this attachment */
	vulkan_image_view_t image_view;

	/* sampler for this attachment if it has to be used in the next render pass, otherwise VK_NULL_HANDLE */
	VkSampler vo_sampler;

	/* type of the descriptor to which this attachment to be bound as vulkan_texture_t */
	VkDescriptorType vo_descriptor_type;

	/* copy of the vulkan image create info structure passed while calling vulkan_attachment_create or vulkan_attachment_create_no_alloc */
	vulkan_image_create_info_t* image_create_info;
	/* copy of the vulkan image view create info structure passed while calling vulkan_attachment_create or vulkan_attachment_create_no_alloc */
	vulkan_image_view_create_info_t* view_create_info;
} vulkan_attachment_t;

#define VULKAN_ATTACHMENT(ptr) VULKAN_OBJECT_UP_CAST(vulkan_attachment_t*, VULKAN_OBJECT_TYPE_ATTACHMENT, ptr)
#define VULKAN_ATTACHMENT_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_attachment_t*, VULKAN_OBJECT_TYPE_ATTACHMENT, ptr)


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_attachment_t* vulkan_attachment_new(memory_allocator_t* allocator);
RENDERER_API vulkan_attachment_t* vulkan_attachment_create(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info);
RENDERER_API void vulkan_attachment_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info, vulkan_attachment_t OUT attachment);
RENDERER_API void vulkan_attachment_destroy(vulkan_attachment_t* attachment);
RENDERER_API void vulkan_attachment_release_resources(vulkan_attachment_t* attachment);

/* structure to hold the recreate information for vulkan attachment object */
typedef struct vulkan_attachment_recreate_info_t
{
	/* width of the attachment */
	u32 width;
	/* height of the attachment */
	u32 height;
} vulkan_attachment_recreate_info_t;

/* recreates the attachment object */
RENDERER_API void vulkan_attachment_recreate(vulkan_attachment_t* attachment, vulkan_attachment_recreate_info_t* info);

/* getters */
RENDERER_API vulkan_image_t* vulkan_attachment_get_image(vulkan_attachment_t* attachment);
RENDERER_API vulkan_image_view_t* vulkan_attachment_get_image_view(vulkan_attachment_t* attachment);

END_CPP_COMPATIBLE