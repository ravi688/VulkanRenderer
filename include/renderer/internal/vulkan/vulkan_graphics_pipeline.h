
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>

typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;
typedef struct vulkan_shader_module_t vulkan_shader_module_t;
typedef struct vulkan_spirv_code_t vulkan_spirv_code_t;;
typedef struct vulkan_render_pass_t vulkan_render_pass_t;
typedef struct vulkan_graphics_pipeline_settings_t vulkan_graphics_pipeline_settings_t;

typedef struct vulkan_graphics_pipeline_create_info_t
{
	/* pipeline layout for this pipeline */
	vulkan_pipeline_layout_t* layout;
	
	/* fixed function settings for this pipeline */
	vulkan_graphics_pipeline_settings_t* settings;
	
	/* list of spirv codes for this pipeline */
	vulkan_spirv_code_t* spirv_codes;
	u32 spirv_code_count;

	/* list of vertex binding descriptions supported by this pipeline  */
	vulkan_vertex_buffer_layout_description_t* vertex_attribute_bindings;
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

	/* CACHED VALUES TO BE USED WHILE REFRESHING THE GFX PIPELINE */

	/* list of shader modules */
	vulkan_shader_module_t* shader_modules;
	union
	{
		/* number of shader modules (identical to spirv code count and shader stage count ) */
		u32 shader_module_count;
		u32 spirv_code_count;
		u32 shader_stage_count;
	};
	/* list of shader stages */
	VkPipelineShaderStageCreateInfo* vo_shader_stages;
	/* list of vertex input binding descriptions */
	VkVertexInputBindingDescription* vo_binding_descriptions;
	/* number of vertex input binding descriptions */
	u32 binding_description_count;
	/* list of vertex attribute descriptions */
	VkVertexInputAttributeDescription* vo_attribute_descriptions;
	/* number of vertex attribute descriptions */
	u32 attribute_description_count;
	/* cache for the settings; as the gfx pipeline might be refreshed on render window resize */
	vulkan_graphics_pipeline_settings_t* settings;

	/* list of viewports */
	VkViewport* vo_viewports;
	/* number of viewports */
	u32 viewport_count;

	/* list of scissors */
	VkRect2D* vo_scissors;
	/* number of scissors */
	u32 scissor_count;

	/* pointer to the pipeline layout this gfx pipeline created with */
	vulkan_pipeline_layout_t* layout;
	/* pointer to the render pass this gfx pipelien created with */
	vulkan_render_pass_t* render_pass;
	/* index of the subpass for which this gfx pipeline would work */
	u32 subpass_index;

} vulkan_graphics_pipeline_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_new(memory_allocator_t* allocator);
RENDERER_API vulkan_graphics_pipeline_t* vulkan_graphics_pipeline_create(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info);
RENDERER_API void vulkan_graphics_pipeline_create_no_alloc(vulkan_renderer_t* renderer, vulkan_graphics_pipeline_create_info_t* create_info, vulkan_graphics_pipeline_t OUT pipeline);
RENDERER_API void vulkan_graphics_pipeline_destroy(vulkan_graphics_pipeline_t* pipeline);
RENDERER_API void vulkan_graphics_pipeline_release_resources(vulkan_graphics_pipeline_t* pipeline);

/* logic functions */
RENDERER_API void vulkan_graphics_pipeline_bind(vulkan_graphics_pipeline_t* pipeline);

/* info struct to store the informations to be refreshed such as window's width and height */
typedef struct vulkan_graphics_pipeline_refresh_info_t
{
	/* width of the render window */
	u32 width;
	/* height of the render window */
	u32 height;
} vulkan_graphics_pipeline_refresh_info_t;

/* refresh is just like recreation of the gfx pipeline with create_noalloc version 
 * but this is much more efficient as it uses the cached values that are not supposed to refreshed generally */
RENDERER_API void vulkan_graphics_pipeline_refresh(vulkan_graphics_pipeline_t* pipeline, vulkan_graphics_pipeline_refresh_info_t* info);

END_CPP_COMPATIBLE
