
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>		// u32

typedef struct vulkan_buffer_create_info_t
{
	void* data;							// pointer to the data filled inside the vulkan buffer's device memory,
										// if no data is available to copy/fill right now then it can be left NULL
	
	u32 count;							// entity count; number of entities to be stored of size "stride"
	u32 stride;							// stride; size of each entity

	u32 size;							// capacity of the buffer in bytes, 
										// if it is non-zero then "count" and "stride" would be ignored

	VkBufferUsageFlags vo_usage_flags;
	VkSharingMode vo_sharing_mode;
	VkMemoryPropertyFlags vo_memory_property_flags;
} vulkan_buffer_create_info_t;

typedef struct vulkan_buffer_t
{
	vulkan_renderer_t* renderer;				// pointer to the vulkan_renderer_t object
	VkBuffer vo_handle;
	VkDeviceMemory vo_memory;
	u32 size; 							// if create_info->size is zero then its value would be "count * stride" in bytes
	u32 stride;							// equals to create_info->stride
	u32 count; 							// equals to create_info->count
} vulkan_buffer_t;


BEGIN_CPP_COMPATIBLE

/* 
	description:
		initializes the memory of the vulkan_buffer_t object to zero
	params:
		buffer: pointer to vulkan_buffer_t object (stack allocated or heap allocated)
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_init(vulkan_buffer_t* buffer);

/* 
	description:
		allocates vulkan_buffer_t object on the heap and calls vulkan_buffer_init and then returns it
	params:
		nothing
	returns:
		pointer to newly created vulkan_buffer_t object on the heap
 */
RENDERER_API vulkan_buffer_t* vulkan_buffer_new();

/* 
	description:
		creates vulkan buffer (VkBuffer and VkDeviceMemory) object,
		it internally calls vulkan_buffer_new()
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_buffer_create_info_t object
	returns:
		pointer to newly created vulkan_buffer_t object on the heap
 */
RENDERER_API vulkan_buffer_t* vulkan_buffer_create(vulkan_renderer_t* renderer, vulkan_buffer_create_info_t* create_info);

/* 
	description:
		creates vulkan buffer (VkBuffer and VkDeviceMemory) object,
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_buffer_create_info_t object
		buffer: pointer to already allocated (stack or heap) and initialized (by calling vulkan_buffer_init) vulkan_buffer_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_buffer_create_info_t* create_info, vulkan_buffer_t* buffer);

/*
	description:
		destroys the vulkan objects (VkBuffer and VkDeviceMemory)
	params:
		buffer: pointer to a valid vulkan_buffer_t object (must be created by calling vulkan_buffer_create_no_alloc or vulkan_buffer_create)
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_destroy(vulkan_buffer_t* buffer);

/*
	description:
		releases heap allocated memory for vulkan_buffer_t object (which must have been allocated/created by vulkan_buffer_new or vulkan_buffer_create)
		if the vulkan_buffer_t object has been allocated on stack then don't call this function.
	params:
		buffer: pointer to a valid vulkan_buffer_t object (must be created by calling vulkan_buffer_create or allocated by vulkan_buffer_new)
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_release_resources(vulkan_buffer_t* buffer);

/*
	description:
		performs a copy operation (bytes pointed by "data" copied into the VkDeviceMemory)
	params:
		buffer: pointer to a valid vulkan_buffer_t object (must be created by calling vulkan_buffer_create_no_alloc or vulkan_buffer_create)
		buffer_offset: offset in destination buffer (VkDeviceMemory), must be less than buffer->size
		data: pointer to the bytes to be copied to the destination
		data_size: number of bytes to be copied to the destination
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_copy_data(vulkan_buffer_t* buffer, u32 buffer_offset, void* data, u32 data_size);

/*
	description:
		maps VkDeviceMemory to the host memory virtual address
	params:
		buffer: pointer to a valid vulkan_buffer_t object (must be created by calling vulkan_buffer_create_no_alloc or vulkan_buffer_create)
	returns:
		nothing
 */
RENDERER_API void* vulkan_buffer_map(vulkan_buffer_t* buffer);

/*
	description:
		unmaps VkDeviceMemory from the host memory virtual address
	params:
		buffer: pointer to a valid vulkan_buffer_t object (must be created by calling vulkan_buffer_create_no_alloc or vulkan_buffer_create)
	returns:
		nothing
 */
RENDERER_API void vulkan_buffer_unmap(vulkan_buffer_t* buffer);


END_CPP_COMPATIBLE
