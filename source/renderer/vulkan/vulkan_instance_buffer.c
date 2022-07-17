#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_instance_buffer.h>
#include <renderer/assert.h>
#include <memory.h> 			// memcpy

#ifndef GLOBAL_DEBUG
#define check_pre_condition(instance_buffer)
#else
	static void check_pre_condition(vulkan_instance_buffer_t* instance_buffer);
#endif /*GLOBAL_DEBUG*/

// constructors and destructors
RENDERER_API void vulkan_instance_buffer_create(vulkan_renderer_t* renderer, vulkan_instance_buffer_create_info_t* create_info, vulkan_instance_buffer_t* out_instance_buffer)
{
	assert(out_instance_buffer != NULL);
	multi_buffer_create(create_info->stride, create_info->capacity, &out_instance_buffer->host_buffer);
	vulkan_buffer_init(&out_instance_buffer->device_buffer);
	out_instance_buffer->renderer = renderer;
	out_instance_buffer->has_device_buffer = false;
}

RENDERER_API void vulkan_instance_buffer_destroy(vulkan_instance_buffer_t* instance_buffer)
{
	check_pre_condition(instance_buffer);
	if(instance_buffer->has_device_buffer)
		vulkan_buffer_destroy(&instance_buffer->device_buffer);
	multi_buffer_clear(&instance_buffer->host_buffer);
}

RENDERER_API void vulkan_instance_buffer_release_resources(vulkan_instance_buffer_t* instance_buffer)
{
	check_pre_condition(instance_buffer);

	// not need to call _release_resources for instance_buffer->device_buffer because it is already inlined inside vulkan_instance_buffer_t object
	// if(instance_buffer->has_device_buffer)
	// 	vulkan_buffer_release_resources(&instance_buffer->device_buffer);
	multi_buffer_free(&instance_buffer->host_buffer);
}

// getters
RENDERER_API multi_buffer_t* vulkan_instance_buffer_get_host_buffer(vulkan_instance_buffer_t* instance_buffer)
{
	check_pre_condition(instance_buffer);
	return &instance_buffer->host_buffer;
}

RENDERER_API vulkan_buffer_t* vulkan_instance_buffer_get_device_buffer(vulkan_instance_buffer_t* instance_buffer)
{
	check_pre_condition(instance_buffer);
	return &instance_buffer->device_buffer;
}

// logic functions
RENDERER_API bool vulkan_instance_buffer_commit(vulkan_instance_buffer_t* instance_buffer)
{
	check_pre_condition(instance_buffer);
	multi_buffer_t* host_buffer = &instance_buffer->host_buffer;
	vulkan_buffer_t* device_buffer = &instance_buffer->device_buffer;
	u32 sub_buffer_count = multi_buffer_get_sub_buffer_count(host_buffer);

	u32 count = 0;
	for(u32 i = 0; i < sub_buffer_count; i++)
		count += sub_buffer_get_count(host_buffer, i);
	
	// here count is capacity of the device_buffer
	if(count > device_buffer->count)
	{
		if(instance_buffer->has_device_buffer)
			vulkan_buffer_destroy(device_buffer);
		vulkan_buffer_create_info_t create_info = 
		{
			.stride = buf_get_element_size(&host_buffer->buffer),
			.count = count,
			.vo_usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	
		};
		vulkan_buffer_create_no_alloc(instance_buffer->renderer, &create_info, device_buffer);
		instance_buffer->has_device_buffer = true;
	}

	if(!instance_buffer->has_device_buffer) return false;

	device_buffer->count = count;
	device_buffer->size = device_buffer->stride * count;
	if(device_buffer->count == 0) return false;
	
	// copy the elements from all the sub buffers to the device buffer contiguously (no gaps, as there could be due to capacities of the sub buffers)
	void* ptr = vulkan_buffer_map(device_buffer);
	for(u32 i = 0; i < sub_buffer_count; i++)
	{
		u32 num_bytes = sub_buffer_get_count(host_buffer, i) * device_buffer->stride;
		if(num_bytes == 0) continue;
		memcpy(ptr, sub_buffer_get_ptr(host_buffer, i), num_bytes);
		ptr += num_bytes;
	}
	vulkan_buffer_unmap(device_buffer);
	return true;
}



#ifdef GLOBAL_DEBUG
static void check_pre_condition(vulkan_instance_buffer_t* instance_buffer)
{
	assert(instance_buffer != NULL);
}
#endif /*GLOBAL_DEBUG*/

