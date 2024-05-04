/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#pragma once

#include <vulkan/vulkan.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h> // vulkan_descriptor_set_layout_t
#include <renderer/internal/vulkan/vulkan_attachment.h> 		// vulkan_attachment_type_t
#include <renderer/internal/vulkan/vulkan_handles.h> 		// vulkan_render_pass_handle_t, vulkan_shader_handle_t
#include <renderer/event.h>
#include <glslcommon/glsl_types.h>
#define VERTEX_ATTRIB(value, index) ((value) << ((index) * 5))
enum
{
	VERTEX_ATTRIB_POSITION = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 0),
	VERTEX_ATTRIB_NORMAL = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 1),
	VERTEX_ATTRIB_COLOR = VERTEX_ATTRIB(GLSL_TYPE_VEC3, 2),
	VERTEX_ATTRIB_TEXCOORD = VERTEX_ATTRIB(GLSL_TYPE_VEC2, 3)
};

typedef struct vulkan_shader_load_info_t
{
	/* 	file path of the file to be loaded 
		if NULL then 'data' will be used.
	 */
	const char* path;

	/* 	pointer to the continguous bytes loaded in the memory (preloaded)
		if NULL then path will be used to load the file,
		if both are not NULL then 'data' will get preference as it is faster to process
		already loaded data.
	 */
	const void* data;

	/*
		size of the memory block pointed by the 'data' pointer in bytes.
		if it is zero then path will be used to load the file,
		if both are 0 or NULL then any function consuming this struct will throw an error.
	 */
	u32 data_size;

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

typedef struct vulkan_shader_resource_description_t vulkan_shader_resource_description_t;
typedef struct vulkan_vertex_buffer_layout_description_t vulkan_vertex_buffer_layout_description_t;
typedef struct vulkan_render_pass_description_t vulkan_render_pass_description_t;
typedef struct vulkan_graphics_pipeline_description_t vulkan_graphics_pipeline_description_t;

typedef struct vulkan_shader_create_info_t
{
	/* MATERIAL SET BINDING DESCRIPTIONS & PUSH CONSTANT RANGE DESCRIPTIONS */
	vulkan_shader_resource_description_t* material_set_bindings;
	u32 material_set_binding_count;

	/* VERTEX ATTRIBUTE DESCRIPTIONS */
	vulkan_vertex_buffer_layout_description_t* vertex_infos;
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
	u32 pipeline_description_count;

} vulkan_shader_create_info_t;

typedef struct vulkan_graphics_pipeline_t vulkan_graphics_pipeline_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;

typedef struct vulkan_shader_subpass_t
{
	/* ptr to the pipeline layout object for this subpass  */
	vulkan_pipeline_layout_t* pipeline_layout;
	/* ptr to the graphics pipeline object for this subpass  */
	vulkan_graphics_pipeline_t* pipeline;
	/* deep copy of sub render set bindings passed with the subpass description */
	vulkan_shader_resource_description_t* sub_render_set_bindings;
	/* number of sub render set bindings */
	u32 sub_render_set_binding_count;
	// /* deep copy of input attachment references passed with the subpass description */
	// u32* input_attachments;
	// /* number of input attachments */
	// u32 input_attachment_count;
} vulkan_shader_subpass_t;

typedef struct vulkan_shader_render_pass_t
{
	vulkan_render_pass_handle_t handle;
	/* deep copy of render set bindings passed with the render pass description 
	 * dimensions: [render_set_binding_count] */
	vulkan_shader_resource_description_t* render_set_bindings;
	/* number of render set bindings */
	u32 render_set_binding_count;
	// /* deep copy of input attachment references passed with the subpass description */
	// u32* input_attachments;
	// /* number of input attachments */
	// u32 input_attachment_count;

	/* pointer to the list of vulkan shader subpasses */	
	vulkan_shader_subpass_t* subpasses;
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
	vulkan_shader_resource_description_t* material_set_bindings;
	u32 material_set_binding_count;

	/* for creating MATERIAL_SET for each of the materials deriving from this shader */
	vulkan_descriptor_set_layout_t material_set_layout;


	/* VERTEX ATTRIBUTE DESCRIPTIONS */
	/* For recreating the graphics pipelines on render window resize */
	vulkan_vertex_buffer_layout_description_t* vertex_infos;
	u32 vertex_info_count;

	/* PUSH CONSTANTS */
	vulkan_push_constant_t push_constant;

	/* handle to the graphics pipeline recreate subscription for render window resize */
	event_subscription_handle_t pipeline_recreate_handle;
	/* handle to the render pass refresh subscription for render window resize */
	event_subscription_handle_t render_pass_refresh_handle;
	/* handle to the rewrite render pass descriptors subscription for render window resize */
	event_subscription_handle_t rewrite_descriptors_handle;

	/* counts the number of passes run 
	 * initialized in vulkan_shader_create() to zero 
	 * incremented by one upon calling vulkan_shader_render_pass_is_next() */
	u32 pass_counter;
	/* counts the number of subpasses run for the render pass at [pass_counter] */
	u32 subpass_counter;

} vulkan_shader_t;

/*
	u32 vertex_info_count;
	vulkan_vertex_buffer_layout_description_t* vertex_infos;
	u32 push_constant_range_count;
	VkPushConstantRange* push_constant_ranges;
 */

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_shader_t* vulkan_shader_new(memory_allocator_t* allocator);
RENDERER_API vulkan_shader_t* vulkan_shader_create(vulkan_renderer_t* renderer, vulkan_shader_create_info_t* create_info);
RENDERER_API vulkan_shader_t* vulkan_shader_load(vulkan_renderer_t* renderer, vulkan_shader_load_info_t* file_info);
RENDERER_API void vulkan_shader_destroy(vulkan_shader_t* shader);
RENDERER_API void vulkan_shader_release_resources(vulkan_shader_t* shader);

/* logic functions */
RENDERER_API vulkan_graphics_pipeline_t* vulkan_shader_get_pipeline(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index);
RENDERER_API vulkan_pipeline_layout_t* vulkan_shader_get_pipeline_layout(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index);
/* returns handle to the previous render pass which ran, otherwise VULKAN_RENDER_PASS_HANDLE_INVALID */
RENDERER_API vulkan_render_pass_handle_t vulkan_shader_get_prev_pass_handle(vulkan_shader_t* shader);
/* returns true if the handle matches with the next pass's handle the shader exepects to be executed */
RENDERER_API bool vulkan_shader_render_pass_is_next(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle);
/* resets the pass_counter variable to zero for next rendering */
RENDERER_API void vulkan_shader_render_pass_counter_reset(vulkan_shader_t* shader);

END_CPP_COMPATIBLE
