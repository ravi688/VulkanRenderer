
#pragma once

#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>

typedef struct vulkan_render_pass_instance_t
{
	vulkan_render_pass_t* render_pass;			// ptr to the actual render pass
	vulkan_descriptor_set_t** set;				// render set for each sub pass
} vulkan_render_pass_instance_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_new(memory_allocator_t* allocator);
RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_create(vulkan_render_pass_t* pass);
RENDERER_API void vulkan_render_pass_instance_destroy(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_release_resources(vulkan_render_pass_instance_t* instance);


RENDERER_API void vulkan_render_pass_instance_begin(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_instance_end(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_instance_next(vulkan_render_pass_instance_t* instance);
RENDERER_API u32 vulkan_render_pass_instance_get_sub_pass_count(vulkan_render_pass_instance_t* instance);
RENDERER_API void vulkan_render_pass_instance_bind_set(vulkan_render_pass_instance_t* instance, u32 sub_pass_index);

END_CPP_COMPATIBLE
