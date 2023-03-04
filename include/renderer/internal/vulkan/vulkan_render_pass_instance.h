
#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>

/* structure to store create information for vulkan_render_pass_instance_t object */
typedef struct vulkan_render_pass_instance_create_info_t
{
	/* handle to the vulkan render pass object in the vulkan render pass pool */
	vulkan_render_pass_handle_t render_pass_handle;
} vulkan_render_pass_instance_create_info_t;

typedef struct vulkan_render_pass_instance_t
{
	/* pointer to the vulkan api context */
	vulkan_renderer_t* renderer;
	/* pointer to the render pass (vulkan_render_pass_t) to which this instance refers */
	vulkan_render_pass_t* render_pass;
	/* pointer to the render set (vulkan_descriptor_set_t); it is unique for every render pass instance */
	vulkan_descriptor_set_t render_set;
} vulkan_render_pass_instance_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_create(vulkan_renderer_t* renderer, vulkan_render_pass_instance_create_info_t* create_info);
RENDERER_API void vulkan_render_pass_instance_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_instance_create_info_t* create_info, vulkan_render_pass_instance_t OUT render_pass);
RENDERER_API void vulkan_render_pass_instance_destroy(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_instance_release_resources(vulkan_render_pass_instance_t* instance);

RENDERER_API void vulkan_render_pass_instance_set_clear_indirect(vulkan_render_pass_instance_t* instance, color_t color, float depth, VkClearValue* indirect_buffer);
RENDERER_API void vulkan_render_pass_instance_set_clear(vulkan_render_pass_instance_t* instance, color_t color, float depth);
RENDERER_API void vulkan_render_pass_instance_begin(vulkan_render_pass_instance_t* instance, u32 framebuffer_index, vulkan_camera_t* camera);
RENDERER_API void vulkan_render_pass_instance_end(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_instance_next(vulkan_render_pass_instance_t* instance);

typedef vulkan_render_pass_refresh_info_t vulkan_render_pass_instance_refresh_info_t;

RENDERER_API void vulkan_render_pass_instance_refresh(vulkan_render_pass_instance_t* instance, vulkan_render_pass_instance_refresh_info_t* info);

END_CPP_COMPATIBLE
