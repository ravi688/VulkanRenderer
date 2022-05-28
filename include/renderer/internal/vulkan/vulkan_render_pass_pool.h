
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

typedef buf_ucount_t vulkan_render_pass_handle_t;
#define VULKAN_RENDER_PASS_HANDLE_INVALID (~0ULL)

typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_render_pass_create_info_t vulkan_render_pass_create_info_t;

typedef struct vulkan_render_pass_pool_slot_t
{
	/* ptr to the vulkan_render_pass_t object */
	vulkan_render_pass_t* render_pass;
	/* for comparison */
	vulkan_render_pass_create_info_t* create_info;
	/* handle to this slot in the pool/library */
	vulkan_render_pass_handle_t handle;
} vulkan_render_pass_pool_slot_t;

typedef struct vulkan_render_pass_pool_t
{
	vulkan_renderer_t* renderer;
	BUFFER relocation_table; 				// relocation table
	BUFFER slots; 							// list of vulkan_render_pass_pool_slot_t
} vulkan_render_pass_pool_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_new();
RENDERER_API vulkan_render_pass_pool_t* vulkan_render_pass_pool_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_render_pass_pool_destroy(vulkan_render_pass_pool_t* pool);
RENDERER_API void vulkan_render_pass_pool_release_resources(vulkan_render_pass_pool_t* pool);

/* logic functions */
RENDERER_API vulkan_render_pass_handle_t vulkan_render_pass_pool_create_pass(vulkan_render_pass_pool_t* pool, vulkan_render_pass_create_info_t* create_info);
RENDERER_API vulkan_render_pass_t* vulkan_render_pass_pool_getH(vulkan_render_pass_pool_t* pool, vulkan_render_pass_handle_t handle);


END_CPP_COMPATIBLE
