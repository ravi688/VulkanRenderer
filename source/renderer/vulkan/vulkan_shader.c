/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <renderer/internal/vulkan/vulkan_shader_loader.h>
#include <renderer/internal/vulkan/vulkan_render_pass_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_subpass_create_info_builder.h>
#include <disk_manager/file_reader.h>
#include <common/binary_reader.h>
#include <renderer/assert.h>
#include <renderer/debug.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/dictionary.h>
#include <renderer/render_window.h>

#include <string.h> 			// for strlen()
#include <math.h> 				// for log2(), and ceil()

RENDERER_API vulkan_shader_t* vulkan_shader_new(memory_allocator_t* allocator)
{
	vulkan_shader_t* shader = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER, vulkan_shader_t);
	memzero(shader, vulkan_shader_t);
	VULKAN_OBJECT_INIT(shader, VULKAN_OBJECT_TYPE_SHADER, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return shader;
}

static vulkan_vertex_buffer_layout_description_t decode_vulkan_vertex_info(memory_allocator_t* allocator, u64 packed, u16 location_offset, VkVertexInputRate input_rate)
{
	u32 bits_per_type = ceil(log2((double)GLSL_TYPE_MAX_NON_OPAQUE));
	assert(((2UL << bits_per_type) >= GLSL_TYPE_MAX_NON_OPAQUE));

	u64 bits_mask = BIT_MASK32(bits_per_type);

	BUFFER formats = memory_allocator_buf_new(allocator, VkFormat);
	BUFFER offsets = memory_allocator_buf_new(allocator, u32);
	BUFFER locations = memory_allocator_buf_new(allocator, u16);

	u32 offset = 0;
	while(packed != 0)
	{
		u64 type = packed & bits_mask;
		packed >>= bits_per_type;
		if(type == 0)
			continue;

		VkFormat vulkan_format = vkformatof_glsl_type(type);
		u32 align = alignof_glsl_type(type);
		u32 field_offset = ((align - (offset % align)) % align) + offset;
		switch(type)
		{
			case GLSL_TYPE_MAT3:
				LOG_FETAL_ERR("Vertex attribute decode error: GLSL_MAT3 isn't supported yet!\n");
			break;

			case GLSL_TYPE_MAT4:
				LOG_FETAL_ERR("Vertex attribute decode error: GLSL_MAT4 isn't supported yet!\n");
			break;

			default:/*or MATERIAL_MAT2*/
				buf_push(&formats, &vulkan_format);
				buf_push(&offsets, &field_offset);
			break;
		}
		packed >>= bits_per_type;
		offset = field_offset + sizeof_glsl_type(type);
		location_offset++;
		buf_push(&locations, &location_offset);
	}

	buf_fit(&formats);
	buf_fit(&offsets);
	buf_fit(&locations);

	vulkan_vertex_buffer_layout_description_t info;
	info.input_rate = input_rate;
	info.attribute_count = buf_get_element_count(&formats);
	info.size = offset;
	info.attribute_formats = (info.attribute_count > 0) ? buf_get_ptr(&formats) : NULL;
	info.attribute_offsets = (info.attribute_count > 0) ? buf_get_ptr(&offsets) : NULL;
	info.attribute_locations = (info.attribute_count > 0) ? buf_get_ptr(&locations) : NULL;

	return info;
}

static vulkan_vertex_buffer_layout_description_t* decode_vulkan_vertex_infos(memory_allocator_t* allocator, u64* per_vertex_attribute_bindings,
												 		u32 per_vertex_attribute_binding_count,
												 		u64* per_instance_attribute_bindings,
												 		u32 per_instance_attribute_binding_count)
{
	// total number of vertex bindings must be equal to number of per vertex bindings + per instance bindings
	u32 binding_count = per_vertex_attribute_binding_count + per_instance_attribute_binding_count;

	// allocate memory
	vulkan_vertex_buffer_layout_description_t* vertex_infos = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_VERTEX_BUFFER_LAYOUT_DESCRIPTION_ARRAY, vulkan_vertex_buffer_layout_description_t, binding_count);
	safe_memzerov(vertex_infos, vulkan_vertex_buffer_layout_description_t, binding_count);
	
	u32 binding_number = 0;
	u16 location_number_offset = 0;

	// decode each 64 bit number for per vertex vertex bindings
	for(; binding_number < per_vertex_attribute_binding_count; binding_number++)
	{
		vertex_infos[binding_number] = decode_vulkan_vertex_info(allocator, per_vertex_attribute_bindings[binding_number], location_number_offset, VK_VERTEX_INPUT_RATE_VERTEX);
		vertex_infos[binding_number].binding = binding_number;
		location_number_offset += vertex_infos[binding_number].attribute_count;
	}
	// decode each 64 bit number for per instance vertex bindings
	for(; binding_number < binding_count; binding_number++)
	{
		vertex_infos[binding_number] = decode_vulkan_vertex_info(allocator, per_instance_attribute_bindings[binding_number], location_number_offset, VK_VERTEX_INPUT_RATE_INSTANCE);
		vertex_infos[binding_number].binding = binding_number;
		location_number_offset += vertex_infos[binding_number].attribute_count;
	}
	return vertex_infos;
}

/* determines if the list of material set bindings contains push constant binding */
static bool is_contains_push_constant(vulkan_shader_resource_description_t* material_set_bindings, u32 material_set_binding_count)
{
	for(u32 i = 0; i < material_set_binding_count; i++)
		if(material_set_bindings[i].is_push_constant)
			return true;
	return false;
}

static vulkan_push_constant_t create_vulkan_push_constant(memory_allocator_t* allocator, vulkan_shader_resource_description_t* material_set_bindings, u32 material_set_binding_count)
{
	// if there are no bindings in the material set then it is guaranteed that there won't be any push constant bindings
	// or it might also be possible that the list doesn't contain push constant bindings, so return empty in that case also
	if(!is_contains_push_constant(material_set_bindings, material_set_binding_count))
	{
		return (vulkan_push_constant_t)
		{
			.ranges = NULL,
			.range_count = 0,
			.buffer = NULL,
			.buffer_size = 0
		};
	}
	
	// holds the indices of the material set binding descriptors which are push constants
	u32* descriptor_indices = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, material_set_binding_count);

	// TODO: add an extra parameter is_zero, if true then each block allocated in the buffer would be first zeroed-out otherwise uninitialized
	BUFFER ranges = memory_allocator_buf_new(allocator, VkPushConstantRange);

	u32 range_count = 0;
	for(u32 i = 0; i < material_set_binding_count; i++)
	{
		vulkan_shader_resource_description_t* descriptor = &material_set_bindings[i];

		// if this is not a push constant then skip it
		if(!descriptor->is_push_constant) continue;

		// sanity check
		_debug_assert__(struct_descriptor_get_type(&descriptor->handle) == GLSL_TYPE_PUSH_CONSTANT);
		
		VkShaderStageFlagBits stage_flags = 0;
		if(descriptor->stage_flags & VULKAN_SHADER_STAGE_VERTEX_BIT)
			stage_flags |= VK_SHADER_STAGE_VERTEX_BIT;
		if(descriptor->stage_flags & VULKAN_SHADER_STAGE_FRAGMENT_BIT)
			stage_flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if(descriptor->stage_flags & VULKAN_SHADER_STAGE_GEOMETRY_BIT)
			stage_flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if(descriptor->stage_flags & VULKAN_SHADER_STAGE_TESSELLATION_BIT)
			stage_flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

		// create push constant range and added it to the list of ranges
		VkPushConstantRange range = 
		{
			.stageFlags = stage_flags,
			.offset = descriptor->push_constant_range_offset,
			.size = struct_descriptor_sizeof(&descriptor->handle)
		};
		buf_push(&ranges, &range);

		descriptor_indices[range_count] = i;
		++range_count;
	}

	buf_fit(&ranges);
	VkPushConstantRange max = DEREF_TO(VkPushConstantRange, buf_get_ptr(&ranges));
	
	// find the max (offset + size)
	for(u16 i = 1; i < range_count; i++)
	{
		VkPushConstantRange range = DEREF_TO(VkPushConstantRange, buf_get_ptr_at(&ranges, i));
		if((max.offset + max.size) < (range.offset + range.size))
			max = range;
	}

	// allocate memory with the size of the push_constant_buffer to (max.offset + max.size)
	void* push_constant_buffer = memory_allocator_alloc(allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, max.offset + max.size);

	// map each range
	for(u16 i = 0; i < range_count; i++)
	{
		VkPushConstantRange range = DEREF_TO(VkPushConstantRange, buf_get_ptr_at(&ranges, i));
		// offsets must be in inceasing order and should be in the bounds of the buffer size which is (max.offset + max.size)
		_debug_assert__(range.offset < (max.offset + max.size));

		// map
		struct_descriptor_map(&material_set_bindings[descriptor_indices[i]].handle, push_constant_buffer + range.offset);
	}

	memory_allocator_dealloc(allocator, descriptor_indices);

	// return the vulkan_push_constant_t object
	return (vulkan_push_constant_t)
	{
		.ranges = buf_get_ptr(&ranges),
		.range_count = range_count,
		.buffer = push_constant_buffer,
		.buffer_size = max.offset + max.size
	};
}

static void destroy_vulkan_push_constant(memory_allocator_t* allocator, vulkan_push_constant_t* push_constant)
{
	if((push_constant->ranges == NULL) || (push_constant->buffer == NULL))
		return;
	memory_allocator_dealloc(allocator, push_constant->ranges);
	memory_allocator_dealloc(allocator, push_constant->buffer);
}

static vulkan_shader_resource_description_t* create_deep_copy_of_set_binding_descriptors(memory_allocator_t* allocator, vulkan_shader_resource_description_t* descriptors, u32 descriptor_count)
{
	/* if we try to allocate any object of size 0 with memory_allocator_alloc_*, then it may lead to assertion failures */
	if(descriptor_count == 0)
		return NULL;
	vulkan_shader_resource_description_t* copy_descriptors = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RESOURCE_DESCRIPTION_ARRAY, vulkan_shader_resource_description_t, descriptor_count);
	for(u32 i = 0; i < descriptor_count; i++)
	{
		memcopy(&copy_descriptors[i], &descriptors[i], vulkan_shader_resource_description_t);
		copy_descriptors[i].handle = struct_descriptor_clone(&descriptors[i].handle);
	}
	return copy_descriptors;
}

static void destroy_vulkan_shader_resource_descriptions(memory_allocator_t* allocator, vulkan_shader_resource_description_t* descriptors, u32 descriptor_count)
{
	for(u32 i = 0; i < descriptor_count; i++)
		struct_descriptor_destroy(allocator, &descriptors[i].handle);
	if(descriptor_count > 0)
		memory_allocator_dealloc(allocator, descriptors);
}

typedef struct vulkan_pipeline_common_data_t
{
	vulkan_vertex_buffer_layout_description_t* vertex_attribute_bindings;
	u32 vertex_attribute_binding_count;
	VkPushConstantRange* push_constant_ranges;
	u32 push_constant_range_count;
	vulkan_descriptor_set_layout_t material_set_layout;
	vulkan_graphics_pipeline_description_t* pipeline_descriptions;
} vulkan_pipeline_common_data_t;

static u32 get_framebuffer_count_from_render_pass_type(vulkan_render_pass_type_t type, u32 swapchain_image_count)
{
	switch(type)
	{
		case VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER: return 1;
		case VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET: return swapchain_image_count;
		default:
			LOG_FETAL_ERR("Unsupported render pass type %u\n", type);
	}
	return 0;
}

static VkFormat get_image_format_from_attachment_type(vulkan_attachment_type_t type)
{
	switch(type)
	{
		case VULKAN_ATTACHMENT_TYPE_COLOR:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case VULKAN_ATTACHMENT_TYPE_DEPTH:
			return VK_FORMAT_D32_SFLOAT;
		case VULKAN_ATTACHMENT_TYPE_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_STENCIL isn't supported yet!\n");
		case VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL:
			LOG_FETAL_ERR("VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL isn't supported yet!\n");
		default:
			LOG_FETAL_ERR("Unsupported attachment type %u for any of the image format\n", type);
	}
	return VK_FORMAT_UNDEFINED;
}

static bool u32_list_contains(const u32* const list, const u32 count, const u32 value)
{
	for(u32 i = 0; i < count; i++)
		if(list[i] == value)
			return true;
	return false;
}

#define is_next_subpass_reads_any_color(pass, prev_pass) is_next_subpass_reads_any((pass), (prev_pass)->color_attachments, (prev_pass)->color_attachment_count)
#define is_next_subpass_reads_depth(pass, prev_pass) is_next_subpass_reads_any((pass), &(prev_pass)->depth_stencil_attachment, ((prev_pass)->depth_stencil_attachment == U32_MAX) ? 0 : 1)
#define is_next_subpass_writes_any_color(pass, prev_pass) is_next_subpass_writes_any((pass), (prev_pass)->color_attachments, (prev_pass)->color_attachment_count)
#define is_next_subpass_writes_depth(pass, prev_pass) is_next_subpass_writes_any((pass), &(prev_pass)->depth_stencil_attachment, ((prev_pass)->depth_stencil_attachment == U32_MAX) ? 0 : 1)

static bool is_next_subpass_reads_any(const vulkan_subpass_description_t* const pass, const u32* const attachments, u32 count)
{
	if(attachments == NULL) return false;
	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->input_attachments[i]))
			return true;
	return false;
}

static bool is_next_subpass_writes_any(const vulkan_subpass_description_t* const pass, const u32* const attachments, u32 count)
{
	for(u32 i = 0; i < pass->color_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->color_attachments[i]))
			return true;
	if(pass->depth_stencil_attachment != U32_MAX)
		if(u32_list_contains(attachments, count, pass->depth_stencil_attachment))
			return true;
	return false;
}

UNUSED_FUNCTION static bool is_renderpass_reads_any(const vulkan_render_pass_description_t* const pass, const u32* const attachments, u32 count)
{
	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(u32_list_contains(attachments, count, pass->input_attachments[i]))
			return true;
	return false;
}

UNUSED_FUNCTION static bool is_renderpass_writes_any(const vulkan_render_pass_description_t* const pass, const u32* const attachments, u32 count)
{
	/* currently not supported */
	return false;
}

static bool is_renderpass_reads_depth(const vulkan_render_pass_description_t* const pass, const vulkan_render_pass_description_t* const prev_pass)
{
	u32 depth_index;
	if((prev_pass->attachment_count > 0) && (prev_pass->attachments[prev_pass->attachment_count - 1] == VULKAN_ATTACHMENT_TYPE_DEPTH))
		depth_index = prev_pass->attachment_count - 1;
	else
	{
		log_wrn("No depth attachment found in the previou pass\n");
		return false;
	}

	for(u32 i = 0; i < pass->input_attachment_count; i++)
		if(pass->input_attachments[i] == depth_index)
				return true;

	return false;
}

static void try_add_dependency(BUFFER* dependencies, const VkSubpassDependency* const in_dependency)
{
	u32 count = buf_get_element_count(dependencies);
	VkSubpassDependency* best_match = NULL;
	for(u32 i = 0; i < count; i++)
	{
		VkSubpassDependency* dependency = buf_get_ptr_at_typeof(dependencies, VkSubpassDependency, i);
		if((dependency->srcSubpass == in_dependency->srcSubpass) && (dependency->dstSubpass == in_dependency->dstSubpass))
		{
			best_match = dependency;
			if(((dependency->srcStageMask & in_dependency->srcStageMask) == in_dependency->srcStageMask)
				&& ((dependency->dstStageMask & in_dependency->dstStageMask) == in_dependency->dstStageMask)
				&& ((dependency->srcAccessMask & in_dependency->srcAccessMask) == in_dependency->srcAccessMask)
				&& ((dependency->dstAccessMask & in_dependency->dstAccessMask) == in_dependency->dstAccessMask))
				return;
		}
	}

	if(best_match == NULL)
		buf_push(dependencies, (VkSubpassDependency*)in_dependency);
	else
	{
		best_match->srcStageMask |= in_dependency->srcStageMask;
		best_match->dstStageMask |= in_dependency->dstStageMask;
		best_match->srcAccessMask |= in_dependency->srcAccessMask;
		best_match->dstAccessMask |= in_dependency->dstAccessMask;
	}
}

static u32 get_most_recent_subpass_index_writing_to_depth(const vulkan_render_pass_description_t* pass)
{
	u32 index = U32_MAX;
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		vulkan_subpass_description_t* subpass = &pass->subpass_descriptions[i];
		if(subpass->depth_stencil_attachment != U32_MAX)
			index = i;
	}
	_debug_assert__(index != U32_MAX);
	return index;
}

static VkSubpassDependency* create_and_resolve_subpass_dependencies(vulkan_renderer_t* renderer, const vulkan_render_pass_description_t* pass, const vulkan_render_pass_description_t* next_pass, u32 OUT dependency_count)
{
	BUFFER dependencies = memory_allocator_buf_new(renderer->allocator, VkSubpassDependency);
	
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		vulkan_subpass_description_t* subpass = &pass->subpass_descriptions[i];
		vulkan_subpass_description_t* next_subpass = (i < (pass->subpass_count - 1)) ? (subpass + 1) : NULL;

		if(subpass->depth_stencil_attachment != U32_MAX)
		{
			/* 	let the image load operation (clear) happen first
				NOTE: VK_ATTACHMENT_LOAD_OP_CLEAR is a write operation 
			 */
			VkSubpassDependency dependency = 
			{ 
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			};

			try_add_dependency(&dependencies, &dependency);

			/* 	let the image layout transition happen first
				NOTE: VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL is a write operation) 
			 */
			dependency = (VkSubpassDependency)
			{
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.dstAccessMask = 0
			};

			try_add_dependency(&dependencies, &dependency);
		}

		if(subpass->color_attachment_count > 0)
		{
			/* 	let the image layout transition happen first
				NOTE: VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL is a write operation 
			*/
			VkSubpassDependency dependency = 
			{
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = i,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
			};

			try_add_dependency(&dependencies, &dependency);
		}

		if(next_subpass != NULL)
		{
			if(is_next_subpass_writes_any_color(next_subpass, subpass))
			{
				/* let the write from this subpass to complete then allow write in the next pass */
				VkSubpassDependency dependency =
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}

			if(is_next_subpass_reads_any_color(next_subpass, subpass))
			{
				/* let color write complete and then allow read in the next pass */
				VkSubpassDependency dependency =
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}

			/* 	if the next subpass writes to the same depth stencil attachment this subpass writes to, 
				add another subpass dependency to let this subpass write first and then the next subpass
			 */
			if(is_next_subpass_writes_depth(next_subpass, subpass))
			{
				VkSubpassDependency dependency = 
				{
					.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					.srcSubpass = i,
					.dstSubpass = i + 1,
					.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
					.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
				};

				try_add_dependency(&dependencies, &dependency);
			}
		}
	}

	if((next_pass != NULL) && is_renderpass_reads_depth(next_pass, pass))
	{
		/* let the write complete and then allow read in the next pass */
		VkSubpassDependency dependency = 
		{
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			.srcSubpass = get_most_recent_subpass_index_writing_to_depth(pass),
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
		};

		try_add_dependency(&dependencies, &dependency);
	}
	
	OUT dependency_count = CAST_TO(u32, buf_get_element_count(&dependencies));
	return CAST_TO(VkSubpassDependency*, buf_get_ptr(&dependencies));

}

typedef struct vulkan_render_pass_description_substitutions_t
{
	u32 subpass_dependency_count;
	VkSubpassDependency* subpass_dependencies;
} vulkan_render_pass_description_substitutions_t;

static vulkan_render_pass_create_info_builder_t* convert_render_pass_description_to_create_info(vulkan_renderer_t* renderer, vulkan_render_pass_description_t* pass, vulkan_render_pass_description_substitutions_t* subst, vulkan_render_pass_description_t* next_passes, u32 next_pass_count)
{
	vulkan_render_pass_create_info_builder_t* builder = vulkan_render_pass_create_info_builder_create(renderer->allocator);
	vulkan_render_pass_create_info_builder_add(builder, 1);
	vulkan_render_pass_create_info_builder_bind(builder, 0);

	// number of frame buffers to create
	u32 framebuffer_count = get_framebuffer_count_from_render_pass_type(pass->type, renderer->swapchain->image_count);
	vulkan_render_pass_create_info_builder_set_framebuffer_count(builder, framebuffer_count);
	
	// list of attachments to bound for each frame buffer
	_debug_assert__(pass->attachment_count > 0);

	u32 supplementary_attachment_bucket_count = 0;
	if(pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET)
	{
		_debug_assert__(framebuffer_count == renderer->swapchain->image_count);
		supplementary_attachment_bucket_count = 1;
		u32 supplementary_attachment_bucket_depth = renderer->swapchain->image_count;
		vulkan_render_pass_create_info_builder_set_supplementary_attachment_bucket(builder, supplementary_attachment_bucket_count, supplementary_attachment_bucket_depth);
		
		for(u32 i = 0; i < supplementary_attachment_bucket_count; i++)
			for(u32 j = 0; j < supplementary_attachment_bucket_depth; j++)
			{
				u32 stride = i * supplementary_attachment_bucket_depth;
				vulkan_render_pass_create_info_builder_set_supplementary_attachments(builder, j + stride, &renderer->swapchain->vo_image_views[j/* + stride */], 1);
			}
	}

	for(u32 i = 0; i < pass->attachment_count; i++)
	{
		bool is_supplementary = i < supplementary_attachment_bucket_count;


		/*
			TODO: STORE operation is expensive, this should be VK_ATTACHMENT_STORE_OP_DONT_CARE by default
			We can define the usage of an attachment upfront while defining the render pass description
			for the attachments that are supposed to be sampled by another shader.
			However the following algorithm creates a usage mask based on if they are being accessed in the
			next pass; but it doesn't know that if the attachment might be sampled in another pass or descriptor binding,
			for that reason we need to define the usage of such attachments upfront.

			However, swapchain color attachment is always stored so we only need to care about the depth attachment
			and that depth attachment must be global to all the render passes.
		 */
		VkAttachmentStoreOp store_op = VK_ATTACHMENT_STORE_OP_STORE;
		
		/* 	if the attachment is a color attachment and related to the swapchain target then it should be loaded first
			otherwise if the color attachment doesn't belong to the swapchain then it should be cleared first.
			if the attachment is a depth attachemnt, then we need to load it as camera pass has cleared it earlier.
		 */
		VkAttachmentLoadOp load_op = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ? 
									 (((pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET) && is_supplementary) ?
									  VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR) : VK_ATTACHMENT_LOAD_OP_LOAD;

		/*	if the attachment is a color attachment and related to the swapchain target then it should have OPTIMAL layout,
			as the driver will be free to choose the internal layout of the swapchain images to better utilize the hardware.
			if the color attachment doesn't belong to the swapchain target then it should have UNDEFINED layout.
			if the attachment is a depth attachment then it should be preserved hence VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		 */
		VkImageLayout initial_layout = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ? 
									 (((pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET) && is_supplementary) ?
									  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED) : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		/*	final layout shall be OPTIMAL to better utilize the hardware */
		VkImageLayout final_layout = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ? 
									VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : 
									VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		vulkan_attachment_next_pass_usage_t usage = VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE;

		/* supplementary attachments shall always be swapchain image */
		if(is_supplementary)
		{
			_debug_assert__(pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR);
			store_op = VK_ATTACHMENT_STORE_OP_STORE;
			usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT;
		}

		/* check if any subsequent subpasses read this attachment as subpassInput */
		bool is_outer_break = false;
		for(u32 j = 0; j < pass->subpass_description_count; j++)
		{
			u32 sub_input_count = pass->subpass_descriptions[j].input_attachment_count;
			for(u32 k = 0; k < sub_input_count; k++)
			{
				if(i == pass->subpass_descriptions[j].input_attachments[k])
				{
					/* NOTE: it is possible to read the swapchain'images in a subpass as subpassInput,
					 * one might argue that how can a subpass read and write to the same attachment? 
					 * it is not possible, but a subpass can definitly read the swapchain'image as subpassInput
					 * while writing to a depth only attachment. this way the two attachments would be different. */

					/* if yes, then the usage mask should contain subpassInput */
					usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT;
					is_outer_break = true;
					break;
				}
			}
			if(is_outer_break)
				break;
		}

		/* check if any subsequent passes sample this attachment or not */
		is_outer_break = false;
		for(u32 j = 0; j < next_pass_count; j++)
		{
			vulkan_render_pass_description_t* next_pass = &next_passes[j];
			u32 input_count = next_pass->input_attachment_count;
			for(u32 k = 0; k < input_count; k++)
			{
				if(next_pass->input_attachments[k] == i)
				{
					/* if yes, then this attachment should be preserved at the end of this pass */
					store_op = VK_ATTACHMENT_STORE_OP_STORE;
					/* and the final layout must be read only */
					final_layout = (pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_COLOR) ?
									VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL :
									VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					/* and the usage should be set as next pass sampled */
					usage |= VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED;

					/* if this is a depth attachment and, as in the current context, is read by any of the subsequent render passes */
					if(pass->attachments[i] == VULKAN_ATTACHMENT_TYPE_DEPTH)
					{
						/* then that means this render pass is supposed to be the first user as a writer on the depth attachment 
						 * and the subsequent render passes will be the second users as a reader of the depth attachment 
						 * however, a smart guy can notice that this won't work if two render passes write to the same depth attachment
						 * i.e.
						 * RenderPass
						 * {
						 *		Subpass
						 *		{
						 * 			depth write is enabled
						 * 			depth test is enabled
						 * 		}
						 * }
						 * RenderPass
						 * {	
						 *		Subpass
						 *		{
						 *			depth write is enabled
						 * 			depth test is enabled
						 *		}
						 * }
						 * [Read(depth, set = ..., binding = ...)]
						 * RenderPass
						 * {
						 * 		Subpass
						 * 		{
						 *			depth write is enabled
						 * 			depth test is enabled
						 *		}
						 * }
						 * 
						 * the above shader code is a nonsense with our current implementation
						 * we can't even disambiguate if the first two RenderPass(s) would use the same global depth buffer (created by the camera)
						 * or they'll have a common, but separate from the global depth buffer, depth buffer attachment.
						 */
						initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
						load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
					}
					is_outer_break = true;
					break;
				}
			}
			if(is_outer_break)
				break;
		}

		VkAttachmentDescription attachment_description =
		{
			.format = get_image_format_from_attachment_type(pass->attachments[i]),
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = load_op,
			.storeOp = store_op,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = initial_layout,
			.finalLayout = final_layout
		};
		vulkan_render_pass_create_info_builder_add_attachment_descriptions(builder, &attachment_description, 1);
		vulkan_render_pass_create_info_builder_add_attachment_usages(builder, &usage, 1);
	}

	vulkan_render_pass_create_info_builder_set_render_set_bindings(builder, pass->render_set_bindings, pass->render_set_binding_count);

	_debug_assert__(pass->subpass_description_count > 0);
	vulkan_subpass_create_info_builder_t* subpass_builder = vulkan_subpass_create_info_builder_create(renderer->allocator);
	vulkan_render_pass_create_info_builder_set_subpasses_builder(builder, subpass_builder, true);
	for(u32 i = 0; i < pass->subpass_count; i++)
	{
		vulkan_subpass_create_info_builder_add(subpass_builder, 1);
		vulkan_subpass_create_info_builder_bind(subpass_builder, i);

		vulkan_subpass_create_info_builder_set_render_set_bindings(subpass_builder, pass->subpass_descriptions[i].sub_render_set_bindings, pass->subpass_descriptions[i].sub_render_set_binding_count);
		vulkan_subpass_create_info_builder_set_bind_point(subpass_builder, VK_PIPELINE_BIND_POINT_GRAPHICS);
		
		for(u32 j = 0; j < pass->subpass_descriptions[i].color_attachment_count; j++)
		{
			u32 index = pass->subpass_descriptions[i].color_attachments[j];
			VkAttachmentReference color_attachment_reference = 
			{
				.attachment = index,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
			vulkan_subpass_create_info_builder_add_color_attachments(subpass_builder, &color_attachment_reference, 1);
		}

		for(u32 j = 0; j < pass->subpass_descriptions[i].input_attachment_count; j++)
		{
			u32 index = pass->subpass_descriptions[i].input_attachments[j];
			VkAttachmentReference input_attachment_reference = 
			{
				.attachment = index,
				.layout = (pass->attachments[index] == VULKAN_ATTACHMENT_TYPE_COLOR) ?
																	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL :
																	VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
			};
			vulkan_subpass_create_info_builder_add_input_attachments(subpass_builder, &input_attachment_reference, 1);
		}

		if(pass->subpass_descriptions[i].depth_stencil_attachment != U32_MAX)
		{
			_debug_assert__(pass->attachments[pass->subpass_descriptions[i].depth_stencil_attachment] == VULKAN_ATTACHMENT_TYPE_DEPTH);
			VkAttachmentReference depth_attachment_reference = 
			{
				.attachment = pass->subpass_descriptions[i].depth_stencil_attachment,
				.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			};
			vulkan_subpass_create_info_builder_set_depth_stencil_attachment(subpass_builder, depth_attachment_reference);
		}

		// for now we won't be using preserve attachments
	}

	/* apply substitutions if any */
	VkSubpassDependency* subpass_dependencies = pass->subpass_dependencies;
	u32 subpass_dependency_count = pass->subpass_dependency_count;
	if(subst->subpass_dependency_count > 0)
	{
		subpass_dependency_count = subst->subpass_dependency_count;
		subpass_dependencies = subst->subpass_dependencies;
	}

	if(subpass_dependency_count > 0)
		vulkan_render_pass_create_info_builder_set_dependencies(builder, subpass_dependencies, subpass_dependency_count);
	
	return builder;
}

static vulkan_shader_render_pass_t* create_shader_render_passes(vulkan_renderer_t* renderer, vulkan_render_pass_description_t* descriptions, vulkan_render_pass_description_substitutions_t* substs, u32 description_count, vulkan_pipeline_common_data_t* common_data)
{
	// allocate memory
	vulkan_shader_render_pass_t* passes = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RENDER_PASS_ARRAY, vulkan_shader_render_pass_t, description_count);

	BUFFER set_layouts = memory_allocator_buf_create(renderer->allocator, sizeof(VkDescriptorSetLayout), 1, 0);

	// push camera set layout, at CAMERA_SET = 0
	buf_push(&set_layouts, &renderer->camera_set_layout.vo_handle);

	// push global set layout, at GLOBAL_SET = 1
	buf_push(&set_layouts, &renderer->global_set_layout.vo_handle);

	vulkan_render_pass_pool_create_path(renderer->render_pass_pool);

	for(u32 i = 0; i < description_count; i++)
	{
		// create a render pass object in the pool, no duplicate render pass would be created

		/* number of subsequent render passes after this render pass */
		u32 next_count = description_count - i - 1;
		vulkan_render_pass_create_info_builder_t* create_info_builder = convert_render_pass_description_to_create_info(renderer, &descriptions[i], &substs[i], (next_count > 0) ? &descriptions[i + 1] : NULL, next_count);
		
		/* prepare render pass and submit input objects */
		u32 subpass_count = passes[i].subpass_count = descriptions[i].subpass_count;

		vulkan_render_pass_input_info_t input = { .input_attachment_count = descriptions[i].input_attachment_count };
		
		input.input_attachments = (input.input_attachment_count == 0) ? NULL : memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, input.input_attachment_count);
		if(input.input_attachments != NULL)
			memcopyv(input.input_attachments, descriptions[i].input_attachments, u32, descriptions[i].input_attachment_count);

		input.subpass_inputs = (subpass_count == 0) ? NULL : memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_INPUT_INFO_ARRAY, vulkan_subpass_input_info_t, subpass_count);
		for(u32 j = 0; j < subpass_count; j++)
		{
			vulkan_subpass_description_t* subpass_description = &descriptions[i].subpass_descriptions[j];
			input.subpass_inputs[j].input_attachment_count = subpass_description->input_attachment_count;
			input.subpass_inputs[j].input_attachments = (subpass_description->input_attachment_count == 0) ? NULL : memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, subpass_description->input_attachment_count);
			if(input.subpass_inputs[j].input_attachments != NULL)
				memcopyv(input.subpass_inputs[j].input_attachments, subpass_description->input_attachments, u32, subpass_description->input_attachment_count);
		}
		
		/* create render pass for this shader */
		passes[i].handle = vulkan_render_pass_pool_create_pass(renderer->render_pass_pool, vulkan_render_pass_create_info_builder_get(create_info_builder), &input);

		/* destroy allocations made in vulkan_render_pass_input_info_t */
		if(input.input_attachment_count > 0)
			memory_allocator_dealloc(renderer->allocator, input.input_attachments);
		if(subpass_count > 0)
		{
			for(u32 j = 0; j < subpass_count; j++)
				if(input.subpass_inputs[j].input_attachment_count > 0)
					memory_allocator_dealloc(renderer->allocator, input.subpass_inputs[j].input_attachments);
			memory_allocator_dealloc(renderer->allocator, input.subpass_inputs);
		}

		vulkan_render_pass_create_info_builder_destroy(create_info_builder);

		/* create pipeline layout and graphics pipelines */

		passes[i].subpasses = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_SUBPASS_ARRAY, vulkan_shader_subpass_t, subpass_count);

		vulkan_render_pass_t* render_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, passes[i].handle);

		// push render set layout, at RENDER_SET = 2
		buf_push(&set_layouts, &render_pass->render_set_layout.vo_handle);

		for(u32 j = 0; j < subpass_count; j++)
		{
			// push sub render set layout, at SUB_RENDER_SET = 3
			buf_push(&set_layouts, &render_pass->sub_render_set_layouts[j].vo_handle);
			// push material set layout, at MATERIAL_SET = 4
			buf_push(&set_layouts, &common_data->material_set_layout.vo_handle);
			// push object set layout, at OBJECT_SET = 5
			buf_push(&set_layouts, &renderer->object_set_layout.vo_handle);

			// create pipeline layout for this subpass
			vulkan_pipeline_layout_create_info_t layout_create_info =
			{
				.vo_push_constant_ranges = common_data->push_constant_ranges,
				.push_constant_range_count = common_data->push_constant_range_count,
				.vo_set_layouts = buf_get_ptr(&set_layouts),
				.set_layout_count = buf_get_element_count(&set_layouts)
			};
			passes[i].subpasses[j].pipeline_layout = vulkan_pipeline_layout_create(renderer, &layout_create_info);

			// pop out the set layouts { SUB_RENDER_SET, MATERIAL_SET, and OBJECT_SET }
			buf_pop(&set_layouts, NULL);
			buf_pop(&set_layouts, NULL);
			buf_pop(&set_layouts, NULL);

			vulkan_graphics_pipeline_description_t* pipeline_description = &common_data->pipeline_descriptions[descriptions[i].subpass_descriptions[j].pipeline_description_index];

			// create graphics pipeline for this subpass
			vulkan_graphics_pipeline_create_info_t pipeline_create_info = 
			{
				.layout = passes[i].subpasses[j].pipeline_layout,
				.settings = pipeline_description->settings,
				.spirv_codes = pipeline_description->spirv_codes,
				.spirv_code_count = pipeline_description->spirv_code_count,
				.vertex_attribute_bindings = common_data->vertex_attribute_bindings,
				.vertex_attribute_binding_count = common_data->vertex_attribute_binding_count,
				.render_pass = render_pass,
				.subpass_index = j
			};
			passes[i].subpasses[j].pipeline = vulkan_graphics_pipeline_create(renderer, &pipeline_create_info);
		}

		// pop out the RENDER_SET layout
		buf_pop(&set_layouts, NULL);
	}

	// pop out the GLOBAL_SET layout
	buf_pop(&set_layouts, NULL);

	// pop out the CAMERA_SET layout
	buf_pop(&set_layouts, NULL);

	buf_free(&set_layouts);
	
	return passes;
}

static void destroy_shader_render_passes(memory_allocator_t* allocator, vulkan_shader_render_pass_t* passes, u32 pass_count)
{
	for(u32 i = 0; i < pass_count; i++)
	{
		if(passes[i].subpass_count > 0)
		{
			for(u32 j = 0; j < passes[i].subpass_count; j++)
			{
				vulkan_pipeline_layout_destroy(passes[i].subpasses[j].pipeline_layout);
				vulkan_pipeline_layout_release_resources(passes[i].subpasses[j].pipeline_layout);
				vulkan_graphics_pipeline_destroy(passes[i].subpasses[j].pipeline);
				vulkan_graphics_pipeline_release_resources(passes[i].subpasses[j].pipeline);
			}
			memory_allocator_dealloc(allocator, passes[i].subpasses);
		}
	}
	if(pass_count > 0)
		memory_allocator_dealloc(allocator, passes);
}

/*	merges the subpass depedencies which targets the same subpass pair,
	and removes duplicate subpass dependencies.
 */
static VkSubpassDependency* merge_subpass_dependencies(vulkan_renderer_t* renderer, VkSubpassDependency* dependencies, u32 OUT dependency_count)
{
	BUFFER new_dependencies = memory_allocator_buf_new(renderer->allocator, VkSubpassDependency);

	u32 count = *dependency_count;
	for(u32 i = 0; i < count; i++)
		try_add_dependency(&new_dependencies, &dependencies[i]);

	OUT dependency_count = count = buf_get_element_count(&new_dependencies);

	return (count == 0) ? NULL : buf_get_ptr(&new_dependencies);
}

static void recreate_graphics_pipelines(void* publisher_data, void* handler_data)
{
	AUTO window = CAST_TO(render_window_t*, publisher_data);
	vulkan_graphics_pipeline_refresh_info_t refresh_info = 
	{
		.width = window->width,
		.height = window->height
	};

	AUTO shader = CAST_TO(vulkan_shader_t*, handler_data);
	for(u32 i = 0; i < shader->render_pass_count; i++)
	{
		u32 subpass_count = shader->render_passes[i].subpass_count;
		for(u32 j = 0; j < subpass_count; j++)
			vulkan_graphics_pipeline_refresh(shader->render_passes[i].subpasses[j].pipeline, &refresh_info);
	}
	debug_log_info("Graphics pipeline recreate success");
}

RENDERER_API vulkan_shader_t* vulkan_shader_create(vulkan_renderer_t* renderer, vulkan_shader_create_info_t* create_info)
{
	vulkan_shader_t* shader = vulkan_shader_new(renderer->allocator);
	shader->renderer = renderer;

	shader->handle = VULKAN_SHADER_HANDLE_INVALID;
	shader->push_constant = create_vulkan_push_constant(renderer->allocator, create_info->material_set_bindings, create_info->material_set_binding_count);
	VULKAN_OBJECT_INIT(&shader->material_set_layout, VULKAN_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, VULKAN_OBJECT_NATIONALITY_EXTERNAL);
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, create_info->material_set_bindings, create_info->material_set_binding_count, &shader->material_set_layout);
	shader->material_set_bindings = create_deep_copy_of_set_binding_descriptors(renderer->allocator, create_info->material_set_bindings, create_info->material_set_binding_count);
	shader->material_set_binding_count = create_info->material_set_binding_count;
	vulkan_pipeline_common_data_t common_data = 
	{
		.vertex_attribute_bindings = create_info->vertex_infos,
		.vertex_attribute_binding_count = create_info->vertex_info_count,
		.push_constant_ranges = shader->push_constant.ranges,
		.push_constant_range_count = shader->push_constant.range_count,
		.material_set_layout = shader->material_set_layout,
		.pipeline_descriptions = create_info->pipeline_descriptions
	};

	vulkan_render_pass_description_substitutions_t* substs = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_DESCRIPTION_SUBSTITUIONS_ARRAY, vulkan_render_pass_description_substitutions_t, create_info->render_pass_description_count);
	memzerov(substs, vulkan_render_pass_description_substitutions_t, create_info->render_pass_description_count);
	for(u32 i = 0; i < create_info->render_pass_description_count; i++)
	{
		vulkan_render_pass_description_t* pass = &create_info->render_pass_descriptions[i];
		vulkan_render_pass_description_t* next_pass = (i < (create_info->render_pass_description_count - 1)) ? (pass + 1) : NULL;
		/* if there is no explicit subpass dependency provided by the description then create and resolve internally */
		if(pass->subpass_dependency_count == 0)
			substs[i].subpass_dependencies = create_and_resolve_subpass_dependencies(renderer, pass, next_pass, &substs[i].subpass_dependency_count);
		/* otherwise merge the subpass dependencies as much as possible */
		else
			substs[i].subpass_dependencies = merge_subpass_dependencies(renderer, pass->subpass_dependencies, &substs[i].subpass_dependency_count);
	}
	shader->render_passes = create_shader_render_passes(renderer, create_info->render_pass_descriptions, substs, create_info->render_pass_description_count, &common_data);
	shader->render_pass_count = create_info->render_pass_description_count;
	for(u32 i = 0; i < create_info->render_pass_description_count; i++)
		if(substs[i].subpass_dependency_count > 0)
			memory_allocator_dealloc(renderer->allocator, substs[i].subpass_dependencies);
	memory_allocator_dealloc(renderer->allocator, substs);
	shader->pass_counter = 0;
	shader->subpass_counter = 0;

	event_subscription_create_info_t subscription_info = 
	{
		.handler = EVENT_HANDLER(recreate_graphics_pipelines),
		.handler_data = (void*)shader,
		.wait_for = SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT,
		.signal = SIGNAL_NOTHING_BIT
	};
	
	shader->pipeline_recreate_handle = event_subscribe(renderer->window->on_resize_event, &subscription_info);

	return shader;
}

static vulkan_shader_resource_description_t* load_descriptors(memory_allocator_t* allocator, binary_reader_t* reader, u32 OUT descriptor_count)
{
	// read the number of descriptors
	u16 count = binary_reader_u16(reader);

	OUT descriptor_count = count;

	// if there are no descriptors then return
	if(count == 0)
		return NULL;

	// allocate memory for the descriptors
	vulkan_shader_resource_description_t* descriptors = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RESOURCE_DESCRIPTION_ARRAY, vulkan_shader_resource_description_t, count);
	safe_memzerov(descriptors, vulkan_shader_resource_description_t, count);

	for(u16 i = 0; i < count; i++)
	{
		// reader the offset to the descriptor
		u32 offset = binary_reader_u32(reader);
		binary_reader_push(reader);
		binary_reader_jump(reader, offset);

		vulkan_shader_resource_description_t* descriptor = &descriptors[i];

		descriptor->set_number = binary_reader_u8(reader);
		descriptor->binding_number = binary_reader_u8(reader);
		// _debug_assert__(descriptor->set_number < 255);
		// _debug_assert__(descriptor->binding_number < 255);
		u32 descriptor_info = binary_reader_u32(reader);

		u32 bits = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_BITS(descriptor_info);
		descriptor->is_push_constant = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PUSH_CONSTANT_BIT & bits;
		descriptor->is_opaque = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_OPAQUE_BIT & bits;
		descriptor->is_uniform = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_UNIFORM_BUFFER_BIT & bits;
		descriptor->is_per_vertex_attribute = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_VERTEX_ATTRIB_BIT & bits;
		descriptor->is_per_instance_attribute = VULKAN_SHADER_RESOURCE_DESCRIPTOR_TYPE_PER_INSTANCE_ATTRIB_BIT & bits;
		struct_descriptor_t* struct_def = &descriptor->handle;
		OBJECT_INIT(struct_def, OBJECT_TYPE_STRUCT_DESCRIPTOR, OBJECT_NATIONALITY_EXTERNAL);

		descriptor->stage_flags = CAST_TO(u8, VULKAN_SHADER_STAGE_BITS(descriptor_info));
		
		u8 descriptor_type = VULKAN_GLSL_TYPE_BITS(descriptor_info);
		bool is_block = (descriptor_type == GLSL_TYPE_UNIFORM_BUFFER)
			|| (descriptor_type == GLSL_TYPE_STORAGE_BUFFER)
			|| (descriptor_type == GLSL_TYPE_PUSH_CONSTANT);

		if(is_block)
		{
			// ignore the block name
			CAN_BE_UNUSED_VARIABLE const char* name = binary_reader_str(reader);
			_debug_assert__(strlen(name) < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
		}

		// get the name of the descriptor
		const char* name = binary_reader_str(reader);
		_debug_assert__(strlen(name) < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
		struct_descriptor_begin(allocator, struct_def, name, descriptor_type);

		log_msg("Descriptor[%u]: (set = %u, binding = %u), stage_flags = %u, is_push_constant = %s, is_uniform = %s, is_opaque = %s, is_block = %s, name = %s\n", 
			i, descriptor->set_number, descriptor->binding_number, descriptor->stage_flags,
			descriptor->is_push_constant ? "true" : "false", descriptor->is_uniform ? "true" : "false", descriptor->is_opaque ? "true" : "false", is_block ? "true" : "false",
			descriptor->handle.name);

		// if the descriptor is a block then iterate through its fields
		if(is_block)
		{
			// get the number of fields in this uniform block
			u32 field_count = binary_reader_u16(reader);
			log_msg("Field Count: %u\n", field_count);

			for(u16 j = 0; j < field_count; j++)
			{
				// get the field info
				u32 field_info = binary_reader_u32(reader);
				
				// get the type of the fields
				u8 type = field_info & 0xFFUL;
				
				// get the name of this field
				const char* name = binary_reader_str(reader);
				_debug_assert__(strlen(name) < STRUCT_FIELD_MAX_NAME_SIZE);
				struct_descriptor_add_field(struct_def, name, type);
			}
		}
		struct_descriptor_end(allocator, struct_def);
		struct_descriptor_recalculate(struct_def);
		binary_reader_pop(reader);
	}
	return descriptors;
}

static vulkan_vertex_buffer_layout_description_t* create_vertex_infos(memory_allocator_t* allocator, binary_reader_t* reader, u32 OUT count)
{
	u32 descriptor_count;
	vulkan_shader_resource_description_t* descriptors = load_descriptors(allocator, reader, &descriptor_count);

	if(descriptor_count == 0)
	{
		// set the number of bindings to zero
		OUT count = 0;
		return NULL;
	}

	// allocate memory
	/* list of vulkan_vertex_buffer_layout_description_t objects */
	buffer_t vertex_infos = memory_allocator_buf_new(allocator, vulkan_vertex_buffer_layout_description_t);

	typedef struct attribute_info_t { BUFFER locations, formats, offsets; u32 largest_element_size; } attribute_info_t;
	dictionary_t bindings = dictionary_create(u32, attribute_info_t, 1, dictionary_key_comparer_u32);

	// iterate through each vertex attribute across all the vertex bindings
	for(u32 i = 0; i < descriptor_count; i++)
	{
		// this must be an attribute
		_debug_assert__(vulkan_shader_resource_description_is_attribute(&descriptors[i]));

		// use vertex binding number as a key
		u32 key = descriptors[i].vertex_attrib_binding_number;
		// if the binding number 'key' is not present then create new attribute_info_t object
		if(!dictionary_contains(&bindings, &key))
		{
			attribute_info_t info = 
			{ 
				.locations = memory_allocator_buf_create(allocator, sizeof(u32), 0, 0), 
				.formats = memory_allocator_buf_create(allocator, sizeof(VkFormat), 0, 0), 
				.offsets = memory_allocator_buf_create(allocator, sizeof(u32), 0, 0),
				.largest_element_size = 0
			};
			dictionary_push(&bindings, &key, &info);

			// add zero offset to be popped later
			u32 offset = 0;
			attribute_info_t* attribute_info = dictionary_get_value_ptr_at(&bindings, dictionary_get_count(&bindings) - 1);
			buf_push(&attribute_info->offsets, &offset);

			// create vertex info object for the binding 'key'
			AUTO vertex_info = CAST_TO(vulkan_vertex_buffer_layout_description_t*, buf_create_element(&vertex_infos));
			*vertex_info = (vulkan_vertex_buffer_layout_description_t)
			{
				.binding = key,
				.input_rate = descriptors[i].is_per_vertex_attribute ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE
			};
		}

		// get pointer to the attribute info object for binding 'key'
		attribute_info_t* attribute_info = dictionary_get_value_ptr(&bindings, &key);

		u32 location = descriptors[i].vertex_attrib_location_number;

		// TODO: These checks should be done at the compilation time, not at the runtime
		if(buf_find_index_of(&attribute_info->locations, &location, buf_u32_comparer) != BUF_INVALID_INDEX)
			LOG_FETAL_ERR("Multiple vertex attributes have the same layout location \"%u\", which is not allowed!\n", location);
			
		// add the location
		buf_push(&attribute_info->locations, &location);
		
		glsl_type_t glsl_type = descriptors[i].handle.type;
		
		// add the format
		VkFormat format = vkformatof_glsl_type(glsl_type);
		buf_push(&attribute_info->formats, &format);

		u32 offset;
		// pop the previous offset
		buf_pop(&attribute_info->offsets, &offset);
		// snap the offset to the correct alignment for this field/attribute
		u32 alignment = alignof_glsl_type(glsl_type);
		offset += (alignment - (offset % alignment)) % alignment;
		// add the offset
		buf_push(&attribute_info->offsets, &offset);
		// increment the offset by the size of this field/attribute
		u32 size = sizeof_glsl_type(glsl_type);
		if(max(alignment, size) > attribute_info->largest_element_size)
			attribute_info->largest_element_size = max(alignment, size);
		offset += size;
		// save the offset for next iteration
		buf_push(&attribute_info->offsets, &offset);
	}

	u32 binding_count = dictionary_get_count(&bindings);
	_debug_assert__(buf_get_element_count(&vertex_infos) == binding_count);
	for(u32 i = 0; i < binding_count; i++)
	{
		vulkan_vertex_buffer_layout_description_t* vinfo = buf_get_ptr_at_typeof(&vertex_infos, vulkan_vertex_buffer_layout_description_t, i);
		attribute_info_t* ainfo = dictionary_get_value_ptr_at(&bindings, i);

		// the final stride would be the last saved offset
		u32 offset = *(u32*)buf_peek_ptr(&ainfo->offsets);
		vinfo->size = offset + (ainfo->largest_element_size - offset % ainfo->largest_element_size) % ainfo->largest_element_size;
		// assign the number of attribute count
		vinfo->attribute_count = buf_get_element_count(&ainfo->locations);
		// assign the internal pointers to their corresponding destinations
		vinfo->attribute_locations = CAST_TO(u32*, buf_get_ptr(&ainfo->locations));
		vinfo->attribute_formats = CAST_TO(VkFormat*, buf_get_ptr(&ainfo->formats));
		vinfo->attribute_offsets = CAST_TO(u32*, buf_get_ptr(&ainfo->offsets));
	}

	// set the number of bindings
	OUT count = binding_count;

	// free the temporary dictionary buffer
	dictionary_free(&bindings);

	// destroy the vulkan resource descriptors because we are done creating vertex infos out of them
	destroy_vulkan_shader_resource_descriptions(allocator, descriptors, descriptor_count);

	// return pointer to the final vulkan vertex info objects
	return (binding_count == 0) ? NULL : buf_get_ptr(&vertex_infos);
}

static void destroy_vertex_infos(memory_allocator_t* allocator, vulkan_vertex_buffer_layout_description_t* vertex_infos, u32 vertex_info_count)
{
	if(vertex_info_count == 0)
		return;
	for(u32 i = 0; i < vertex_info_count; i++)
	{
		vulkan_vertex_buffer_layout_description_t* vinfo = &vertex_infos[i];
		if(vinfo->attribute_count > 0)
		{
			memory_allocator_dealloc(allocator, vinfo->attribute_locations);
			memory_allocator_dealloc(allocator, vinfo->attribute_formats);
			memory_allocator_dealloc(allocator, vinfo->attribute_offsets);
		}
	}
	memory_allocator_dealloc(allocator, vertex_infos);
}

static vulkan_spirv_code_t* load_spirv_codes(memory_allocator_t* allocator, binary_reader_t* reader, u32 OUT count)
{
	// calculate the number of shader stages (vertex, geometry, fragment, etc) present in the shader binary
	u32 shader_mask = binary_reader_u8(reader);
	u32 code_count = 0; 
	for(u32 i = 0; i < VULKAN_SHADER_STAGE_MAX; i++)
		if(shader_mask & BIT32(i))
			code_count++;
	OUT count = code_count;

	if(code_count == 0)
		return NULL;

	vulkan_spirv_code_t* codes = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SPIRV_CODE_ARRAY, vulkan_spirv_code_t, code_count);
	for(u8 i = 0, count = 0; i < VULKAN_SHADER_STAGE_MAX; i++)
	{
		u32 bit = shader_mask & BIT32(i);
		if(bit == 0) continue;

		vulkan_spirv_code_t* code = &codes[count++];

		switch(bit)
		{
			/* vertex shader spirv */
			case VULKAN_SHADER_STAGE_VERTEX_BIT:
				code->type = VULKAN_SHADER_TYPE_VERTEX;
				break;
			/* tessellation shader spirv */
			case VULKAN_SHADER_STAGE_TESSELLATION_BIT:
				code->type = VULKAN_SHADER_TYPE_TESSELLATION;
			break;
			/* geometry shader spirv */
			case VULKAN_SHADER_STAGE_GEOMETRY_BIT:
				code->type = VULKAN_SHADER_TYPE_GEOMETRY;
			break;
			/* fragment shader spirv */
			case VULKAN_SHADER_STAGE_FRAGMENT_BIT:
				code->type = VULKAN_SHADER_TYPE_FRAGMENT;
			break;
			default:
				// TODO: improve diagnostic messages (it should be with file name and LOC)
				LOG_FETAL_ERR("[Shader Loader] Invalid shader stage bit \"%u\" in shader_mask\n", bit);
			break;
		}

		code->length = binary_reader_u32(reader);
		u32 offset = binary_reader_u32(reader);
		binary_reader_push(reader);
		binary_reader_jump(reader, offset);
		code->spirv = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, code->length);
		memcopyv(code->spirv, __binary_reader_read(reader, code->length), u8, code->length);
		binary_reader_pop(reader);
	}
	
	return codes;
}

static void destroy_spirv_code(memory_allocator_t* allocator, vulkan_spirv_code_t* spirv_code)
{
	_debug_assert__(spirv_code->length > 0);
	memory_allocator_dealloc(allocator, spirv_code->spirv);
}

static u32 create_add_pipeline_description(memory_allocator_t* allocator, binary_reader_t* reader, BUFFER* pipeline_descriptions)
{
	buf_push_pseudo(pipeline_descriptions, 1);
	vulkan_graphics_pipeline_description_t* description = buf_peek_ptr(pipeline_descriptions);

	u32 offset = binary_reader_u32(reader);
	binary_reader_push(reader);
	binary_reader_jump(reader, offset);

	// read vulkan_graphics_pipeline_description_t
	const vulkan_graphics_pipeline_settings_t* pipeline = binary_reader_read(reader, vulkan_graphics_pipeline_settings_t);
	description->settings = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_GRAPHICS_PIPELINE_SETTINGS, vulkan_graphics_pipeline_settings_t);
	memcopy(description->settings, pipeline, vulkan_graphics_pipeline_settings_t);

	// allocating anything with zero size causes assertion failure, so always check for positive value
	if(description->settings->colorblend.attachmentCount > 0)
	{
		VkPipelineColorBlendAttachmentState* attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_PIPELINE_COLOR_BLEND_ATTACHMENT_STATE_ARRAY, VkPipelineColorBlendAttachmentState, description->settings->colorblend.attachmentCount);
		u32 attachments_size = sizeof(VkPipelineColorBlendAttachmentState) * description->settings->colorblend.attachmentCount;
		memcopyv(attachments, __binary_reader_read(reader, attachments_size), u8, attachments_size);
		description->settings->colorblend.pAttachments = attachments;
	}

	VkRect2D* scissors = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_RECT2D_ARRAY, VkRect2D, description->settings->viewport.scissorCount);
	u32 scissors_size = sizeof(VkRect2D) * description->settings->viewport.scissorCount;
	memcopyv(scissors, __binary_reader_read(reader, scissors_size), u8, scissors_size);
	description->settings->viewport.pScissors = scissors;

	VkViewport* viewports = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_VIEWPORT_ARRAY,  VkViewport, description->settings->viewport.viewportCount);
	u32 viewports_size = sizeof(VkViewport) * description->settings->viewport.viewportCount;
	memcopyv(viewports, __binary_reader_read(reader, viewports_size), u8, viewports_size);
	description->settings->viewport.pViewports = viewports;

	// read the spirv code array
	description->spirv_codes = load_spirv_codes(allocator, reader, &description->spirv_code_count);

	binary_reader_pop(reader);
	return buf_get_element_count(pipeline_descriptions) - 1;
}

static void destroy_pipeline_descriptions(memory_allocator_t* allocator, vulkan_graphics_pipeline_description_t* descriptions, u32 description_count)
{
	for(u32 i = 0; i < description_count; i++)
	{
		AUTO description = &descriptions[i];
		_debug_assert__(description->settings != NULL);
		if(description->settings->colorblend.attachmentCount > 0)
			memory_allocator_dealloc(allocator, CAST_TO(VkPipelineColorBlendAttachmentState*, description->settings->colorblend.pAttachments));
		if(description->settings->viewport.scissorCount > 0)
			memory_allocator_dealloc(allocator, CAST_TO(VkRect2D*, description->settings->viewport.pScissors));
		if(description->settings->viewport.viewportCount > 0)
			memory_allocator_dealloc(allocator, CAST_TO(VkViewport*, description->settings->viewport.pViewports));
		for(u32 i = 0; i < description->spirv_code_count; i++)
			destroy_spirv_code(allocator, &description->spirv_codes[i]);
		if(description->spirv_code_count > 0)
			memory_allocator_dealloc(allocator, description->spirv_codes);
		memory_allocator_dealloc(allocator, description->settings);
	}
	if(description_count > 0)
		memory_allocator_dealloc(allocator, descriptions);
}

static vulkan_subpass_description_t* create_subpass_descriptions(memory_allocator_t* allocator, binary_reader_t* reader, u32 OUT count, BUFFER* pipeline_descriptions)
{
	u32 description_count = binary_reader_u32(reader);
	OUT count = description_count;
	if(description_count == 0)
		return NULL;

	vulkan_subpass_description_t* descriptions = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SUBPASS_DESCRIPTION_ARRAY, vulkan_subpass_description_t, description_count);
	safe_memzerov(descriptions, vulkan_subpass_description_t, description_count);

	for(u32 i = 0; i < description_count; i++)
	{
		vulkan_subpass_description_t* description = &descriptions[i];

		// read the input attachment array
		description->input_attachment_count = binary_reader_u32(reader);
		if(description->input_attachment_count > 0)
		{
			description->input_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, description->input_attachment_count);
			for(u32 j = 0; j < description->input_attachment_count; j++)
				description->input_attachments[j] = binary_reader_u32(reader);
		}

		// read the color attachment array
		description->color_attachment_count = binary_reader_u32(reader);
		if(description->color_attachment_count > 0)
		{
			description->color_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, description->color_attachment_count);
			for(u32 j = 0; j < description->color_attachment_count; j++)
				description->color_attachments[j] = binary_reader_u32(reader);
		}

		// read the preserve attachment array
		description->preserve_attachment_count = binary_reader_u32(reader);
		if(description->preserve_attachment_count > 0)
		{
			description->preserve_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, description->preserve_attachment_count);
			for(u32 j = 0; j < description->preserve_attachment_count; j++)
				description->preserve_attachments[j] = binary_reader_u32(reader);
		}	

		description->depth_stencil_attachment = binary_reader_u32(reader);
		description->pipeline_description_index = create_add_pipeline_description(allocator, reader, pipeline_descriptions);
		
		description->sub_render_set_bindings = load_descriptors(allocator, reader, &description->sub_render_set_binding_count);	
	}

	return descriptions;
}

static void destroy_subpass_descriptions(memory_allocator_t* allocator, vulkan_subpass_description_t* descriptions, u32 description_count)
{
	for(u32 i = 0; i < description_count; i++)
	{
		AUTO description = &descriptions[i];
		if(description->input_attachment_count > 0)
			memory_allocator_dealloc(allocator, description->input_attachments);
		if(description->color_attachment_count > 0)
			memory_allocator_dealloc(allocator, description->color_attachments);
		if(description->preserve_attachment_count > 0)
			memory_allocator_dealloc(allocator, description->preserve_attachments);
		destroy_vulkan_shader_resource_descriptions(allocator, description->sub_render_set_bindings, description->sub_render_set_binding_count);
	}
	if(description_count > 0)
		memory_allocator_dealloc(allocator, descriptions);
}

static vulkan_render_pass_description_t* create_render_pass_descriptions(memory_allocator_t* allocator, binary_reader_t* reader, u32 OUT count, BUFFER* pipeline_descriptions)
{
	u32 description_count = binary_reader_u32(reader);
	OUT count = description_count;
	if(description_count == 0)
		return NULL;

	vulkan_render_pass_description_t* descriptions = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_DESCRIPTION_ARRAY, vulkan_render_pass_description_t, description_count);
	safe_memzerov(descriptions, vulkan_render_pass_description_t, description_count);

	for(u32 i = 0; i < description_count; i++)
	{
		vulkan_render_pass_description_t* description = &descriptions[i];

		// read the type of the render pass
		description->type = CAST_TO(vulkan_render_pass_type_t, binary_reader_u32(reader));

		// read the input attachment array
		description->input_attachment_count = binary_reader_u32(reader);
		if(description->input_attachment_count > 0)
		{
			description->input_attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, description->input_attachment_count);
			for(u32 j = 0; j < description->input_attachment_count; j++)
				description->input_attachments[j] = binary_reader_u32(reader);
		}

		// read the attachment type array
		description->attachment_count = binary_reader_u32(reader);
		if(description->attachment_count > 0)
		{
			description->attachments = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_ATTACHMENT_TYPE_ARRAY, vulkan_attachment_type_t, description->attachment_count);
			for(u32 j = 0; j < description->attachment_count; j++)
				description->attachments[j] = CAST_TO(vulkan_attachment_type_t, binary_reader_u32(reader));
		}

		// read the subpass dependency array
		description->subpass_dependency_count = binary_reader_u32(reader);
		if(description->subpass_dependency_count > 0)
		{
			description->subpass_dependencies = memory_allocator_alloc_obj_array(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_SUBPASS_DEPENDENCY_ARRAY, VkSubpassDependency, description->subpass_dependency_count);
			for(u32 j = 0; j < description->subpass_dependency_count; j++)
				memcopy(&description->subpass_dependencies[j], binary_reader_read(reader, VkSubpassDependency), VkSubpassDependency);
		}
		
		// read the render set binding
		description->render_set_bindings = load_descriptors(allocator, reader, &description->render_set_binding_count);

		// read the subpass descriptions array
		description->subpass_descriptions = create_subpass_descriptions(allocator, reader, &description->subpass_count, pipeline_descriptions);
	}

	return descriptions;
}

static void destroy_render_pass_descriptions(memory_allocator_t* allocator, vulkan_render_pass_description_t* descriptions, u32 description_count)
{
	for(u32 i = 0; i < description_count; i++)
	{
		AUTO description = &descriptions[i];
		if(description->input_attachment_count > 0)
			memory_allocator_dealloc(allocator, description->input_attachments);
		if(description->attachment_count > 0)
			memory_allocator_dealloc(allocator, description->attachments);
		if(description->subpass_dependency_count > 0)
			memory_allocator_dealloc(allocator, description->subpass_dependencies);
		destroy_vulkan_shader_resource_descriptions(allocator, description->render_set_bindings, description->render_set_binding_count);
		destroy_subpass_descriptions(allocator, description->subpass_descriptions, description->subpass_description_count);
	}
	if(description_count > 0)
		memory_allocator_dealloc(allocator, descriptions);
}

typedef struct header_t
{
	u32 sb_version;
	u32 sl_version;
} header_t;

static header_t* read_header(memory_allocator_t* allocator,  binary_reader_t* reader)
{
	if(strncmp(binary_reader_str(reader), "V3D Shader Binary", strlen("V3D Shader Binary")) != 0)
		LOG_FETAL_ERR("[Shader Loader] Invalid file header, file is broken");

	header_t* header = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_HEADER, header_t);
	u32 cmd;
	header->sb_version = U32_MAX;
	header->sl_version = U32_MAX;
	u32 count = binary_reader_u32(reader);
	while(count > 0)
	{
		cmd = binary_reader_u32(reader);
		/* shader binary */
		switch(cmd)
		{
			case 1:
			{
				/* version of the sb */
				if((header->sb_version = binary_reader_u32(reader)) != 2022)
					LOG_FETAL_ERR("[Shader Loader] Invalid shader binary version, renderer-1.0 only supports version 2022");
				break;
			}

			case 2:
			{
				/* skip the version of the shader language as we don't need in dissembling the file */
				header->sl_version = binary_reader_u32(reader);
				break;
			}
			
			default:
			{
				LOG_FETAL_ERR("[Shader Loader] Unable to find the shader binary version, file is broken");
				break;
			}
		}
		if((header->sb_version != U32_MAX) && (header->sl_version != U32_MAX))
			break;
		--count;
	}
	return header;
}

static vulkan_shader_create_info_t* convert_load_info_to_create_info(memory_allocator_t* allocator, vulkan_shader_load_info_t* load_info)
{
	binary_reader_t* reader = binary_reader_create(load_info->data, load_info->data_size);

	CAN_BE_UNUSED_VARIABLE AUTO header = read_header(allocator, reader);
	/* use header to determine binary spec version and choose the loader appropriatly, 
	 * but for now, we don't maintain such versioning - everything changes for now */
	memory_allocator_dealloc(allocator, header);

	// load resource descriptors for material
	u32 descriptor_count;
	vulkan_shader_resource_description_t* descriptors = load_descriptors(allocator, reader, &descriptor_count);

	// load the next resource descriptors meant for vertex attributes and create vertex buffer layout descriptions
	u32 vertex_info_count;
	vulkan_vertex_buffer_layout_description_t* vertex_infos = load_info->is_vertex_attrib_from_file ? 
										create_vertex_infos(allocator, reader, &vertex_info_count) :
										decode_vulkan_vertex_infos(allocator, load_info->per_vertex_attribute_bindings,
																   load_info->per_vertex_attribute_binding_count,
																   load_info->per_instance_attribute_bindings,
																   load_info->per_instance_attribute_binding_count);
	if(!load_info->is_vertex_attrib_from_file)
		vertex_info_count = load_info->per_vertex_attribute_binding_count + load_info->per_instance_attribute_binding_count;

	// create an empty list of graphics pipeline descriptions
	BUFFER pipeline_descriptions = memory_allocator_buf_new(allocator, vulkan_graphics_pipeline_description_t);

	// create render pass descriptions
	u32 render_pass_description_count;
	vulkan_render_pass_description_t* render_pass_descriptions = create_render_pass_descriptions(allocator, reader, &render_pass_description_count, &pipeline_descriptions);

	buf_fit(&pipeline_descriptions);

	// create vulkan shader
	vulkan_shader_create_info_t* create_info = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_CREATE_INFO, vulkan_shader_create_info_t);
	*create_info = (vulkan_shader_create_info_t)
	{
		.material_set_bindings = descriptors,
		.material_set_binding_count = descriptor_count,

		.vertex_infos = vertex_infos,
		.vertex_info_count = vertex_info_count,

		.render_pass_descriptions = render_pass_descriptions,
		.render_pass_description_count = render_pass_description_count,

		.pipeline_description_count = buf_get_element_count(&pipeline_descriptions),
		.pipeline_descriptions = CAST_TO(vulkan_graphics_pipeline_description_t*, buf_get_ptr(&pipeline_descriptions))
	};

	binary_reader_destroy(reader);
	binary_reader_release_resources(reader);

	return create_info;
}

static void destroy_vulkan_shader_create_info(memory_allocator_t* allocator, vulkan_shader_create_info_t* create_info)
{
	destroy_vulkan_shader_resource_descriptions(allocator, create_info->material_set_bindings, create_info->material_set_binding_count);
	destroy_vertex_infos(allocator, create_info->vertex_infos, create_info->vertex_info_count);
	/* pipeline descriptions may be NULL, i.e. no subpasses in any of the render passes, or no render passes at all. 
	 * if such cases exists, then we may remove this assertion in future. */
	_debug_assert__(create_info->pipeline_descriptions != NULL);
	destroy_pipeline_descriptions(allocator, create_info->pipeline_descriptions, create_info->pipeline_description_count);
	destroy_render_pass_descriptions(allocator, create_info->render_pass_descriptions, create_info->render_pass_description_count);
	memory_allocator_dealloc(allocator, create_info);
}

RENDERER_API vulkan_shader_t* vulkan_shader_load(vulkan_renderer_t* renderer, vulkan_shader_load_info_t* load_info)
{
	vulkan_shader_create_info_t* create_info = NULL;
	if((load_info->data != NULL) && (load_info->data_size != 0))
		create_info = convert_load_info_to_create_info(renderer->allocator, load_info);
	else if(load_info->path != NULL)
	{
		BUFFER* buffer = load_binary_from_file(load_info->path);
		
		vulkan_shader_load_info_t _load_info;
		memcopy(&_load_info, load_info, vulkan_shader_load_info_t);
		_load_info.data = buffer->bytes;
		_load_info.data_size = buffer->element_count;

		create_info = convert_load_info_to_create_info(renderer->allocator, &_load_info);
		buf_free(buffer);
	}
	else 
		LOG_FETAL_ERR("Unable to get the shader file data neither by a memory pointer nor by a file path\n");
	AUTO shader = vulkan_shader_create(renderer, create_info);
	destroy_vulkan_shader_create_info(renderer->allocator, create_info);
	return shader;
}

RENDERER_API void vulkan_shader_destroy(vulkan_shader_t* shader)
{
	event_unsubscribe(shader->renderer->window->on_resize_event, shader->pipeline_recreate_handle);

	shader->handle = VULKAN_SHADER_HANDLE_INVALID;
	vulkan_descriptor_set_layout_destroy(&shader->material_set_layout);
	destroy_shader_render_passes(shader->renderer->allocator, shader->render_passes, shader->render_pass_count);
}

RENDERER_API void vulkan_shader_release_resources(vulkan_shader_t* shader)
{
	destroy_vulkan_push_constant(shader->renderer->allocator, &shader->push_constant);
	destroy_vulkan_shader_resource_descriptions(shader->renderer->allocator, shader->material_set_bindings, shader->material_set_binding_count);
	if(VULKAN_OBJECT_IS_INTERNAL(shader))
		memory_allocator_dealloc(shader->renderer->allocator, shader);
}

RENDERER_API vulkan_graphics_pipeline_t* vulkan_shader_get_pipeline(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index)
{
	u32 count = shader->render_pass_count;
	for(u32 i = 0; i < count; i++)
	{
		if(shader->render_passes[i].handle == handle)
		{
			_debug_assert__(subpass_index < shader->render_passes[i].subpass_count);
			return shader->render_passes[i].subpasses[subpass_index].pipeline;
		}
	}
	return NULL;
}

RENDERER_API vulkan_pipeline_layout_t* vulkan_shader_get_pipeline_layout(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle, u32 subpass_index)
{
	u32 count = shader->render_pass_count;
	for(u32 i = 0; i < count; i++)
	{
		if(shader->render_passes[i].handle == handle)
		{
			_debug_assert__(subpass_index < shader->render_passes[i].subpass_count);
			return shader->render_passes[i].subpasses[subpass_index].pipeline_layout;
		}
	}
	return NULL;
}

RENDERER_API vulkan_render_pass_handle_t vulkan_shader_get_prev_pass_handle(vulkan_shader_t* shader)
{
	return (shader->pass_counter == 0) ? VULKAN_RENDER_PASS_HANDLE_INVALID : shader->render_passes[shader->pass_counter - 1].handle;
}

RENDERER_API bool vulkan_shader_render_pass_is_next(vulkan_shader_t* shader, vulkan_render_pass_handle_t handle)
{
	AUTO pass = &shader->render_passes[shader->pass_counter];
	if((shader->pass_counter < shader->render_pass_count) && (pass->handle == handle))
	{
		shader->subpass_counter++;
		if(shader->subpass_counter == pass->subpass_count)
		{
			shader->pass_counter++;
			shader->subpass_counter = 0;
		}
		return true;
	}
	return false;
}

RENDERER_API void vulkan_shader_render_pass_counter_reset(vulkan_shader_t* shader)
{
	shader->pass_counter = 0;
}
