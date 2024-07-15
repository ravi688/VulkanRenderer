/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_host_buffered_texture.h is a part of VulkanRenderer

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

#include <sge/defines.h>
#include <sge/internal/vulkan/vulkan_texture.h>
#include <sge/internal/vulkan/vulkan_host_buffered_buffer.h> // vulkan_host_buffered_buffer_t
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/buffer2d_view.h> // buffer2d_view_t
#include <bufferlib/buffer.h> // buffer_t

typedef struct vulkan_host_buffered_texture_create_info_t
{
	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count; 		// ignored if this is a depth texture, otherwise it must be in the range [1, 4]

	vulkan_texture_type_t type;						// type of the final vulkan texure i.e VULKAN_TEXTURE_TYPE_ALBEDO, VULKAN_TEXTURE_TYPE_NORMAL or VULKAN_TEXTURE_TYPE_CUBE
	vulkan_texture_usage_t usage; 					// runtime usage of the texture
	vulkan_texture_usage_t final_usage; 			// final usage of the texture
} vulkan_host_buffered_texture_create_info_t;

typedef struct vulkan_host_buffered_texture_t
{
	__VULKAN_OBJECT__;
	DERIVE_FROM(vulkan_texture_t);
	vulkan_renderer_t* renderer;
	/* linear buffer to store store pixel values on the host side and stage on the device (gpu) side */
	vulkan_host_buffered_buffer_t buffer;
	/* 2d view of the above linear buffer */
	buffer2d_view_t view;
} vulkan_host_buffered_texture_t;

#define VULKAN_HOST_BUFFERED_TEXTURE(ptr) VULKAN_OBJECT_UP_CAST(vulkan_host_buffered_texture_t*, VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE, ptr)
#define VULKAN_HOST_BUFFERED_TEXTURE_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_host_buffered_texture_t*, VULKAN_OBJECT_TYPE_HOST_BUFFERED_TEXTURE, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_new(memory_allocator_t* allocator);
SGE_API vulkan_host_buffered_texture_t* vulkan_host_buffered_texture_create(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info);
SGE_API void vulkan_host_buffered_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_host_buffered_texture_create_info_t* create_info, vulkan_host_buffered_texture_t OUT texture);
SGE_API void vulkan_host_buffered_texture_destroy(vulkan_host_buffered_texture_t* texture);
SGE_API void vulkan_host_buffered_texture_release_resources(vulkan_host_buffered_texture_t* texture);

/* returns pointer to the 2d view object used over the host side linear buffer */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer2d_view_t* vulkan_host_buffered_texture_get_view(vulkan_host_buffered_texture_t* texture)
{ 
	return &texture->view;
}
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer2d_view_t* vulkan_host_buffered_texture_get_view_readonly(vulkan_host_buffered_texture_t* texture)
{ 
	return vulkan_host_buffered_texture_get_view(texture); 
}
/* returns pointer to the host side linear buffer object */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer_t* vulkan_host_buffered_texture_get_host_buffer(vulkan_host_buffered_texture_t* texture) 
{ 
	return vulkan_host_buffered_buffer_get_host_buffer(&texture->buffer);
}
/* usage of this function should be preferred over the above function as this one doesn't mark the device side VkBuffer object outdated.
 * Also, the contents of the queried host buffer must not be modified as it would be meant only for reading. */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE buffer_t* vulkan_host_buffered_texture_get_host_buffer_readonly(vulkan_host_buffered_texture_t* texture) 
{ 
	return vulkan_host_buffered_buffer_get_host_buffer_readonly(&texture->buffer);
}
/* returns pointer to the underlying vulkan_texture_t object */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_texture_t* vulkan_host_buffered_texture_get_texture(vulkan_host_buffered_texture_t* texture)
{
	return BASE(texture);
}

/* however, currently it is not used anywehere */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_host_buffered_texture_set_dirty(vulkan_host_buffered_texture_t* texture, bool is_dirty)
{
	vulkan_host_buffered_buffer_set_dirty(&texture->buffer, is_dirty);
}

/* flushes the host side buffer to the device (gpu) side VkDeviceMemory 
 * returns true if either the contents are updated or the internal VkImage, VkDeviceMemory, and VkBuffer has been recreated */
SGE_API bool vulkan_host_buffered_texture_commit(vulkan_host_buffered_texture_t* texture, bool OUT is_resized);


END_CPP_COMPATIBLE
