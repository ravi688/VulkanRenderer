#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>  // buffer_t
#include <renderer/internal/vulkan/vulkan_render_pass_description.h> // vulkan_attachment_reference_type_t

typedef struct vulkan_render_pass_description_builder_t
{
	memory_allocator_t* allocator;
	/* array of vulkan_render_pass_description_t objects */
	buffer_t description_array;
	u32 bind_index;
} vulkan_render_pass_description_builder_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create(memory_allocator_t* allocator);
RENDERER_API vulkan_render_pass_description_builder_t* vulkan_render_pass_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_render_pass_description_t* descriptions, u32 description_count);
RENDERER_API void vulkan_render_pass_description_builder_destroy(vulkan_render_pass_description_builder_t* builder);

RENDERER_API void vulkan_render_pass_description_builder_add(vulkan_render_pass_description_builder_t* builder, u32 count);
RENDERER_API void vulkan_render_pass_description_builder_bind(vulkan_render_pass_description_builder_t* builder, u32 index);

RENDERER_API vulkan_render_pass_description_t* vulkan_render_pass_description_builder_get(vulkan_render_pass_description_builder_t* builder);
RENDERER_API u32 vulkan_render_pass_description_builder_get_count(vulkan_render_pass_description_builder_t* builder);


RENDERER_API void vulkan_render_pass_description_builder_begin_pass(vulkan_render_pass_description_builder_t* builder, vulkan_render_pass_type_t type);
RENDERER_API void vulkan_render_pass_description_builder_add_input(vulkan_render_pass_description_builder_t* builder, glsl_type_t type, u32 index, u32 binding);
RENDERER_API void vulkan_render_pass_description_builder_add_attachment(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_type_t type);

RENDERER_API void vulkan_render_pass_description_builder_begin_subpass(vulkan_render_pass_description_builder_t* builder, u32 pipeline_index);
RENDERER_API void vulkan_render_pass_description_builder_add_attachment_reference(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_reference_type_t type, u32 reference, u32 binding);
RENDERER_API void vulkan_render_pass_description_builder_end_subpass(vulkan_render_pass_description_builder_t* builder);

RENDERER_API void vulkan_render_pass_description_builder_add_subpass_dependency(vulkan_render_pass_description_builder_t* builder, VkSubpassDependency* dependency);
RENDERER_API void vulkan_render_pass_description_builder_end_pass(vulkan_render_pass_description_builder_t* builder);

END_CPP_COMPATIBLE
