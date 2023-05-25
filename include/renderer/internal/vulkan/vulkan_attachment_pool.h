/* TODO: Part of TID-49 
 * for now lets merge the branc TID-49/RenderGraph-Optimization */
#pragma once

typedef struct vulkan_renderer_t vulkan_renderer_t;

/* structure to hold vulkan attachment pool create information(s) */
typedef struct vulkan_attachment_pool_create_info_t
{

} vulkan_attachment_pool_create_info_t;

/* creates framebuffer attachmens upon request and minimizes the number of vulkan attachments (VkImagView(s)) */
typedef struct vulkan_attachment_pool_t
{
	
} vulkan_attachment_pool_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_attachment_pool_t* vulkan_attachment_pool_new(memory_allocator_t* allocator);
RENDERER_API vulkan_attachment_pool_t* vulkan_attachment_pool_create(vulkan_renderer_t* renderer, vulkan_attachment_pool_create_info_t* create_info);
RENDERER_API void vulkan_attachment_pool_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_pool_create_info_t* create_info, vulkan_attachment_pool_t OUT pool);
RENDERER_API void vulkan_attachment_pool_destroy(vulkan_attachment_pool_t* pool);
RENDERER_API void vulkan_attachment_pool_release_resources(vulkan_attachment_pool_t* pool);

END_CPP_COMPATIBLE
