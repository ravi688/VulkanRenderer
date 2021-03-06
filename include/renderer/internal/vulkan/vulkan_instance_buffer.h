
#pragma once

#include <renderer/internal/vulkan/vulkan_buffer.h>
#include <renderer/multi_buffer.h>

typedef struct vulkan_renderer_t vulkan_renderer_t;

typedef struct vulkan_instance_buffer_create_info_t
{
	u32 stride;			// element size
	u32 capacity;		// initial capacity, zero is also a valid capacity here
} vulkan_instance_buffer_create_info_t;

typedef struct vulkan_instance_buffer_t
{
	vulkan_renderer_t* renderer;
	vulkan_buffer_t device_buffer;			// buffer in the device memory
	multi_buffer_t host_buffer;				// buffer in the host memory
	bool has_device_buffer; 				// initially only host-side buffer is created and used; so it would be false if *_commit never called
} vulkan_instance_buffer_t;

BEGIN_CPP_COMPATIBLE

// constructors and destructors
RENDERER_API void vulkan_instance_buffer_create(vulkan_renderer_t* renderer, vulkan_instance_buffer_create_info_t* create_info, vulkan_instance_buffer_t* out_instance_buffer);
RENDERER_API void vulkan_instance_buffer_destroy(vulkan_instance_buffer_t* instance_buffer);
RENDERER_API void vulkan_instance_buffer_release_resources(vulkan_instance_buffer_t* instance_buffer);

// getters
RENDERER_API multi_buffer_t* vulkan_instance_buffer_get_host_buffer(vulkan_instance_buffer_t* instance_buffer);
RENDERER_API vulkan_buffer_t* vulkan_instance_buffer_get_device_buffer(vulkan_instance_buffer_t* instance_buffer);

// logic functions

// returns true when this buffer should be binded in the pipeline, otherwise false
RENDERER_API bool vulkan_instance_buffer_commit(vulkan_instance_buffer_t* instance_buffer);

END_CPP_COMPATIBLE
