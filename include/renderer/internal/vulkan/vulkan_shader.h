
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h> // vulkan_descriptor_set_layout_t
#include <renderer/internal/vulkan/vulkan_attachment.h> 		// vulkan_attachment_type_t
#include <renderer/internal/vulkan/vulkan_handles.h> 		// vulkan_render_pass_handle_t, vulkan_shader_handle_t

#include <shader_compiler/settings_parser.h> 		// GraphicsPipeline

#include <renderer/glsl_types.h>
#define VERTEX_ATTRIB(value, index) ((value) << ((index) * 5))
enum
{
	VERTEX_ATTRIB_POSITION = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 0),
	VERTEX_ATTRIB_NORMAL = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 1),
	VERTEX_ATTRIB_COLOR = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 2),
	VERTEX_ATTRIB_TEXCOORD = VERTEX_ATTRIB(GLSL_TYPE_VEC2, 3)
};

/* NOTE: this must be in sync with vulkan_shader_module_create_info_t */
typedef struct vulkan_spirv_code_t
{
	/* ptr to the SPIRV binary */
	void* spirv;
	/* length of the SPIRV binary in bytes */
	u32 length;
	/* type of the shader module */
	vulkan_shader_type_t type;

} vulkan_spirv_code_t;

typedef GraphicsPipeline GraphicsPipelineSettings;

typedef struct vulkan_graphics_pipeline_description_t
{
	/* fixed function settings for this graphics pipeline */
	GraphicsPipelineSettings* settings;

	/* list of spirv shaders (just bytes and lengths) */
	vulkan_spirv_code_t* spirv_codes;
	u32 spirv_code_count;

} vulkan_graphics_pipeline_description_t;

typedef struct vulkan_shader_resource_descriptor_t vulkan_shader_resource_descriptor_t;

/* NOTE: sub_render_set_binding_count is always equal to input_attachment_count */
typedef struct vulkan_subpass_description_t
{
	/* SUB RENDER SET BINDING DESCRIPTIONS */
	vulkan_shader_resource_descriptor_t* sub_render_set_bindings;
	u32 sub_render_set_binding_count; 	// must be equal to input_attachment_count

	/* graphics pipeline description */
	u32 pipeline_description_index;

	/* ATTACHMENT POINTERS */
	
	/* list of color attachments */
	u32* color_attachments;
	u32 color_attachment_count;

	/* list of input attachments */
	u32* input_attachments;
	u32 input_attachment_count; 	// must be equal to sub_render_set_binding_count

	/* index of the depth stencil attachment */
	u32 depth_stencil_attachment;

	/* list of preserve attachments */
	u32* preserve_attachments;
	u32 preserve_attachment_count;

} vulkan_subpass_description_t;

typedef enum vulkan_render_pass_type_t
{
	VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER,
	VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET
	
} vulkan_render_pass_type_t;

/* NOTE: render_set_binding_count is always equal to input_attachment_count */
typedef struct vulkan_render_pass_description_t
{
	/* RENDER SET BINDING DESCRIPTIONS */
	vulkan_shader_resource_descriptor_t* render_set_bindings;
	u32 render_set_binding_count; 	// must be equal to input_attachment_count

	/* SUBPASS DESCRIPTIONS */
	/* ptr to the list of subpass descriptions */
	vulkan_subpass_description_t* subpass_descriptions;
	/* number of subpasses in this render pass */
	union
	{	
		u32 subpass_description_count;
		u32 subpass_count;
	};

	/* ATTACHMENTS */
	vulkan_attachment_type_t* attachments;
	u32 attachment_count;

	/* INPUTS */

	/* list of input attachments from the previous render pass */
	u32* input_attachments;
	u32 input_attachment_count; 	// must be equal to render_set_binding_count

	/* type of this render pass */
	vulkan_render_pass_type_t type;

} vulkan_render_pass_description_t;

typedef struct vulkan_shader_load_info_t
{
	/* file path of the file to be loaded */
	const char* path;
	/*
		if true, then shader would look for the vertex attribute descriptions in the file to be loaded
		if false, then shader would decode the endcoded vertex attribute descriptions in 64 bit numbers
 	 */
	bool is_vertex_attrib_from_file;

	/* each 64 bit integer encodes vertex attribute descriptions for a vertex binding ( per vertex ) */
	u64* per_vertex_attribute_bindings;
	/* number of vertex bindings for per vertex input rate */
	u32 per_vertex_attribute_binding_count;

	/* each 64 bit integers encodes vertex attribute descriptions for a vertex binding ( per instance ) */
	u64* per_instance_attribute_bindings;
	/* number of vertex bindings for per instance input rate */
	u32 per_instance_attribute_binding_count;

} vulkan_shader_load_info_t;

typedef struct vulkan_shader_create_info_t
{
	/* MATERIAL SET BINDING DESCRIPTIONS & PUSH CONSTANT RANGE DESCRIPTIONS */
	vulkan_shader_resource_descriptor_t* material_set_bindings;
	u32 material_set_binding_count;

	/* VERTEX ATTRIBUTE DESCRIPTIONS */
	vulkan_vertex_info_t* vertex_infos;
	u32 vertex_info_count;

	/* RENDER PASS DESCRIPTIONS */
	/* ptr to the list of render pass descriptions */
	vulkan_render_pass_description_t* render_pass_descriptions;
	/* number of render passes in this shader */
	union
	{
		u32 render_pass_description_count;
		u32 render_pass_count;
	};

	/* PIPELINES */
	vulkan_graphics_pipeline_description_t* pipeline_descriptions;

} vulkan_shader_create_info_t;

typedef struct vulkan_graphics_pipeline_t vulkan_graphics_pipeline_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;

typedef struct vulkan_shader_render_pass_t
{
	/* handle to the render pass instance into the render pass pool */
	vulkan_render_pass_handle_t handle;
	/* ptr to the list of vulkan_pipeline_layout_t objects for each sub pass */
	vulkan_pipeline_layout_t* pipeline_layouts;
	/* ptr to the list of vulkan_graphics_pipeline_layout_t objects for each sub pass */
	vulkan_graphics_pipeline_t* pipelines;
	/* number of subpasses in this render pass */
	u32 subpass_count;
} vulkan_shader_render_pass_t;

typedef struct vulkan_push_constant_t
{
	/* TODO: there is no need to preserve this list, once all the pipeline layouts has been created we can delete it */
	/* list of push constant ranges */
	VkPushConstantRange* ranges;
	u32 range_count;

	/* ptr the CPU side memory buffer for all the push constant ranges */
	void* buffer;
	/* size of the CPU side memory buffer for all the push constant ranges */
	u32 buffer_size;

} vulkan_push_constant_t;

typedef struct vulkan_shader_t
{
	vulkan_renderer_t* renderer;

	vulkan_shader_handle_t handle;

	/* RENDER PASS HANDLES, PIPELINE LAYOUTS & GRAPHICS PIPELINES */
	vulkan_shader_render_pass_t* render_passes;
	u32 render_pass_count;

	/* MATERIAL SET */
	/*
		ptr to the list of material set bindings (inclusive of push constants)
		1. for creating material_set_layout
		2. for manipulating material properties (bindings)
	*/
	vulkan_shader_resource_descriptor_t* material_set_bindings;
	u32 material_set_binding_count;

	/* for creating MATERIAL_SET for each of the materials deriving from this shader */
	vulkan_descriptor_set_layout_t material_set_layout;


	/* VERTEX ATTRIBUTE DESCRIPTIONS */
	/* For recreating the graphics pipelines on render window resize */
	vulkan_vertex_info_t* vertex_infos;
	u32 vertex_info_count;

	/* PUSH CONSTANTS */
	vulkan_push_constant_t push_constant;

	/* ptr to the currently bound pipeline */
	vulkan_graphics_pipeline_t* current_bound_pipeline;

} vulkan_shader_t;

/*
	u32 vertex_info_count;
	vulkan_vertex_info_t* vertex_infos;
	u32 push_constant_range_count;
	VkPushConstantRange* push_constant_ranges;
 */

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_shader_t* vulkan_shader_new();
RENDERER_API vulkan_shader_t* vulkan_shader_create(vulkan_renderer_t* renderer, vulkan_shader_create_info_t* create_info);
RENDERER_API vulkan_shader_t* vulkan_shader_load(vulkan_renderer_t* renderer, vulkan_shader_load_info_t* file_info);
RENDERER_API void vulkan_shader_destroy(vulkan_shader_t* shader);
RENDERER_API void vulkan_shader_release_resources(vulkan_shader_t* shader);

/* logic functions */
RENDERER_API vulkan_graphics_pipeline_t* vulkan_shader_get_pipeline(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index);
RENDERER_API vulkan_pipeline_layout_t* vulkan_shader_get_pipeline_layout(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index);

END_CPP_COMPATIBLE
