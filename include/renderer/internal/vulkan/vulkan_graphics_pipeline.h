
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_types.h>

typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;
typedef struct GraphicsPipeline GraphicsPipeline;
typedef GraphicsPipeline GraphicsPipelineSettings;
typedef struct vulkan_shader_module_t vulkan_shader_module_t;
typedef struct vulkan_render_pass_t vulkan_render_pass_t;

typedef struct vulkan_graphics_pipeline_create_info_t
{
	/* pipeline layout for this pipeline */
	vulkan_pipeline_layout_t* layout;
	
	/* fixed function settings for this pipeline */
	GraphicsPipelineSettings* settings;
	
	/* list of shader modues for this pipeline */
	vulkan_shader_module_t* shader_modules;
	u32 shader_module_count;

	/* list of vertex binding descriptions supported by this pipeline  */
	vulkan_vertex_info_t* vertex_attribute_bindings;
	u32 vertex_attribute_binding_count;

	/* render pass for this pipeline */
	vulkan_render_pass_t* render_pass;
	/* subpass index for this pipeline */
	u32 subpass_index;
	
} vulkan_graphics_pipeline_create_info_t;

typedef struct vulkan_graphics_pipeline_t
{
	vulkan_renderer_t* renderer;
	VkPipeline vo_handle;
} vulkan_graphics_pipeline_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new();
RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info);
RENDERER_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t OUT pipeline);
RENDERER_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline);
RENDERER_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline);

/* logic functions */
RENDERER_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline);

END_CPP_COMPATIBLE
